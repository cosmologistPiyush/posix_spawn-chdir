/*	$NetBSD: iopsp.c,v 1.2 2000/11/09 12:51:36 ad Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Raw SCSI/FC-AL device support for I2O.  I2O presents SCSI devices
 * individually; we group them by controller port.
 */

#include "opt_i2o.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/queue.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/endian.h>
#include <sys/malloc.h>
#include <sys/scsiio.h>

#include <machine/bswap.h>
#include <machine/bus.h>

#include <dev/scsipi/scsi_all.h>
#include <dev/scsipi/scsi_disk.h>
#include <dev/scsipi/scsipi_all.h>
#include <dev/scsipi/scsiconf.h>

#include <dev/i2o/i2o.h>
#include <dev/i2o/iopreg.h>
#include <dev/i2o/iopvar.h>
#include <dev/i2o/iopspvar.h>

static void	iopsp_attach(struct device *, struct device *, void *);
static void	iopsp_intr(struct device *, struct iop_msg *, void *);
static int	iopsp_ioctl(struct scsipi_link *, u_long, caddr_t, int,
			    struct proc *);
static int	iopsp_match(struct device *, struct cfdata *, void *);
static void	iopsp_minphys(struct buf *);
static int	iopsp_rescan(struct iopsp_softc *);
static void	iopsp_scan(struct iopsp_softc *);
static int	iopsp_scsi_abort(struct iopsp_softc *, struct iop_msg *);
static int	iopsp_scsi_cmd(struct scsipi_xfer *);

static struct scsipi_device iopsp_dev = {
	NULL,			/* Use default error handler */
	NULL,			/* have a queue, served by this */
	NULL,			/* have no async handler */
	NULL,			/* Use default 'done' routine */
};

struct cfattach iopsp_ca = {
	sizeof(struct iopsp_softc), iopsp_match, iopsp_attach
};

/*
 * Match SCSI and fibre channel ports.
 */
static int
iopsp_match(struct device *parent, struct cfdata *match, void *aux)
{
	struct iop_attach_args *ia;
	struct {
		struct	i2o_param_op_results pr;
		struct	i2o_param_read_results prr;
		struct	i2o_param_hba_ctlr_info ci;
	} __attribute__ ((__packed__)) param;

	ia = aux;

	if (ia->ia_class != I2O_CLASS_BUS_ADAPTER_PORT)
		return (0);

	if (iop_params_get((struct iop_softc *)parent, ia->ia_tid,
	    I2O_PARAM_HBA_CTLR_INFO, &param, sizeof(param)) != 0)
		return (0);

	/* XXX */
	return (param.ci.bustype == I2O_HBA_BUS_SCSI ||
	    param.ci.bustype == I2O_HBA_BUS_FCA);
}

/*
 * Attach a supported device.
 */
static void
iopsp_attach(struct device *parent, struct device *self, void *aux)
{
	struct iop_attach_args *ia;
	struct iopsp_softc *sc;
	struct scsipi_link *sc_link;
	struct iop_softc *iop;
	struct {
		struct	i2o_param_op_results pr;
		struct	i2o_param_read_results prr;
		union {
			struct	i2o_param_device_identity di;
			struct	i2o_param_hba_ctlr_info ci;
			struct	i2o_param_hba_scsi_ctlr_info sci;
		} p;
	} __attribute__ ((__packed__)) param;
	char ident[64];
	int fcal;
#ifdef I2OVERBOSE
	int size;
#endif

	ia = (struct iop_attach_args *)aux;
	sc = (struct iopsp_softc *)self;
	iop = (struct iop_softc *)parent;
	sc->sc_tid = ia->ia_tid;

	/* Register us as an initiator. */
	sc->sc_ii.ii_dv = self;
	sc->sc_ii.ii_intr = iopsp_intr;
	sc->sc_ii.ii_flags = 0;
	if (iop_initiator_register(iop, &sc->sc_ii) != 0) {
		printf("%s: unable to register as an initiator",
		    sc->sc_dv.dv_xname);
		return;
	}

	if (iop_params_get(iop, ia->ia_tid, I2O_PARAM_HBA_CTLR_INFO, &param,
	    sizeof(param)) != 0) {
	    	printf("%s: unable to retrieve parameters\n",
	    	    sc->sc_dv.dv_xname);
		goto bad;
	}

	fcal = (param.p.ci.bustype == I2O_HBA_BUS_FCA);		/* XXX */

	/* 
	 * Say what the device is.  If we can find out what the controling
	 * device is, say what that is too.
	 */
	printf(": %s port", fcal ? "FC-AL" : "SCSI");
	if (iop_params_get(iop, ia->ia_tid, I2O_PARAM_DEVICE_IDENTITY, &param,
	    sizeof(param)) == 0) {
		iop_strvis(param.p.di.vendorinfo, 
		    sizeof(param.p.di.vendorinfo), ident, sizeof(ident));
		printf(" <%s, ", ident);
		iop_strvis(param.p.di.productinfo, 
		    sizeof(param.p.di.productinfo), ident, sizeof(ident));
		printf("%s, ", ident);
		iop_strvis(param.p.di.revlevel, 
		    sizeof(param.p.di.revlevel), ident, sizeof(ident));
		printf("%s> ", ident);
	}
	printf("\n");

	if (iop_params_get(iop, ia->ia_tid, I2O_PARAM_HBA_SCSI_CTLR_INFO,
	    &param, sizeof(param)) != 0) {
		printf("%s: unable to retrieve controller parameters\n",
		    sc->sc_dv.dv_xname);
		goto bad;
	}

#ifdef I2OVERBOSE
	printf("%s: %d-bit, max sync rate %dMHz, initiator ID %d\n",
	    sc->sc_dv.dv_xname, param.p.sci.maxdatawidth,
	    (u_int32_t)le64toh(param.p.sci.maxsyncrate) / 1000,
	    le32toh(param.p.sci.initiatorid));
#endif

	sc->sc_adapter.scsipi_cmd = iopsp_scsi_cmd;
	sc->sc_adapter.scsipi_minphys = iopsp_minphys;
	sc->sc_adapter.scsipi_ioctl = iopsp_ioctl;

	sc_link = &sc->sc_link;
	sc_link->type = BUS_SCSI;
	sc_link->device = &iopsp_dev;
	sc_link->adapter = &sc->sc_adapter;
	sc_link->adapter_softc = sc;
	sc_link->scsipi_scsi.channel = 0;
	sc_link->scsipi_scsi.adapter_target = le32toh(param.p.sci.initiatorid);
	sc_link->scsipi_scsi.max_target = 
	    fcal ? IOPSP_MAX_FCAL_TARGET : param.p.sci.maxdatawidth - 1;
	sc_link->scsipi_scsi.max_lun = IOPSP_MAX_LUN;
	sc_link->openings = iop->sc_maxqueuecnt;		/* XXX */

#ifdef I2OVERBOSE
	/*
	 * Allocate the target map.  Currently used for informational
	 * purposes only.
	 */
	size = (sc_link->scsipi_scsi.max_target + 1) *
	    sizeof(struct iopsp_target);
	sc->sc_targetmap = malloc(size, M_DEVBUF, M_NOWAIT);
	memset(sc->sc_targetmap, 0, size);
#endif

	/* Build the two maps, and attach to scsipi. */
	iopsp_scan(sc);
	config_found(self, sc_link, scsiprint);
	return;

bad:
	iop_initiator_unregister(iop, &sc->sc_ii);
}

/*
 * Determine which devices we control, and enter them into the maps.
 */
static void
iopsp_scan(struct iopsp_softc *sc)
{
	struct iop_softc *iop;
	struct i2o_lct_entry *le;
	struct scsipi_link *sc_link;
	struct {
		struct	i2o_param_op_results pr;
		struct	i2o_param_read_results prr;
		struct	i2o_param_scsi_device_info sdi;
	} __attribute__ ((__packed__)) param;
	int tid, nent, i, targ, lun, size;
	u_short *tidmap;
#ifdef I2OVERBOSE
	struct iopsp_target *it;
	int syncrate;	
#endif

	iop = (struct iop_softc *)sc->sc_dv.dv_parent;
	sc_link = &sc->sc_link;

	/*
	 * Allocate memory for the target/LUN -> TID map.  Use zero to
	 * denote absent targets (zero is the TID of the I2O executive,
	 * and we never address that here).
	 */
	size = (sc_link->scsipi_scsi.max_target + 1) *
	    (IOPSP_MAX_LUN + 1) * sizeof(u_short);
	tidmap = malloc(size, M_DEVBUF, M_NOWAIT);
	memset(tidmap, 0, size);	/* XXX */

#ifdef I2OVERBOSE
	for (i = 0; i <= sc_link->scsipi_scsi.max_target; i++)
		sc->sc_targetmap[i].it_flags &= ~IT_PRESENT;
#endif

	if (iop_lct_lock(iop) != 0)
		return;

	nent = iop->sc_nlctent;
	for (i = 0, le = iop->sc_lct->entry; i < nent; i++, le++) {
		switch (le16toh(le->classid) & 4095) {
		case I2O_CLASS_SCSI_PERIPHERAL:
		case I2O_CLASS_FIBRE_CHANNEL_PERIPHERAL:
			break;
		default:
			continue;
		}
		if (((le32toh(le->usertid) >> 12) & 4095) != sc->sc_tid)
			continue;

		tid = le32toh(le->localtid) & 4095;

		if (iop_params_get(iop, tid, I2O_PARAM_SCSI_DEVICE_INFO, &param,
		    sizeof(param)) != 0) {
			printf("%s: unable to retrieve device parameters\n",
			    sc->sc_dv.dv_xname);
			continue;
		}
		targ = le32toh(param.sdi.identifier);
		lun = param.sdi.luninfo[1];

		/* If the device is in use by another module, ignore it. */
		if (!iop_tid_inuse(iop, tid)) {
#ifdef I2OVERBOSE
			if (sc->sc_tidmap == NULL ||
			    IOPSP_TIDMAP(sc->sc_tidmap, targ, lun) !=
			    IOPSP_TID_INUSE)
				printf("%s: target %d,%d (tid %d): in use by"
				    " another module\n", sc->sc_dv.dv_xname,
				    targ, lun, tid);
#endif
			IOPSP_TIDMAP(tidmap, targ, lun) = IOPSP_TID_INUSE;
			continue;
		}
		IOPSP_TIDMAP(tidmap, targ, lun) = (u_short)tid;

#ifdef I2OVERBOSE
		/*
		 * If we've already described this target, and nothing has
		 * changed, then don't describe it again.
		 */
		it = &sc->sc_targetmap[targ];
		it->it_flags |= IT_PRESENT;
		syncrate = ((int)le64toh(param.sdi.negsyncrate) + 500) / 1000;
		if (it->it_width == param.sdi.negdatawidth &&
		    it->it_offset == param.sdi.negoffset &&
		    it->it_syncrate == syncrate)
			continue;

		it->it_width = param.sdi.negdatawidth;
		it->it_offset = param.sdi.negoffset;
		it->it_syncrate = syncrate;

		printf("%s: target %d (tid %d): %d-bit, ", sc->sc_dv.dv_xname,
		    targ, tid, it->it_width);
		if (it->it_syncrate == 0)
			printf("asynchronous\n");
		else
			printf("synchronous at %dMHz, offset 0x%x\n",
			    it->it_syncrate, it->it_offset);
#endif
	}

	iop_lct_unlock(iop);

#ifdef I2OVERBOSE
	for (i = 0; i <= sc_link->scsipi_scsi.max_target; i++)
		if ((sc->sc_targetmap[i].it_flags & IT_PRESENT) == 0)
			sc->sc_targetmap[i].it_width = 0;
#endif

	/* Swap in the new map and return. */
	if (sc->sc_tidmap != NULL)
		free(sc->sc_tidmap, M_DEVBUF);
	sc->sc_tidmap = tidmap;
	sc->sc_chgindicator = iop->sc_lct->changeindicator;
}

/*
 * Adjust the size an I/O request.
 */
static void
iopsp_minphys(struct buf *bp)
{

	if (bp->b_bcount > IOP_MAX_XFER)
		bp->b_bcount = IOP_MAX_XFER;
	minphys(bp);
}

/*
 * Ask the bus port to perform a rescan.  XXX It's unclear whether or not
 * TIDs for established targets will change.
 */
static int
iopsp_rescan(struct iopsp_softc *sc)
{
	struct iop_softc *iop;
	struct iop_msg *im;
	struct i2o_hba_bus_scan *mb;
	int rv;

	iop = (struct iop_softc *)sc->sc_dv.dv_parent;

	if (iop_msg_alloc(iop, &sc->sc_ii, &im, IM_NOINTR) != 0)
		return (-1);

	mb = (struct i2o_hba_bus_scan *)im->im_msg;
	mb->msgflags = I2O_MSGFLAGS(i2o_hba_bus_scan);
	mb->msgfunc = I2O_MSGFUNC(sc->sc_tid, I2O_HBA_BUS_SCAN);
	mb->msgictx = sc->sc_ii.ii_ictx;
	mb->msgtctx = im->im_tctx;

	if (iop_msg_enqueue(iop, im) != 0)
		rv = -1;
	else if(iop_msg_wait(iop, im, 1000) != 0)
		rv = -1;
	else
		rv = 0;
	iop_msg_free(iop, &sc->sc_ii, im);
	if (rv != 0)
		return (rv);

	/*
         * Re-read the LCT and determine if it has changed.  XXX This should
         * probably be accomplished by noticing a CHANGED_LCT event from the
         * executive.
	 */
	if (iop_lct_get(iop))
		return (-1);
	if (iop->sc_lct->changeindicator == sc->sc_chgindicator)
		return (0);

	/* Rebuild the target/LUN -> TID map, and return. */
	iopsp_scan(sc);
	return (0);
}

/*
 * Start a SCSI command.
 */
static int
iopsp_scsi_cmd(struct scsipi_xfer *xs)
{
	struct scsipi_link *sc_link;
	struct iopsp_softc *sc;
	struct iop_msg *im;
	struct iop_softc *iop;
	struct i2o_scsi_scb_exec *mb;
	int error, flags, tid;

	sc_link = xs->sc_link;
	flags = xs->xs_control;
	sc = sc_link->adapter_softc;
	iop = (struct iop_softc *)sc->sc_dv.dv_parent;

	tid = IOPSP_TIDMAP(sc->sc_tidmap, sc_link->scsipi_scsi.target,
	    sc_link->scsipi_scsi.lun);
	if (tid == IOPSP_TID_ABSENT || tid == IOPSP_TID_INUSE) {
		xs->error = XS_SELTIMEOUT;
		return (COMPLETE);
	}

	SC_DEBUG(sc_link, SDEV_DB2, ("iopsp_scsi_cmd\n"));

	/* Need to reset the target? */
	if ((flags & XS_CTL_RESET) != 0) {
		if (iop_simple_cmd(iop, tid, I2O_SCSI_DEVICE_RESET,
		    sc->sc_ii.ii_ictx) != 0) {
#ifdef I2ODEBUG
			printf("%s: reset failed\n", sc->sc_dv.dv_xname);
#endif
			xs->error = XS_DRIVER_STUFFUP;
		}
		return (COMPLETE);
	}

#if defined(I2ODEBUG) || defined(SCSIDEBUG)
	if (xs->cmdlen > 16) {
		printf("%s: CDB too large\n", sc->sc_dv.dv_xname);
		xs->error = XS_DRIVER_STUFFUP;
		return (COMPLETE);
	}
#endif

	if (iop_msg_alloc(iop, &sc->sc_ii, &im,
	    (flags & (XS_CTL_POLL | XS_CTL_NOSLEEP)) != 0 ? IM_NOWAIT : 0)) {
		xs->error = XS_DRIVER_STUFFUP;
		return (TRY_AGAIN_LATER);
	}
	im->im_dvcontext = xs;

	mb = (struct i2o_scsi_scb_exec *)im->im_msg;
	mb->msgflags = I2O_MSGFLAGS(i2o_scsi_scb_exec);
	mb->msgfunc = I2O_MSGFUNC(tid, I2O_SCSI_SCB_EXEC);
	mb->msgictx = sc->sc_ii.ii_ictx;
	mb->msgtctx = im->im_tctx;
	mb->flags = xs->cmdlen | I2O_SCB_FLAG_ENABLE_DISCONNECT |
	    I2O_SCB_FLAG_SENSE_DATA_IN_MESSAGE;
	memset(mb->cdb, 0, sizeof(mb->cdb));
	memcpy(mb->cdb, xs->cmd, xs->cmdlen);
	mb->datalen = xs->datalen;

	/* XXX */
	if ((xs->sc_link->quirks & SDEV_NOTAG) == 0 &&
	    xs->cmd->opcode != INQUIRY &&
	    xs->cmd->opcode != TEST_UNIT_READY &&
	    xs->cmd->opcode != REQUEST_SENSE) {
		if (xs->bp != NULL && (xs->bp->b_flags & B_ASYNC) != 0)
			mb->flags |= I2O_SCB_FLAG_ORDERED_QUEUE_TAG;
		else
			mb->flags |= I2O_SCB_FLAG_SIMPLE_QUEUE_TAG;
	}

	if (xs->datalen != 0) {
		error = iop_msg_map(iop, im, xs->data, xs->datalen, 
		    (flags & XS_CTL_DATA_OUT) == 0);
		if (error) {
#ifdef I2ODEBUG
			printf("%s: error %d mapping xfer\n",
			    sc->sc_dv.dv_xname, error);
#endif
			xs->error = XS_DRIVER_STUFFUP;
			iop_msg_free(iop, &sc->sc_ii, im);
			return (COMPLETE);
		}
		if ((flags & XS_CTL_DATA_IN) == 0)
			mb->flags |= I2O_SCB_FLAG_XFER_TO_DEVICE;
		else
			mb->flags |= I2O_SCB_FLAG_XFER_FROM_DEVICE;
	}

	/* 
	 * If the command is allowed to execute asynchronously, enqueue it
	 * with the IOP.  XXX Time out async commands?
	 */ 
	if ((flags & XS_CTL_POLL) == 0) {
		if (iop_msg_enqueue(iop, im)) {
			iop_msg_unmap(iop, im);
			iop_msg_free(iop, &sc->sc_ii, im);
#ifdef I2ODEBUG
			printf("%s: can't enqueue msg\n", sc->sc_dv.dv_xname);
#endif
			xs->error = XS_DRIVER_STUFFUP;
			return (COMPLETE);
		}
		return (SUCCESSFULLY_QUEUED);
	}

	if (iop_msg_send(iop, im, xs->timeout)) {
		scsi_print_addr(xs->sc_link);
		printf("timeout; aborting command\n");
		if (iopsp_scsi_abort(sc, im)) {
			scsi_print_addr(xs->sc_link);
			printf("abort failed\n");
		}
		xs->error = XS_DRIVER_STUFFUP;
	}
	return (COMPLETE);
}

/*
 * Abort the specified I2O_SCSI_SCB_EXEC message and its associated SCB.
 */
static int
iopsp_scsi_abort(struct iopsp_softc *sc, struct iop_msg *aim)
{
	struct iop_msg *im;
	struct i2o_scsi_scb_abort *mb;
	struct iop_softc *iop;
	int rv;

	iop = (struct iop_softc *)sc->sc_dv.dv_parent;

	rv = iop_msg_alloc(iop, &sc->sc_ii, &im, IM_NOWAIT | IM_NOINTR);
	if (rv != 0)
		return (rv);

	mb = (struct i2o_scsi_scb_abort *)im->im_msg;
	mb->msgflags = I2O_MSGFLAGS(i2o_scsi_scb_abort);
	mb->msgfunc = I2O_MSGFUNC(aim->im_tid, I2O_SCSI_SCB_ABORT);
	mb->msgictx = sc->sc_ii.ii_ictx;
	mb->msgtctx = im->im_tctx;
	mb->tctxabort = aim->im_tctx;

	rv = iop_msg_send(iop, im, 1000);
	iop_msg_free(iop, &sc->sc_ii, im);
	return (rv);
}

/*
 * We have a message which has been processed and replied to by the IOP -
 * deal with it.
 */
static void
iopsp_intr(struct device *dv, struct iop_msg *im, void *reply)
{
	struct scsipi_xfer *xs;
	struct iopsp_softc *sc;
	struct i2o_scsi_reply *rb;
	struct iop_softc *iop;
	u_int hba_status, scsi_status, detail;
	int sl;

	sc = (struct iopsp_softc *)dv;
	xs = (struct scsipi_xfer *)im->im_dvcontext;
	iop = (struct iop_softc *)dv->dv_parent;

	SC_DEBUG(xs->sc_link, SDEV_DB2, ("iopsp_intr\n"));

	if (xs->error == XS_NOERROR) {
		rb = reply;
		detail = le16toh(rb->detail);
		hba_status = (detail >> 8) & 0xff;
		scsi_status = detail & 0xff;

		if (hba_status != I2O_SCSI_DSC_SUCCESS) {
			switch (hba_status) {
			case I2O_SCSI_DSC_ADAPTER_BUSY:
			case I2O_SCSI_DSC_SCSI_BUS_RESET:
			case I2O_SCSI_DSC_BUS_BUSY:
				xs->error = XS_BUSY;
				break;
			case I2O_SCSI_DSC_SELECTION_TIMEOUT:
				xs->error = XS_SELTIMEOUT;
				break;
			case I2O_SCSI_DSC_COMMAND_TIMEOUT:
			case I2O_SCSI_DSC_DEVICE_NOT_PRESENT:
			case I2O_SCSI_DSC_LUN_INVALID:
			case I2O_SCSI_DSC_SCSI_TID_INVALID:
				xs->error = XS_TIMEOUT;
				break;
			default:
				xs->error = XS_DRIVER_STUFFUP;
				break;
			}
#ifdef I2ODEBUG
			printf("%s: HBA status 0x%02x\n", sc->sc_dv.dv_xname,
			    hba_status);
#endif
		} else if (scsi_status != SCSI_OK) {
			switch (scsi_status) {
			case SCSI_CHECK:
				xs->error = XS_SENSE;
				sl = le32toh(rb->senselen);
				if (xs->req_sense_length != 0 &&
				    xs->req_sense_length < sl)
					sl = xs->req_sense_length;
				if (sl > sizeof(xs->sense.scsi_sense))
					sl = le32toh(rb->senselen);
				memcpy(&xs->sense.scsi_sense, rb->sense, sl);
				break;
			case SCSI_BUSY:
				xs->error = XS_BUSY;
				break;
			default:
				xs->error = XS_DRIVER_STUFFUP;
				break;
			}
		} else
			xs->error = XS_NOERROR;

		xs->resid = le32toh(rb->datalen) - xs->datalen;
		xs->status = scsi_status;
	}

	/* Free the message wrapper and pass the news to scsipi. */
	iop_msg_unmap(iop, im);
	iop_msg_free(iop, &sc->sc_ii, im);
	xs->xs_status |= XS_STS_DONE;
	scsipi_done(xs);
}

/*
 * ioctl hook; used here only to initiate low-level rescans.
 */
static int
iopsp_ioctl(struct scsipi_link *sc_link, u_long cmd, caddr_t data, int flag,
	    struct proc *p)
{
	int rv;

	switch (cmd) {
	case SCBUSIOLLSCAN:
		rv = iopsp_rescan(sc_link->adapter_softc);
		break;
	default:
		rv = ENXIO;
		break;
	}
	
	return (rv);
}
