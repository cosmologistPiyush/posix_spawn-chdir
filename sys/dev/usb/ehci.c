/*	$NetBSD: ehci.c,v 1.9 2001/11/18 00:39:46 augustss Exp $	*/

/*
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net).
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
 * USB Enhanced Host Controller Driver, a.k.a. USB 2.0 controller.
 *
 * The EHCI 0.96 spec can be found at
 * http://developer.intel.com/technology/usb/download/ehci-r096.pdf
 * and the USB 2.0 spec at
 * http://www.usb.org/developers/data/usb_20.zip
 *
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ehci.c,v 1.9 2001/11/18 00:39:46 augustss Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/select.h>
#include <sys/proc.h>
#include <sys/queue.h>

#include <machine/bus.h>
#include <machine/endian.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdivar.h>
#include <dev/usb/usb_mem.h>
#include <dev/usb/usb_quirks.h>

#include <dev/usb/ehcireg.h>
#include <dev/usb/ehcivar.h>

#ifdef EHCI_DEBUG
#define DPRINTF(x)	if (ehcidebug) printf x
#define DPRINTFN(n,x)	if (ehcidebug>(n)) printf x
int ehcidebug = 0;
#define bitmask_snprintf(q,f,b,l) snprintf((b), (l), "%b", (q), (f))
#else
#define DPRINTF(x)
#define DPRINTFN(n,x)
#endif

struct ehci_pipe {
	struct usbd_pipe pipe;
};

Static void		ehci_shutdown(void *);
Static void		ehci_power(int, void *);

Static usbd_status	ehci_open(usbd_pipe_handle);
Static void		ehci_poll(struct usbd_bus *);
Static void		ehci_softintr(void *);

Static usbd_status	ehci_allocm(struct usbd_bus *, usb_dma_t *, u_int32_t);
Static void		ehci_freem(struct usbd_bus *, usb_dma_t *);

Static usbd_xfer_handle	ehci_allocx(struct usbd_bus *);
Static void		ehci_freex(struct usbd_bus *, usbd_xfer_handle);

Static usbd_status	ehci_root_ctrl_transfer(usbd_xfer_handle);
Static usbd_status	ehci_root_ctrl_start(usbd_xfer_handle);
Static void		ehci_root_ctrl_abort(usbd_xfer_handle);
Static void		ehci_root_ctrl_close(usbd_pipe_handle);
Static void		ehci_root_ctrl_done(usbd_xfer_handle);

Static usbd_status	ehci_root_intr_transfer(usbd_xfer_handle);
Static usbd_status	ehci_root_intr_start(usbd_xfer_handle);
Static void		ehci_root_intr_abort(usbd_xfer_handle);
Static void		ehci_root_intr_close(usbd_pipe_handle);
Static void		ehci_root_intr_done(usbd_xfer_handle);

Static usbd_status	ehci_device_ctrl_transfer(usbd_xfer_handle);
Static usbd_status	ehci_device_ctrl_start(usbd_xfer_handle);
Static void		ehci_device_ctrl_abort(usbd_xfer_handle);
Static void		ehci_device_ctrl_close(usbd_pipe_handle);
Static void		ehci_device_ctrl_done(usbd_xfer_handle);

Static usbd_status	ehci_device_bulk_transfer(usbd_xfer_handle);
Static usbd_status	ehci_device_bulk_start(usbd_xfer_handle);
Static void		ehci_device_bulk_abort(usbd_xfer_handle);
Static void		ehci_device_bulk_close(usbd_pipe_handle);
Static void		ehci_device_bulk_done(usbd_xfer_handle);

Static usbd_status	ehci_device_intr_transfer(usbd_xfer_handle);
Static usbd_status	ehci_device_intr_start(usbd_xfer_handle);
Static void		ehci_device_intr_abort(usbd_xfer_handle);
Static void		ehci_device_intr_close(usbd_pipe_handle);
Static void		ehci_device_intr_done(usbd_xfer_handle);

Static usbd_status	ehci_device_isoc_transfer(usbd_xfer_handle);
Static usbd_status	ehci_device_isoc_start(usbd_xfer_handle);
Static void		ehci_device_isoc_abort(usbd_xfer_handle);
Static void		ehci_device_isoc_close(usbd_pipe_handle);
Static void		ehci_device_isoc_done(usbd_xfer_handle);

Static void		ehci_device_clear_toggle(usbd_pipe_handle pipe);
Static void		ehci_noop(usbd_pipe_handle pipe);

Static int		ehci_str(usb_string_descriptor_t *, int, char *);
Static void		ehci_pcd(ehci_softc_t *, usbd_xfer_handle);
Static void		ehci_pcd_able(ehci_softc_t *, int);
Static void		ehci_pcd_enable(void *);
Static void		ehci_disown(ehci_softc_t *, int, int);

Static ehci_soft_qh_t  *ehci_alloc_sqh(ehci_softc_t *);
Static void		ehci_free_sqh(ehci_softc_t *, ehci_soft_qh_t *);

Static ehci_soft_qtd_t  *ehci_alloc_sqtd(ehci_softc_t *);
Static void		ehci_free_sqtd(ehci_softc_t *, ehci_soft_qtd_t *);

Static void		ehci_hash_add_qtd(ehci_softc_t *, ehci_soft_qtd_t *);
Static void		ehci_hash_rem_qtd(ehci_softc_t *, ehci_soft_qtd_t *);
Static ehci_soft_qtd_t  *ehci_hash_find_qtd(ehci_softc_t *, ehci_physaddr_t);

#ifdef EHCI_DEBUG
Static void		ehci_dumpregs(ehci_softc_t *);
Static void		ehci_dump(void);
Static ehci_softc_t 	*theehci;
Static void		ehci_dump_link(ehci_link_t);
Static void		ehci_dump_sqtd(ehci_soft_qtd_t *);
Static void		ehci_dump_qtd(ehci_qtd_t *);
Static void		ehci_dump_sqh(ehci_soft_qh_t *);
#endif

#define EHCI_INTR_ENDPT 1

Static struct usbd_bus_methods ehci_bus_methods = {
	ehci_open,
	ehci_softintr,
	ehci_poll,
	ehci_allocm,
	ehci_freem,
	ehci_allocx,
	ehci_freex,
};

Static struct usbd_pipe_methods ehci_root_ctrl_methods = {	
	ehci_root_ctrl_transfer,
	ehci_root_ctrl_start,
	ehci_root_ctrl_abort,
	ehci_root_ctrl_close,
	ehci_noop,
	ehci_root_ctrl_done,
};

Static struct usbd_pipe_methods ehci_root_intr_methods = {	
	ehci_root_intr_transfer,
	ehci_root_intr_start,
	ehci_root_intr_abort,
	ehci_root_intr_close,
	ehci_noop,
	ehci_root_intr_done,
};

Static struct usbd_pipe_methods ehci_device_ctrl_methods = {	
	ehci_device_ctrl_transfer,
	ehci_device_ctrl_start,
	ehci_device_ctrl_abort,
	ehci_device_ctrl_close,
	ehci_noop,
	ehci_device_ctrl_done,
};

Static struct usbd_pipe_methods ehci_device_intr_methods = {	
	ehci_device_intr_transfer,
	ehci_device_intr_start,
	ehci_device_intr_abort,
	ehci_device_intr_close,
	ehci_device_clear_toggle,
	ehci_device_intr_done,
};

Static struct usbd_pipe_methods ehci_device_bulk_methods = {	
	ehci_device_bulk_transfer,
	ehci_device_bulk_start,
	ehci_device_bulk_abort,
	ehci_device_bulk_close,
	ehci_device_clear_toggle,
	ehci_device_bulk_done,
};

Static struct usbd_pipe_methods ehci_device_isoc_methods = {
	ehci_device_isoc_transfer,
	ehci_device_isoc_start,
	ehci_device_isoc_abort,
	ehci_device_isoc_close,
	ehci_noop,
	ehci_device_isoc_done,
};

usbd_status
ehci_init(ehci_softc_t *sc)
{
	u_int32_t version, sparams, cparams, hcr;
	u_int i;
	usbd_status err;

	DPRINTF(("ehci_init: start\n"));
#ifdef EHCI_DEBUG
	theehci = sc;
#endif

	sc->sc_offs = EREAD1(sc, EHCI_CAPLENGTH);

	version = EREAD2(sc, EHCI_HCIVERSION);
	printf("%s: EHCI version %x.%x\n", USBDEVNAME(sc->sc_bus.bdev),
	       version >> 8, version & 0xff);

	sparams = EREAD4(sc, EHCI_HCSPARAMS);
	DPRINTF(("ehci_init: sparams=0x%x\n", sparams));
	sc->sc_npcomp = EHCI_HCS_N_PCC(sparams);
	if (EHCI_HCS_N_CC(sparams) != sc->sc_ncomp) {
		printf("%s: wrong number of companions (%d != %d)\n",
		       USBDEVNAME(sc->sc_bus.bdev),
		       EHCI_HCS_N_CC(sparams), sc->sc_ncomp);
		return (USBD_IOERROR);
	}
	if (sc->sc_ncomp > 0) {
		printf("%s: companion controller%s, %d port%s each:",
		    USBDEVNAME(sc->sc_bus.bdev), sc->sc_ncomp!=1 ? "s" : "",
		    EHCI_HCS_N_PCC(sparams),
		    EHCI_HCS_N_PCC(sparams)!=1 ? "s" : "");
		for (i = 0; i < sc->sc_ncomp; i++)
			printf(" %s", USBDEVNAME(sc->sc_comps[i]->bdev));
		printf("\n");
	}
	sc->sc_noport = EHCI_HCS_N_PORTS(sparams);
	cparams = EREAD4(sc, EHCI_HCCPARAMS);
	DPRINTF(("ehci_init: cparams=0x%x\n", cparams));
	
	sc->sc_bus.usbrev = USBREV_2_0;

	for (i = 0; i < EHCI_HASH_SIZE; i++)
		LIST_INIT(&sc->sc_hash_qtds[i]);

	/* Reset the controller */
	DPRINTF(("%s: resetting\n", USBDEVNAME(sc->sc_bus.bdev)));
	EOWRITE4(sc, EHCI_USBCMD, 0);	/* Halt controller */
	usb_delay_ms(&sc->sc_bus, 1);
	EOWRITE4(sc, EHCI_USBCMD, EHCI_CMD_HCRESET);
	for (i = 0; i < 100; i++) {
		delay(10);
		hcr = EOREAD4(sc, EHCI_USBCMD) & EHCI_CMD_HCRESET;
		if (!hcr)
			break;
	}
	if (hcr) {
		printf("%s: reset timeout\n", USBDEVNAME(sc->sc_bus.bdev));
		return (USBD_IOERROR);
	}

	/* frame list size at default, read back what we got and use that */
	switch (EHCI_CMD_FLS(EOREAD4(sc, EHCI_USBCMD))) {
	case 0: sc->sc_flsize = 1024*4; break;
	case 1: sc->sc_flsize = 512*4; break;
	case 2: sc->sc_flsize = 256*4; break;
	case 3: return (USBD_IOERROR);
	}
	err = usb_allocmem(&sc->sc_bus, sc->sc_flsize,
			   EHCI_FLALIGN_ALIGN, &sc->sc_fldma);
	if (err)
		return (err);
	DPRINTF(("%s: flsize=%d\n", USBDEVNAME(sc->sc_bus.bdev),sc->sc_flsize));

	/* Set up the bus struct. */
	sc->sc_bus.methods = &ehci_bus_methods;
	sc->sc_bus.pipe_size = sizeof(struct ehci_pipe);

	sc->sc_powerhook = powerhook_establish(ehci_power, sc);
	sc->sc_shutdownhook = shutdownhook_establish(ehci_shutdown, sc);

	sc->sc_eintrs = EHCI_NORMAL_INTRS;

	/* Allocate dummy QH that starts the bulk list. */
	sc->sc_bulk_head = ehci_alloc_sqh(sc);
	if (sc->sc_bulk_head == NULL) {
		err = USBD_NOMEM;
		goto bad1;
	}
	memset(&sc->sc_bulk_head->qh, 0, sizeof(ehci_qtd_t));
	sc->sc_bulk_head->qh.qh_qtd.qtd_status =
	    htole32(EHCI_QTD_SET_STATUS(EHCI_QTD_HALTED));
	sc->sc_bulk_head->qh.qh_link =
	    htole32(EHCI_LINK_TERMINATE); /* XXX no bw reclaimation */
	sc->sc_bulk_head->next = NULL;
#ifdef EHCI_DEBUG
	if (ehcidebug) {
		ehci_dump_sqh(sc->sc_bulk_head);
	}
#endif

	/* Allocate dummy QH that starts the control list. */
	sc->sc_ctrl_head = ehci_alloc_sqh(sc);
	if (sc->sc_ctrl_head == NULL) {
		err = USBD_NOMEM;
		goto bad2;
	}
	memset(&sc->sc_ctrl_head->qh, 0, sizeof(ehci_qtd_t));
	sc->sc_ctrl_head->qh.qh_qtd.qtd_status =
	    htole32(EHCI_QTD_SET_STATUS(EHCI_QTD_HALTED));
	sc->sc_ctrl_head->qh.qh_endp = htole32(EHCI_QH_HRECL);
	sc->sc_ctrl_head->qh.qh_link =
	    htole32(sc->sc_bulk_head->physaddr | EHCI_LINK_QH);
	sc->sc_ctrl_head = sc->sc_bulk_head;
#ifdef EHCI_DEBUG
	if (ehcidebug) {
		ehci_dump_sqh(sc->sc_ctrl_head);
	}
#endif

	/* Point to async list */
	EOWRITE4(sc, EHCI_ASYNCLISTADDR,
		 sc->sc_ctrl_head->physaddr | EHCI_LINK_QH);

	usb_callout_init(sc->sc_tmo_pcd);

	/* Enable interrupts */
	EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);

	/* Turn on controller */
	EOWRITE4(sc, EHCI_USBCMD,
		 EHCI_CMD_ITC_8 | /* 8 microframes */
		 (EOREAD4(sc, EHCI_USBCMD) & EHCI_CMD_FLS_M) |
		 /* EHCI_CMD_ASE | */
		 /* EHCI_CMD_PSE | */
		 EHCI_CMD_RS);

	/* Take over port ownership */
	EOWRITE4(sc, EHCI_CONFIGFLAG, EHCI_CONF_CF);

	for (i = 0; i < 100; i++) {
		delay(10);
		hcr = EOREAD4(sc, EHCI_USBSTS) & EHCI_STS_HCH;
		if (!hcr)
			break;
	}
	if (hcr) {
		printf("%s: run timeout\n", USBDEVNAME(sc->sc_bus.bdev));
		return (USBD_IOERROR);
	}

	return (USBD_NORMAL_COMPLETION);

#if 0
 bad3:
	ehci_free_sqh(sc, sc->sc_bulk_head);
#endif
 bad2:
	ehci_free_sqh(sc, sc->sc_ctrl_head);
 bad1:
	usb_freemem(&sc->sc_bus, &sc->sc_fldma);
	return (err);
}

Static int ehci_intr1(ehci_softc_t *);

int
ehci_intr(void *v)
{
	ehci_softc_t *sc = v;

	/* If we get an interrupt while polling, then just ignore it. */
	if (sc->sc_bus.use_polling) {
#ifdef DIAGNOSTIC
		printf("ehci_intr: ignored interrupt while polling\n");
#endif
		return (0);
	}

	return (ehci_intr1(sc)); 
}

Static int
ehci_intr1(ehci_softc_t *sc)
{
	u_int32_t intrs, eintrs;

	DPRINTFN(20,("ehci_intr1: enter\n"));

	/* In case the interrupt occurs before initialization has completed. */
	if (sc == NULL) {
#ifdef DIAGNOSTIC
		printf("ehci_intr: sc == NULL\n");
#endif
		return (0);
	}

	intrs = EHCI_STS_INTRS(EOREAD4(sc, EHCI_USBSTS));

	if (!intrs)
		return (0);

	EOWRITE4(sc, EHCI_USBSTS, intrs); /* Acknowledge */
	eintrs = intrs & sc->sc_eintrs;
	DPRINTFN(7, ("ehci_intr: sc=%p intrs=0x%x(0x%x) eintrs=0x%x\n", 
		     sc, (u_int)intrs, EOREAD4(sc, EHCI_USBSTS),
		     (u_int)eintrs));
	if (!eintrs)
		return (0);

	sc->sc_bus.intr_context++;
	sc->sc_bus.no_intrs++;
	if (eintrs & EHCI_STS_INT) {
		DPRINTF(("ehci_intr1: something is done\n"));
		eintrs &= ~EHCI_STS_INT;
	}
	if (eintrs & EHCI_STS_ERRINT) {
		DPRINTF(("ehci_intr1: some error\n"));
		eintrs &= ~EHCI_STS_HSE;
	}
	if (eintrs & EHCI_STS_HSE) {
		printf("%s: unrecoverable error, controller halted\n",
		       USBDEVNAME(sc->sc_bus.bdev));
		/* XXX what else */
	}
	if (eintrs & EHCI_STS_PCD) {
		ehci_pcd(sc, sc->sc_intrxfer);
		/* 
		 * Disable PCD interrupt for now, because it will be
		 * on until the port has been reset.
		 */
		ehci_pcd_able(sc, 0);
		/* Do not allow RHSC interrupts > 1 per second */
                usb_callout(sc->sc_tmo_pcd, hz, ehci_pcd_enable, sc);
		eintrs &= ~EHCI_STS_PCD;
	}

	sc->sc_bus.intr_context--;

	if (eintrs != 0) {
		/* Block unprocessed interrupts. */
		sc->sc_eintrs &= ~eintrs;
		EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);
		printf("%s: blocking intrs 0x%x\n",
		       USBDEVNAME(sc->sc_bus.bdev), eintrs);
	}

	return (1);
}

void
ehci_pcd_able(ehci_softc_t *sc, int on)
{
	DPRINTFN(4, ("ehci_pcd_able: on=%d\n", on));
	if (on)
		sc->sc_eintrs |= EHCI_STS_PCD;
	else
		sc->sc_eintrs &= ~EHCI_STS_PCD;
	EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);
}

void
ehci_pcd_enable(void *v_sc)
{
	ehci_softc_t *sc = v_sc;

	ehci_pcd_able(sc, 1);
}

void
ehci_pcd(ehci_softc_t *sc, usbd_xfer_handle xfer)
{
	usbd_pipe_handle pipe;
	struct ehci_pipe *opipe;
	u_char *p;
	int i, m;

	if (xfer == NULL) {
		/* Just ignore the change. */
		return;
	}

	pipe = xfer->pipe;
	opipe = (struct ehci_pipe *)pipe;

	p = KERNADDR(&xfer->dmabuf);
	m = min(sc->sc_noport, xfer->length * 8 - 1);
	memset(p, 0, xfer->length);
	for (i = 1; i <= m; i++) {
		/* Pick out CHANGE bits from the status reg. */
		if (EOREAD4(sc, EHCI_PORTSC(i)) & EHCI_PS_CLEAR)
			p[i/8] |= 1 << (i%8);
	}
	DPRINTF(("ehci_pcd: change=0x%02x\n", *p));
	xfer->actlen = xfer->length;
	xfer->status = USBD_NORMAL_COMPLETION;

	usb_transfer_complete(xfer);
}

void
ehci_softintr(void *v)
{
	//ehci_softc_t *sc = v;
}

void
ehci_poll(struct usbd_bus *bus)
{
	ehci_softc_t *sc = (ehci_softc_t *)bus;
#ifdef EHCI_DEBUG
	static int last;
	int new;
	new = EHCI_STS_INTRS(EOREAD4(sc, EHCI_USBSTS));
	if (new != last) {
		DPRINTFN(10,("ehci_poll: intrs=0x%04x\n", new));
		last = new;
	}
#endif

	if (EOREAD4(sc, EHCI_USBSTS) & sc->sc_eintrs)
		ehci_intr1(sc);
}

int
ehci_detach(struct ehci_softc *sc, int flags)
{
	int rv = 0;

	if (sc->sc_child != NULL)
		rv = config_detach(sc->sc_child, flags);
	
	if (rv != 0)
		return (rv);

	usb_uncallout(sc->sc_tmo_pcd, ehci_pcd_enable, sc);

	if (sc->sc_powerhook != NULL)
		powerhook_disestablish(sc->sc_powerhook);
	if (sc->sc_shutdownhook != NULL)
		shutdownhook_disestablish(sc->sc_shutdownhook);

	/* XXX free other data structures XXX */

	return (rv);
}


int
ehci_activate(device_ptr_t self, enum devact act)
{
	struct ehci_softc *sc = (struct ehci_softc *)self;
	int rv = 0;

	switch (act) {
	case DVACT_ACTIVATE:
		return (EOPNOTSUPP);
		break;

	case DVACT_DEACTIVATE:
		if (sc->sc_child != NULL)
			rv = config_deactivate(sc->sc_child);
		sc->sc_dying = 1;
		break;
	}
	return (rv);
}

/*
 * Handle suspend/resume.
 *
 * We need to switch to polling mode here, because this routine is
 * called from an intterupt context.  This is all right since we
 * are almost suspended anyway.
 */
void
ehci_power(int why, void *v)
{
	ehci_softc_t *sc = v;
	//u_int32_t ctl;
	int s;

#ifdef EHCI_DEBUG
	DPRINTF(("ehci_power: sc=%p, why=%d\n", sc, why));
	ehci_dumpregs(sc);
#endif

	s = splhardusb();
	switch (why) {
	case PWR_SUSPEND:
	case PWR_STANDBY:
		sc->sc_bus.use_polling++;
#if 0
OOO
		ctl = OREAD4(sc, EHCI_CONTROL) & ~EHCI_HCFS_MASK;
		if (sc->sc_control == 0) {
			/*
			 * Preserve register values, in case that APM BIOS
			 * does not recover them.
			 */
			sc->sc_control = ctl;
			sc->sc_intre = OREAD4(sc, EHCI_INTERRUPT_ENABLE);
		}
		ctl |= EHCI_HCFS_SUSPEND;
		OWRITE4(sc, EHCI_CONTROL, ctl);
#endif
		usb_delay_ms(&sc->sc_bus, USB_RESUME_WAIT);
		sc->sc_bus.use_polling--;
		break;
	case PWR_RESUME:
		sc->sc_bus.use_polling++;
#if 0
OOO
		/* Some broken BIOSes do not recover these values */
		OWRITE4(sc, EHCI_HCCA, DMAADDR(&sc->sc_hccadma));
		OWRITE4(sc, EHCI_CONTROL_HEAD_ED, sc->sc_ctrl_head->physaddr);
		OWRITE4(sc, EHCI_BULK_HEAD_ED, sc->sc_bulk_head->physaddr);
		if (sc->sc_intre)
			OWRITE4(sc, EHCI_INTERRUPT_ENABLE,
				sc->sc_intre & (EHCI_ALL_INTRS | EHCI_MIE));
		if (sc->sc_control)
			ctl = sc->sc_control;
		else
			ctl = OREAD4(sc, EHCI_CONTROL);
		ctl |= EHCI_HCFS_RESUME;
		OWRITE4(sc, EHCI_CONTROL, ctl);
		usb_delay_ms(&sc->sc_bus, USB_RESUME_DELAY);
		ctl = (ctl & ~EHCI_HCFS_MASK) | EHCI_HCFS_OPERATIONAL;
		OWRITE4(sc, EHCI_CONTROL, ctl);
		usb_delay_ms(&sc->sc_bus, USB_RESUME_RECOVERY);
		sc->sc_control = sc->sc_intre = 0;
#endif
		sc->sc_bus.use_polling--;
		break;
	case PWR_SOFTSUSPEND:
	case PWR_SOFTSTANDBY:
	case PWR_SOFTRESUME:
		break;
	}
	splx(s);
}

/*
 * Shut down the controller when the system is going down.
 */
void
ehci_shutdown(void *v)
{
	ehci_softc_t *sc = v;

	DPRINTF(("ehci_shutdown: stopping the HC\n"));
	EOWRITE4(sc, EHCI_USBCMD, 0);	/* Halt controller */
	EOWRITE4(sc, EHCI_USBCMD, EHCI_CMD_HCRESET);
}

usbd_status
ehci_allocm(struct usbd_bus *bus, usb_dma_t *dma, u_int32_t size)
{
	struct ehci_softc *sc = (struct ehci_softc *)bus;

	return (usb_allocmem(&sc->sc_bus, size, 0, dma));
}

void
ehci_freem(struct usbd_bus *bus, usb_dma_t *dma)
{
	struct ehci_softc *sc = (struct ehci_softc *)bus;

	usb_freemem(&sc->sc_bus, dma);
}

usbd_xfer_handle
ehci_allocx(struct usbd_bus *bus)
{
	struct ehci_softc *sc = (struct ehci_softc *)bus;
	usbd_xfer_handle xfer;

	xfer = SIMPLEQ_FIRST(&sc->sc_free_xfers);
	if (xfer != NULL)
		SIMPLEQ_REMOVE_HEAD(&sc->sc_free_xfers, xfer, next);
	else
		xfer = malloc(sizeof(*xfer), M_USB, M_NOWAIT);
	if (xfer != NULL)
		memset(xfer, 0, sizeof *xfer);
	return (xfer);
}

void
ehci_freex(struct usbd_bus *bus, usbd_xfer_handle xfer)
{
	struct ehci_softc *sc = (struct ehci_softc *)bus;

	SIMPLEQ_INSERT_HEAD(&sc->sc_free_xfers, xfer, next);
}

Static void
ehci_device_clear_toggle(usbd_pipe_handle pipe)
{
#if 0
OOO
	struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;

	epipe->sed->ed.ed_headp &= htole32(~EHCI_TOGGLECARRY);
#endif
}

Static void
ehci_noop(usbd_pipe_handle pipe)
{
}

#ifdef EHCI_DEBUG
void
ehci_dumpregs(ehci_softc_t *sc)
{
	int i;
	printf("cmd=0x%08x, sts=0x%08x, ien=0x%08x\n",
	       EOREAD4(sc, EHCI_USBCMD),
	       EOREAD4(sc, EHCI_USBSTS),
	       EOREAD4(sc, EHCI_USBINTR));
	for (i = 1; i <= sc->sc_noport; i++)
		printf("port %d status=0x%08x\n", i, 
		       EOREAD4(sc, EHCI_PORTSC(i)));
}

void
ehci_dump()
{
	ehci_dumpregs(theehci);
}

void
ehci_dump_link(ehci_link_t link)
{
	printf("0x%08x<", link);
	switch (EHCI_LINK_TYPE(link)) {
	case EHCI_LINK_ITD: printf("ITD"); break;
	case EHCI_LINK_QH: printf("QH"); break;
	case EHCI_LINK_SITD: printf("SITD"); break;
	case EHCI_LINK_FSTN: printf("FSTN"); break;
	}
	if (link & EHCI_LINK_TERMINATE)
		printf(",T>");
	else
		printf(">");
}

void
ehci_dump_sqtd(ehci_soft_qtd_t *sqtd)
{
	printf("QTD(%p) at 0x%08x:\n", sqtd, sqtd->physaddr);
	ehci_dump_qtd(&sqtd->qtd);
}

void
ehci_dump_qtd(ehci_qtd_t *qtd)
{
	u_int32_t s;

	printf("  next="); ehci_dump_link(qtd->qtd_next);
	printf("altnext="); ehci_dump_link(qtd->qtd_altnext);
	printf("\n");
	s = qtd->qtd_status;
	printf("  status=0x%08x: toggle=%d bytes=0x%x ioc=%d c_page=0x%x\n",
	       s, EHCI_QTD_GET_TOGGLE(s), EHCI_QTD_GET_BYTES(s),
	       EHCI_QTD_GET_IOC(s), EHCI_QTD_GET_C_PAGE(s));
	printf("    cerr=%d pid=%d stat=0x%02x\n", EHCI_QTD_GET_CERR(s),
	       EHCI_QTD_GET_PID(s), EHCI_QTD_GET_STATUS(s));
	for (s = 0; s < 5; s++)
		printf("  buffer[%d]=0x%08x\n", s, qtd->qtd_buffer[s]);
}

void
ehci_dump_sqh(ehci_soft_qh_t *sqh)
{
	ehci_qh_t *qh = &sqh->qh;

	printf("QH(%p) at 0x%08x:\n", sqh, sqh->physaddr);
	printf("  link="); ehci_dump_link(qh->qh_link); printf("\n");
	printf("  endp=0x%08x endphub=0x%08x\n", qh->qh_endp, qh->qh_endphub);
	printf("  curqtd="); ehci_dump_link(qh->qh_curqtd); printf("\n  ");
	ehci_dump_qtd(&qh->qh_qtd);
}

#endif

usbd_status
ehci_open(usbd_pipe_handle pipe)
{
	usbd_device_handle dev = pipe->device;
	ehci_softc_t *sc = (ehci_softc_t *)dev->bus;
	usb_endpoint_descriptor_t *ed = pipe->endpoint->edesc;
	u_int8_t addr = dev->address;
#if 0
	u_int8_t xfertype = ed->bmAttributes & UE_XFERTYPE;
	struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;
	ehci_soft_ed_t *sqh;
	ehci_soft_qtd_t *sqtd;
	ehci_soft_itd_t *sitd;
	ehci_physaddr_t tdphys;
	u_int32_t fmt;
	usbd_status err;
	int s;
	int ival;
#endif

	DPRINTFN(1, ("ehci_open: pipe=%p, addr=%d, endpt=%d (%d)\n",
		     pipe, addr, ed->bEndpointAddress, sc->sc_addr));

	if (addr == sc->sc_addr) {
		switch (ed->bEndpointAddress) {
		case USB_CONTROL_ENDPOINT:
			pipe->methods = &ehci_root_ctrl_methods;
			break;
		case UE_DIR_IN | EHCI_INTR_ENDPT:
			pipe->methods = &ehci_root_intr_methods;
			break;
		default:
			return (USBD_INVAL);
		}
	} else {
#if 0
		sqtd = NULL;
		sqh = NULL;

		sqh = ehci_alloc_sqh(sc);
		if (sqh == NULL)
			goto bad0;
		epipe->sqh = sqh;
		if (xfertype == UE_ISOCHRONOUS || xfertype == UE_INTERRUPT) {
			return (USBD_IOERROR);
		} else {
			sqtd = ehci_alloc_sqtd(sc);
			if (sqtd == NULL) {
				ehci_free_sqtd(sc, sqtd);
				goto bad1;
			}
			epipe->tail.qtd = sqtd;
			tdphys = sqtd->physaddr;
			fmt = EHCI_ED_FORMAT_GEN | EHCI_ED_DIR_TD;
		}
		sqh->ed.ed_flags = htole32(
			EHCI_ED_SET_FA(addr) | 
			EHCI_ED_SET_EN(ed->bEndpointAddress) |
			(dev->lowspeed ? EHCI_ED_SPEED : 0) | fmt |
			EHCI_ED_SET_MAXP(UGETW(ed->wMaxPacketSize)));
		sqh->ed.ed_headp = sqh->ed.ed_tailp = htole32(tdphys);

		switch (xfertype) {
		case UE_CONTROL:
			pipe->methods = &ehci_device_ctrl_methods;
			err = usb_allocmem(&sc->sc_bus, 
				  sizeof(usb_device_request_t), 
				  0, &epipe->u.ctl.reqdma);
			if (err)
				goto bad;
			s = splusb();
			ehci_add_ed(sqh, sc->sc_ctrl_head);
			splx(s);
			break;
		case UE_INTERRUPT:
			pipe->methods = &ehci_device_intr_methods;
			ival = pipe->interval;
			if (ival == USBD_DEFAULT_INTERVAL)
				ival = ed->bInterval;
			return (ehci_device_setintr(sc, epipe, ival));
		case UE_ISOCHRONOUS:
			pipe->methods = &ehci_device_isoc_methods;
			return (ehci_setup_isoc(pipe));
		case UE_BULK:
			pipe->methods = &ehci_device_bulk_methods;
			s = splusb();
			ehci_add_ed(sqh, sc->sc_bulk_head);
			splx(s);
			break;
		}
#else	
		return (USBD_IOERROR);
#endif
	}
	return (USBD_NORMAL_COMPLETION);

#if 0
 bad:
	if (sqtd != NULL)
		ehci_free_sqtd(sc, sqtd);
 bad1:
	if (sqh != NULL)
		ehci_free_sqh(sc, sqh);
 bad0:
	return (USBD_NOMEM);
#endif
}

/***********/

/*
 * Data structures and routines to emulate the root hub.
 */
Static usb_device_descriptor_t ehci_devd = {
	USB_DEVICE_DESCRIPTOR_SIZE,
	UDESC_DEVICE,		/* type */
	{0x00, 0x02},		/* USB version */
	UDCLASS_HUB,		/* class */
	UDSUBCLASS_HUB,		/* subclass */
	0,			/* protocol */
	64,			/* max packet */
	{0},{0},{0x00,0x01},	/* device id */
	1,2,0,			/* string indicies */
	1			/* # of configurations */
};

Static usb_config_descriptor_t ehci_confd = {
	USB_CONFIG_DESCRIPTOR_SIZE,
	UDESC_CONFIG,
	{USB_CONFIG_DESCRIPTOR_SIZE +
	 USB_INTERFACE_DESCRIPTOR_SIZE +
	 USB_ENDPOINT_DESCRIPTOR_SIZE},
	1,
	1,
	0,
	UC_SELF_POWERED,
	0			/* max power */
};

Static usb_interface_descriptor_t ehci_ifcd = {
	USB_INTERFACE_DESCRIPTOR_SIZE,
	UDESC_INTERFACE,
	0,
	0,
	1,
	UICLASS_HUB,
	UISUBCLASS_HUB,
	0,
	0
};

Static usb_endpoint_descriptor_t ehci_endpd = {
	USB_ENDPOINT_DESCRIPTOR_SIZE,
	UDESC_ENDPOINT,
	UE_DIR_IN | EHCI_INTR_ENDPT,
	UE_INTERRUPT,
	{8, 0},			/* max packet */
	255
};

Static usb_hub_descriptor_t ehci_hubd = {
	USB_HUB_DESCRIPTOR_SIZE,
	UDESC_HUB,
	0,
	{0,0},
	0,
	0,
	{0},
};

Static int
ehci_str(p, l, s)
	usb_string_descriptor_t *p;
	int l;
	char *s;
{
	int i;

	if (l == 0)
		return (0);
	p->bLength = 2 * strlen(s) + 2;
	if (l == 1)
		return (1);
	p->bDescriptorType = UDESC_STRING;
	l -= 2;
	for (i = 0; s[i] && l > 1; i++, l -= 2)
		USETW2(p->bString[i], 0, s[i]);
	return (2*i+2);
}

/*
 * Simulate a hardware hub by handling all the necessary requests.
 */
Static usbd_status
ehci_root_ctrl_transfer(usbd_xfer_handle xfer)
{
	usbd_status err;

	/* Insert last in queue. */
	err = usb_insert_transfer(xfer);
	if (err)
		return (err);

	/* Pipe isn't running, start first */
	return (ehci_root_ctrl_start(SIMPLEQ_FIRST(&xfer->pipe->queue)));
}

Static usbd_status
ehci_root_ctrl_start(usbd_xfer_handle xfer)
{
	ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;
	usb_device_request_t *req;
	void *buf = NULL;
	int port, i;
	int s, len, value, index, l, totlen = 0;
	usb_port_status_t ps;
	usb_hub_descriptor_t hubd;
	usbd_status err;
	u_int32_t v;

	if (sc->sc_dying)
		return (USBD_IOERROR);

#ifdef DIAGNOSTIC
	if (!(xfer->rqflags & URQ_REQUEST))
		/* XXX panic */
		return (USBD_INVAL);
#endif
	req = &xfer->request;

	DPRINTFN(4,("ehci_root_ctrl_control type=0x%02x request=%02x\n", 
		    req->bmRequestType, req->bRequest));

	len = UGETW(req->wLength);
	value = UGETW(req->wValue);
	index = UGETW(req->wIndex);

	if (len != 0)
		buf = KERNADDR(&xfer->dmabuf);

#define C(x,y) ((x) | ((y) << 8))
	switch(C(req->bRequest, req->bmRequestType)) {
	case C(UR_CLEAR_FEATURE, UT_WRITE_DEVICE):
	case C(UR_CLEAR_FEATURE, UT_WRITE_INTERFACE):
	case C(UR_CLEAR_FEATURE, UT_WRITE_ENDPOINT):
		/* 
		 * DEVICE_REMOTE_WAKEUP and ENDPOINT_HALT are no-ops
		 * for the integrated root hub.
		 */
		break;
	case C(UR_GET_CONFIG, UT_READ_DEVICE):
		if (len > 0) {
			*(u_int8_t *)buf = sc->sc_conf;
			totlen = 1;
		}
		break;
	case C(UR_GET_DESCRIPTOR, UT_READ_DEVICE):
		DPRINTFN(8,("ehci_root_ctrl_control wValue=0x%04x\n", value));
		switch(value >> 8) {
		case UDESC_DEVICE:
			if ((value & 0xff) != 0) {
				err = USBD_IOERROR;
				goto ret;
			}
			totlen = l = min(len, USB_DEVICE_DESCRIPTOR_SIZE);
			USETW(ehci_devd.idVendor, sc->sc_id_vendor);
			memcpy(buf, &ehci_devd, l);
			break;
		case UDESC_CONFIG:
			if ((value & 0xff) != 0) {
				err = USBD_IOERROR;
				goto ret;
			}
			totlen = l = min(len, USB_CONFIG_DESCRIPTOR_SIZE);
			memcpy(buf, &ehci_confd, l);
			buf = (char *)buf + l;
			len -= l;
			l = min(len, USB_INTERFACE_DESCRIPTOR_SIZE);
			totlen += l;
			memcpy(buf, &ehci_ifcd, l);
			buf = (char *)buf + l;
			len -= l;
			l = min(len, USB_ENDPOINT_DESCRIPTOR_SIZE);
			totlen += l;
			memcpy(buf, &ehci_endpd, l);
			break;
		case UDESC_STRING:
			if (len == 0)
				break;
			*(u_int8_t *)buf = 0;
			totlen = 1;
			switch (value & 0xff) {
			case 1: /* Vendor */
				totlen = ehci_str(buf, len, sc->sc_vendor);
				break;
			case 2: /* Product */
				totlen = ehci_str(buf, len, "EHCI root hub");
				break;
			}
			break;
		default:
			err = USBD_IOERROR;
			goto ret;
		}
		break;
	case C(UR_GET_INTERFACE, UT_READ_INTERFACE):
		if (len > 0) {
			*(u_int8_t *)buf = 0;
			totlen = 1;
		}
		break;
	case C(UR_GET_STATUS, UT_READ_DEVICE):
		if (len > 1) {
			USETW(((usb_status_t *)buf)->wStatus,UDS_SELF_POWERED);
			totlen = 2;
		}
		break;
	case C(UR_GET_STATUS, UT_READ_INTERFACE):
	case C(UR_GET_STATUS, UT_READ_ENDPOINT):
		if (len > 1) {
			USETW(((usb_status_t *)buf)->wStatus, 0);
			totlen = 2;
		}
		break;
	case C(UR_SET_ADDRESS, UT_WRITE_DEVICE):
		if (value >= USB_MAX_DEVICES) {
			err = USBD_IOERROR;
			goto ret;
		}
		sc->sc_addr = value;
		break;
	case C(UR_SET_CONFIG, UT_WRITE_DEVICE):
		if (value != 0 && value != 1) {
			err = USBD_IOERROR;
			goto ret;
		}
		sc->sc_conf = value;
		break;
	case C(UR_SET_DESCRIPTOR, UT_WRITE_DEVICE):
		break;
	case C(UR_SET_FEATURE, UT_WRITE_DEVICE):
	case C(UR_SET_FEATURE, UT_WRITE_INTERFACE):
	case C(UR_SET_FEATURE, UT_WRITE_ENDPOINT):
		err = USBD_IOERROR;
		goto ret;
	case C(UR_SET_INTERFACE, UT_WRITE_INTERFACE):
		break;
	case C(UR_SYNCH_FRAME, UT_WRITE_ENDPOINT):
		break;
	/* Hub requests */
	case C(UR_CLEAR_FEATURE, UT_WRITE_CLASS_DEVICE):
		break;
	case C(UR_CLEAR_FEATURE, UT_WRITE_CLASS_OTHER):
		DPRINTFN(8, ("ehci_root_ctrl_control: UR_CLEAR_PORT_FEATURE "
			     "port=%d feature=%d\n",
			     index, value));
		if (index < 1 || index > sc->sc_noport) {
			err = USBD_IOERROR;
			goto ret;
		}
		port = EHCI_PORTSC(index);
		v = EOREAD4(sc, port) &~ EHCI_PS_CLEAR;
		switch(value) {
		case UHF_PORT_ENABLE:
			EOWRITE4(sc, port, v &~ EHCI_PS_PE);
			break;
		case UHF_PORT_SUSPEND:
			EOWRITE4(sc, port, v &~ EHCI_PS_SUSP);
			break;
		case UHF_PORT_POWER:
			EOWRITE4(sc, port, v &~ EHCI_PS_PP);
			break;
		case UHF_C_PORT_CONNECTION:
			EOWRITE4(sc, port, v | EHCI_PS_CSC);
			break;
		case UHF_C_PORT_ENABLE:
			EOWRITE4(sc, port, v | EHCI_PS_PEC);
			break;
		case UHF_C_PORT_SUSPEND:
			/* how? */
			break;
		case UHF_C_PORT_OVER_CURRENT:
			EOWRITE4(sc, port, v | EHCI_PS_OCC);
			break;
		case UHF_C_PORT_RESET:
			sc->sc_isreset = 0;
			break;
		default:
			err = USBD_IOERROR;
			goto ret;
		}
#if 0
		switch(value) {
		case UHF_C_PORT_CONNECTION:
		case UHF_C_PORT_ENABLE:
		case UHF_C_PORT_SUSPEND:
		case UHF_C_PORT_OVER_CURRENT:
		case UHF_C_PORT_RESET:
			/* Enable RHSC interrupt if condition is cleared. */
			if ((OREAD4(sc, port) >> 16) == 0)
				ehci_pcd_able(sc, 1);
			break;
		default:
			break;
		}
#endif
		break;
	case C(UR_GET_DESCRIPTOR, UT_READ_CLASS_DEVICE):
		if (value != 0) {
			err = USBD_IOERROR;
			goto ret;
		}
		hubd = ehci_hubd;
		hubd.bNbrPorts = sc->sc_noport;
		v = EOREAD4(sc, EHCI_HCSPARAMS);
		USETW(hubd.wHubCharacteristics,
		      EHCI_HCS_PPC(v) ? UHD_PWR_INDIVIDUAL : UHD_PWR_NO_SWITCH);
		hubd.bPwrOn2PwrGood = 200; /* XXX can't find out? */
		for (i = 0, l = sc->sc_noport; l > 0; i++, l -= 8, v >>= 8) 
			hubd.DeviceRemovable[i++] = 0; /* XXX can't find out? */
		hubd.bDescLength = USB_HUB_DESCRIPTOR_SIZE + i;
		l = min(len, hubd.bDescLength);
		totlen = l;
		memcpy(buf, &hubd, l);
		break;
	case C(UR_GET_STATUS, UT_READ_CLASS_DEVICE):
		if (len != 4) {
			err = USBD_IOERROR;
			goto ret;
		}
		memset(buf, 0, len); /* ? XXX */
		totlen = len;
		break;
	case C(UR_GET_STATUS, UT_READ_CLASS_OTHER):
		DPRINTFN(8,("ehci_root_ctrl_transfer: get port status i=%d\n",
			    index));
		if (index < 1 || index > sc->sc_noport) {
			err = USBD_IOERROR;
			goto ret;
		}
		if (len != 4) {
			err = USBD_IOERROR;
			goto ret;
		}
		v = EOREAD4(sc, EHCI_PORTSC(index));
		DPRINTFN(8,("ehci_root_ctrl_transfer: port status=0x%04x\n",
			    v));
		i = 0;
		if (v & EHCI_PS_CS)	i |= UPS_CURRENT_CONNECT_STATUS;
		if (v & EHCI_PS_PE)	i |= UPS_PORT_ENABLED;
		if (v & EHCI_PS_SUSP)	i |= UPS_SUSPEND;
		if (v & EHCI_PS_OCA)	i |= UPS_OVERCURRENT_INDICATOR;
		if (v & EHCI_PS_PR)	i |= UPS_RESET;
		if (v & EHCI_PS_PP)	i |= UPS_PORT_POWER;
		USETW(ps.wPortStatus, i);
		i = 0;
		if (v & EHCI_PS_CSC)	i |= UPS_C_CONNECT_STATUS;
		if (v & EHCI_PS_PEC)	i |= UPS_C_PORT_ENABLED;
		if (v & EHCI_PS_OCC)	i |= UPS_C_OVERCURRENT_INDICATOR;
		if (sc->sc_isreset)	i |= UPS_C_PORT_RESET;
		USETW(ps.wPortChange, i);
		l = min(len, sizeof ps);
		memcpy(buf, &ps, l);
		totlen = l;
		break;
	case C(UR_SET_DESCRIPTOR, UT_WRITE_CLASS_DEVICE):
		err = USBD_IOERROR;
		goto ret;
	case C(UR_SET_FEATURE, UT_WRITE_CLASS_DEVICE):
		break;
	case C(UR_SET_FEATURE, UT_WRITE_CLASS_OTHER):
		if (index < 1 || index > sc->sc_noport) {
			err = USBD_IOERROR;
			goto ret;
		}
		port = EHCI_PORTSC(index);
		v = EOREAD4(sc, port) &~ EHCI_PS_CLEAR;
		switch(value) {
		case UHF_PORT_ENABLE:
			EOWRITE4(sc, port, v | EHCI_PS_PE);
			break;
		case UHF_PORT_SUSPEND:
			EOWRITE4(sc, port, v | EHCI_PS_SUSP);
			break;
		case UHF_PORT_RESET:
			DPRINTFN(5,("ehci_root_ctrl_transfer: reset port %d\n",
				    index));
			if (EHCI_PS_IS_LOWSPEED(v)) {
				/* Low speed device, give up ownership. */
				ehci_disown(sc, index, 1);
				break;
			}
			/* Start reset sequence. */
			v &= ~ (EHCI_PS_PE | EHCI_PS_PR);
			EOWRITE4(sc, port, v | EHCI_PS_PR);
			/* Wait for reset to complete. */
			usb_delay_ms(&sc->sc_bus, USB_PORT_RESET_DELAY * 2);
			/* Terminate reset sequence. */
			EOWRITE4(sc, port, v);
			/* Wait for HC to complete reset. */
			usb_delay_ms(&sc->sc_bus, EHCI_PORT_RESET_COMPLETE * 2);
			v = EOREAD4(sc, port);
			DPRINTF(("ehci after reset, status=0x%08x\n", v));
			if (v & EHCI_PS_PR) {
				printf("%s: port reset timeout\n",
				       USBDEVNAME(sc->sc_bus.bdev));
				return (USBD_TIMEOUT);
			}
			if (!(v & EHCI_PS_PE)) {
				/* Not a high speed device, give up ownership.*/
				ehci_disown(sc, index, 0);
				break;
			}
			sc->sc_isreset = 1;
			DPRINTF(("ehci port %d reset, status = 0x%08x\n",
				 index, v));
			break;
		case UHF_PORT_POWER:
			DPRINTFN(2,("ehci_root_ctrl_transfer: set port power "
				    "%d\n", index));
			EOWRITE4(sc, port, v | EHCI_PS_PP);
			break;
		default:
			err = USBD_IOERROR;
			goto ret;
		}
		break;
	default:
		err = USBD_IOERROR;
		goto ret;
	}
	xfer->actlen = totlen;
	err = USBD_NORMAL_COMPLETION;
 ret:
	xfer->status = err;
	s = splusb();
	usb_transfer_complete(xfer);
	splx(s);
	return (USBD_IN_PROGRESS);
}

void
ehci_disown(ehci_softc_t *sc, int index, int lowspeed)
{
	int i, port;
	u_int32_t v;

	DPRINTF(("ehci_disown: index=%d lowspeed=%d\n", index, lowspeed));
#ifdef DIAGNOSTIC
	if (sc->sc_npcomp != 0) {
		i = (index-1) / sc->sc_npcomp;
		if (i >= sc->sc_ncomp)
			printf("%s: strange port\n",
			       USBDEVNAME(sc->sc_bus.bdev));
		else
			printf("%s: handing over %s speed device on "
			       "port %d to %s\n",
			       USBDEVNAME(sc->sc_bus.bdev),
			       lowspeed ? "low" : "full",
			       index, USBDEVNAME(sc->sc_comps[i]->bdev));
	} else {
		printf("%s: npcomp == 0\n", USBDEVNAME(sc->sc_bus.bdev));
	}
#endif
	port = EHCI_PORTSC(index);
	v = EOREAD4(sc, port) &~ EHCI_PS_CLEAR;
	EOWRITE4(sc, port, v | EHCI_PS_PO);
}

/* Abort a root control request. */
Static void
ehci_root_ctrl_abort(usbd_xfer_handle xfer)
{
	/* Nothing to do, all transfers are synchronous. */
}

/* Close the root pipe. */
Static void
ehci_root_ctrl_close(usbd_pipe_handle pipe)
{
	DPRINTF(("ehci_root_ctrl_close\n"));
	/* Nothing to do. */
}

void
ehci_root_intr_done(usbd_xfer_handle xfer)
{
	xfer->hcpriv = NULL;
}

Static usbd_status
ehci_root_intr_transfer(usbd_xfer_handle xfer)
{
	usbd_status err;

	/* Insert last in queue. */
	err = usb_insert_transfer(xfer);
	if (err)
		return (err);

	/* Pipe isn't running, start first */
	return (ehci_root_intr_start(SIMPLEQ_FIRST(&xfer->pipe->queue)));
}

Static usbd_status
ehci_root_intr_start(usbd_xfer_handle xfer)
{
	usbd_pipe_handle pipe = xfer->pipe;
	ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;

	if (sc->sc_dying)
		return (USBD_IOERROR);

	sc->sc_intrxfer = xfer;

	return (USBD_IN_PROGRESS);
}

/* Abort a root interrupt request. */
Static void
ehci_root_intr_abort(usbd_xfer_handle xfer)
{
	int s;

	if (xfer->pipe->intrxfer == xfer) {
		DPRINTF(("ehci_root_intr_abort: remove\n"));
		xfer->pipe->intrxfer = NULL;
	}
	xfer->status = USBD_CANCELLED;
	s = splusb();
	usb_transfer_complete(xfer);
	splx(s);
}

/* Close the root pipe. */
Static void
ehci_root_intr_close(usbd_pipe_handle pipe)
{
	ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;
	
	DPRINTF(("ehci_root_intr_close\n"));

	sc->sc_intrxfer = NULL;
}

void
ehci_root_ctrl_done(usbd_xfer_handle xfer)
{
	xfer->hcpriv = NULL;
}

/************************/

ehci_soft_qh_t *
ehci_alloc_sqh(ehci_softc_t *sc)
{
	ehci_soft_qh_t *sqh;
	usbd_status err;
	int i, offs;
	usb_dma_t dma;

	if (sc->sc_freeqhs == NULL) {
		DPRINTFN(2, ("ehci_alloc_sqh: allocating chunk\n"));
		err = usb_allocmem(&sc->sc_bus, EHCI_SQH_SIZE * EHCI_SQH_CHUNK,
			  EHCI_PAGE_SIZE, &dma);
		if (err)
			return (0);
		for(i = 0; i < EHCI_SQH_CHUNK; i++) {
			offs = i * EHCI_SQH_SIZE;
			sqh = (ehci_soft_qh_t *)((char *)KERNADDR(&dma) +offs);
			sqh->physaddr = DMAADDR(&dma) + offs;
			sqh->next = sc->sc_freeqhs;
			sc->sc_freeqhs = sqh;
		}
	}
	sqh = sc->sc_freeqhs;
	sc->sc_freeqhs = sqh->next;
	memset(&sqh->qh, 0, sizeof(ehci_qh_t));
	sqh->next = 0;
	return (sqh);
}

void
ehci_free_sqh(ehci_softc_t *sc, ehci_soft_qh_t *sqh)
{
	sqh->next = sc->sc_freeqhs;
	sc->sc_freeqhs = sqh;
}

ehci_soft_qtd_t *
ehci_alloc_sqtd(ehci_softc_t *sc)
{
	ehci_soft_qtd_t *sqtd;
	usbd_status err;
	int i, offs;
	usb_dma_t dma;
	int s;

	if (sc->sc_freeqtds == NULL) {
		DPRINTFN(2, ("ehci_alloc_sqtd: allocating chunk\n"));
		err = usb_allocmem(&sc->sc_bus, EHCI_SQTD_SIZE*EHCI_SQTD_CHUNK,
			  EHCI_PAGE_SIZE, &dma);
		if (err)
			return (NULL);
		s = splusb();
		for(i = 0; i < EHCI_SQTD_CHUNK; i++) {
			offs = i * EHCI_SQTD_SIZE;
			sqtd = (ehci_soft_qtd_t *)((char *)KERNADDR(&dma)+offs);
			sqtd->physaddr = DMAADDR(&dma) + offs;
			sqtd->nextqtd = sc->sc_freeqtds;
			sc->sc_freeqtds = sqtd;
		}
		splx(s);
	}

	s = splusb();
	sqtd = sc->sc_freeqtds;
	sc->sc_freeqtds = sqtd->nextqtd;
	memset(&sqtd->qtd, 0, sizeof(ehci_qtd_t));
	sqtd->nextqtd = NULL;
	sqtd->xfer = NULL;
	ehci_hash_add_qtd(sc, sqtd);
	splx(s);

	return (sqtd);
}

void
ehci_free_sqtd(ehci_softc_t *sc, ehci_soft_qtd_t *sqtd)
{
	int s;

	s = splusb();
	ehci_hash_rem_qtd(sc, sqtd);
	sqtd->nextqtd = sc->sc_freeqtds;
	sc->sc_freeqtds = sqtd;
	splx(s);
}

/*
 * When a transfer is completed the TD is added to the done queue by
 * the host controller.  This queue is the processed by software.
 * Unfortunately the queue contains the physical address of the TD
 * and we have no simple way to translate this back to a kernel address.
 * To make the translation possible (and fast) we use a hash table of
 * TDs currently in the schedule.  The physical address is used as the
 * hash value.
 */

#define HASH(a) (((a) >> 4) % EHCI_HASH_SIZE)
/* Called at splusb() */
void
ehci_hash_add_qtd(ehci_softc_t *sc, ehci_soft_qtd_t *sqtd)
{
	int h = HASH(sqtd->physaddr);

	SPLUSBCHECK;

	LIST_INSERT_HEAD(&sc->sc_hash_qtds[h], sqtd, hnext);
}

/* Called at splusb() */
void
ehci_hash_rem_qtd(ehci_softc_t *sc, ehci_soft_qtd_t *sqtd)
{
	SPLUSBCHECK;

	LIST_REMOVE(sqtd, hnext);
}

ehci_soft_qtd_t *
ehci_hash_find_qtd(ehci_softc_t *sc, ehci_physaddr_t a)
{
	int h = HASH(a);
	ehci_soft_qtd_t *sqtd;

	for (sqtd = LIST_FIRST(&sc->sc_hash_qtds[h]); 
	     sqtd != NULL;
	     sqtd = LIST_NEXT(sqtd, hnext))
		if (sqtd->physaddr == a)
			return (sqtd);
	return (NULL);
}

/************************/

Static usbd_status	ehci_device_ctrl_transfer(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static usbd_status	ehci_device_ctrl_start(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static void		ehci_device_ctrl_abort(usbd_xfer_handle xfer) { }
Static void		ehci_device_ctrl_close(usbd_pipe_handle pipe) { }
Static void		ehci_device_ctrl_done(usbd_xfer_handle xfer) { }

Static usbd_status	ehci_device_bulk_transfer(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static usbd_status	ehci_device_bulk_start(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static void		ehci_device_bulk_abort(usbd_xfer_handle xfer) { }
Static void		ehci_device_bulk_close(usbd_pipe_handle pipe) { }
Static void		ehci_device_bulk_done(usbd_xfer_handle xfer) { }

Static usbd_status	ehci_device_intr_transfer(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static usbd_status	ehci_device_intr_start(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static void		ehci_device_intr_abort(usbd_xfer_handle xfer) { }
Static void		ehci_device_intr_close(usbd_pipe_handle pipe) { }
Static void		ehci_device_intr_done(usbd_xfer_handle xfer) { }

Static usbd_status	ehci_device_isoc_transfer(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static usbd_status	ehci_device_isoc_start(usbd_xfer_handle xfer) { return USBD_IOERROR; }
Static void		ehci_device_isoc_abort(usbd_xfer_handle xfer) { }
Static void		ehci_device_isoc_close(usbd_pipe_handle pipe) { }
Static void		ehci_device_isoc_done(usbd_xfer_handle xfer) { }
