/*	$NetBSD: ncr.c,v 1.31 1996/11/24 13:32:50 matthias Exp $	*/

/*
 * Copyright (c) 1996 Matthias Pfaller.
 * All rights reserved.
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
 *	This product includes software developed by Matthias Pfaller.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/buf.h>
#include <scsi/scsi_all.h>
#include <scsi/scsi_message.h>
#include <scsi/scsiconf.h>

#include <dev/ic/ncr5380reg.h>
#include <dev/ic/ncr5380var.h>
#include <machine/cpufunc.h>

/*
 * Function declarations:
 */
static int ncr_pdma_in __P((struct ncr5380_softc *, int, int, u_char *));
static int ncr_pdma_out __P((struct ncr5380_softc *, int, int, u_char *));
static void ncr_minphys __P((struct buf *bp));
static void ncr_intr __P((void *));
static void ncr_attach __P((struct device *, struct device *, void *));
static int ncr_match __P((struct device *, void *, void *));

/*
 * Some constants.
 */
#define PDMA_ADDRESS	((volatile u_char *) 0xffe00000)
#define	NCR5380		((volatile u_char *) 0xffd00000)

/*
 * Bit allocation in config's sc_flags field.
 *
 * bit     0: disable disconnect/reconnect
 * bit     1: disable use of interrupts
 * bits 8-15: disable parity (per target)
 */
#define NCR_DISABLE_RESELECT	1
#define NCR_DISABLE_INTERRUPTS	2

/*
 * Make the default options patchable with gdb.
 */
int ncr_default_options = 0;

struct scsi_adapter ncr_switch = {
	ncr5380_scsi_cmd,	/* scsi_cmd()				*/
	minphys,		/* scsi_minphys()			*/
	0,			/* open_target_lu()			*/
	0			/* close_target_lu()			*/
};

struct scsi_device ncr_dev = {
	NULL,			/* use default error handler		*/
	NULL,			/* do not have a start functio		*/
	NULL,			/* have no async handler		*/
	NULL			/* Use default done routine		*/
};

struct cfattach ncr_ca = {
	sizeof(struct ncr5380_softc), ncr_match, ncr_attach
};

struct cfdriver ncr_cd = {
	NULL, "ncr", DV_DULL, NULL, 0,
};

static int
ncr_match(parent, cf, aux)
	struct device	*parent;
	void		*cf, *aux;
{
	int unit = ((struct cfdata *)cf)->cf_unit;
	
	if (unit != 0)	/* Only one unit */
		return(0);
	return(1);
}

static void
ncr_attach(parent, self, aux)
	struct device	*parent, *self;
	void		*aux;
{
	struct ncr5380_softc *sc = (struct ncr5380_softc *) self;
	int flags;

	/*
	 * For now we only support the DP8490.
	 */
	scsi_select_ctlr(DP8490);

	/* Pull in config flags. */ 
	flags = sc->sc_dev.dv_cfdata->cf_flags | ncr_default_options;

	/*
	 * Fill in the prototype scsi_link.
	 */
	sc->sc_link.channel        = SCSI_CHANNEL_ONLY_ONE;
	sc->sc_link.adapter_softc  = sc;
	sc->sc_link.adapter_target = 7;
	sc->sc_link.adapter        = &ncr_switch;
	sc->sc_link.device	   = &ncr_dev;

	/*
	 * Initialize NCR5380 register addresses.
	 */
	sc->sci_r0 = NCR5380 + 0;
	sc->sci_r1 = NCR5380 + 1;
	sc->sci_r2 = NCR5380 + 2;
	sc->sci_r3 = NCR5380 + 3;
	sc->sci_r4 = NCR5380 + 4;
	sc->sci_r5 = NCR5380 + 5;
	sc->sci_r6 = NCR5380 + 6;
	sc->sci_r7 = NCR5380 + 7;

	/*
	 * We only have to set the sc_pio_in and sc_pio_out
	 * function pointers. The rest of the MD functions is
	 * not used and defaults to NULL.
	 */
	sc->sc_pio_in	= ncr_pdma_in;
	sc->sc_pio_out	= ncr_pdma_out;

	/*
	 * Copy options from cf_flags to sc_flags and sc_parity_disable.
	 */
	sc->sc_flags = ((flags & NCR_DISABLE_RESELECT) ?
				0 : NCR5380_PERMIT_RESELECT) |
		       ((flags & NCR_DISABLE_INTERRUPTS) ?
				NCR5380_FORCE_POLLING : 0);
	sc->sc_parity_disable = flags >> 8;

	intr_establish(IR_SCSI1, ncr_intr, (void *)sc, sc->sc_dev.dv_xname,
		IPL_BIO, RISING_EDGE);
	printf(" addr 0x%x, irq %d\n", NCR5380, IR_SCSI1);

	/*
	 *  Initialize the SCSI controller itself.
	 */
	ncr5380_init(sc);
	ncr5380_reset_scsibus(sc);
	config_found(self, &(sc->sc_link), scsiprint);
}

static void
ncr_intr(arg)
	void *arg;
{
	register struct ncr5380_softc *sc = arg;
	int s;

	if (*sc->sci_csr & SCI_CSR_INT) {
		s = splbio();
		if (ncr5380_intr(sc) == 0) {
			printf("%s: ", sc->sc_dev.dv_xname);
			if ((*sc->sci_bus_csr & ~SCI_BUS_RST) == 0)
				printf("BUS RESET\n");
			else
				printf("spurious interrupt\n");
			SCI_CLR_INTR(sc);
		}
		splx(s);
	}
}

/*
 * PDMA stuff
 */

#define byte_data ((volatile u_char *)pdma)
#define word_data ((volatile u_short *)pdma)
#define long_data ((volatile u_long *)pdma)

#define W1(n)	*byte_data = *(data + n)
#define W2(n)	*word_data = *((u_short *)data + n)
#define W4(n)	*long_data = *((u_long *)data + n)
#define R1(n)	*(data + n) = *byte_data
#define R4(n)	*((u_long *)data + n) = *long_data

#define TSIZE	512

#define TIMEOUT	1000000

static __inline int
ncr_ready(sc)
	struct ncr5380_softc *sc;
{
	register int i;

	for (i = TIMEOUT; i > 0; i--) {
		if ((*sc->sci_csr & (SCI_CSR_DREQ | SCI_CSR_PHASE_MATCH)) ==
		    (SCI_CSR_DREQ | SCI_CSR_PHASE_MATCH))
		    	return(1);

		if ((*sc->sci_csr & SCI_CSR_PHASE_MATCH) == 0 ||
		    SCI_BUSY(sc) == 0)
			return(0);
	}
	printf("%s: ready timeout\n", sc->sc_dev.dv_xname);
	return(0);
}

static int
ncr_pdma_in(sc, phase, datalen, data)
	struct ncr5380_softc *sc;
	int phase, datalen;
	u_char *data;
{
	register volatile u_char *pdma = PDMA_ADDRESS;
	register int resid, s, ready = 1;

	if (datalen < TSIZE)
		return(ncr5380_pio_in(sc, phase, datalen, data));

	s = splbio();
	*sc->sci_mode |= SCI_MODE_DMA;
	*sc->sci_irecv = 0;

	resid = datalen;
	while (resid >= TSIZE) {
		if (ncr_ready(sc) == 0) {
			ready = 0;
			break;
		}
		di();
		movsd((u_char *)pdma, data, TSIZE / 4);
		resid -= TSIZE;
		ei();
	}

	if (resid && ready) {
		di();
		while (resid > 0) {
			if (ncr_ready(sc) == 0)
				break;
			R1(0);
			data++;
			resid--;
		}
		ei();
	}

	SCI_CLR_INTR(sc);
	*sc->sci_mode &= ~SCI_MODE_DMA;
	splx(s);
	return(datalen - resid);
}

static int
ncr_pdma_out(sc, phase, datalen, data)
	struct ncr5380_softc *sc;
	int phase, datalen;
	u_char *data;
{
	register volatile u_char *pdma = PDMA_ADDRESS;
	register int i, s, resid, ready = 1;
	register u_char icmd;

	if (datalen < TSIZE)
		return(ncr5380_pio_out(sc, phase, datalen, data));

	s = splbio();
	icmd = *(sc->sci_icmd) & SCI_ICMD_RMASK;
	*sc->sci_icmd = icmd | SCI_ICMD_DATA;
	*sc->sci_mode |= SCI_MODE_DMA;
	*sc->sci_dma_send = 0;

	resid = datalen;
	while (resid >= TSIZE) {
		if (ncr_ready(sc) == 0) {
			ready = 0;
			break;
		}
		di();
		W1(0);

		/*
		 * The second ready is to compensate for DMA-prefetch.
		 * Since we adjust resid only at the end of the block,
		 * there is no need to correct the residue.
		 */
		if (ncr_ready(sc) == 0) {
			ready = 0;
			break;
		}
		W1(1); W2(1);
		data += 4;
		movsd(data, (u_char *)pdma, TSIZE / 4 - 1);
		ei();
		resid -= TSIZE;
	}

	if (resid && ready) {
		if (ncr_ready(sc) == 1) {
			di();
			while (resid > 0) {
				W1(0);
				if (ncr_ready(sc) == 0)
					break;
				data++;
				resid--;
			}
			ei();
		}
	}

	for (i = TIMEOUT; i > 0; i--) {
		if ((*sc->sci_csr & (SCI_CSR_DREQ|SCI_CSR_PHASE_MATCH))
		    != SCI_CSR_DREQ)
			break;
	}
	if (i != 0)
		*byte_data = 0;
	else
		printf("%s: timeout waiting for final SCI_DSR_DREQ.\n",
			sc->sc_dev.dv_xname);

	SCI_CLR_INTR(sc);
	*sc->sci_mode &= ~SCI_MODE_DMA;
	*sc->sci_icmd = icmd;
	splx(s);
	return(datalen - resid);
}
