/*	$NetBSD: bpp.c,v 1.33 2008/03/02 15:54:58 nakayama Exp $ */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: bpp.c,v 1.33 2008/03/02 15:54:58 nakayama Exp $");

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/signalvar.h>
#include <sys/conf.h>
#include <sys/errno.h>
#include <sys/device.h>

#include <sys/bus.h>
#include <sys/intr.h>
#include <machine/autoconf.h>

#include <dev/ic/lsi64854reg.h>
#include <dev/ic/lsi64854var.h>

#include <dev/sbus/sbusvar.h>
#include <dev/sbus/bppreg.h>

#define splbpp()	spltty()	/* XXX */

#ifdef DEBUG
#define DPRINTF(x) do { if (bppdebug) printf x ; } while (0)
int bppdebug = 1;
#else
#define DPRINTF(x)
#endif

#if 0
struct bpp_param {
	int	bpp_dss;		/* data setup to strobe */
	int	bpp_dsw;		/* data strobe width */
	int	bpp_outputpins;		/* Select/Autofeed/Init pins */
	int	bpp_inputpins;		/* Error/Select/Paperout pins */
};
#endif

struct hwstate {
	u_int16_t	hw_hcr;		/* Hardware config register */
	u_int16_t	hw_ocr;		/* Operation config register */
	u_int8_t	hw_tcr;		/* Transfer Control register */
	u_int8_t	hw_or;		/* Output register */
	u_int16_t	hw_irq;		/* IRQ; polarity bits only */
};

struct bpp_softc {
	struct lsi64854_softc	sc_lsi64854;	/* base device */
	struct sbusdev	sc_sd;			/* sbus device */

	size_t		sc_bufsz;		/* temp buffer */
	void *		sc_buf;

	int		sc_error;		/* bottom-half error */
	int		sc_flags;
#define BPP_OPEN	0x01		/* Device is open */
#define BPP_XCLUDE	0x02		/* Exclusive-open mode */
#define BPP_ASYNC	0x04		/* Asynchronous I/O mode */
#define BPP_LOCKED	0x08		/* DMA in progress */
#define BPP_WANT	0x10		/* Waiting for DMA */

	struct selinfo	sc_rsel;
	struct selinfo	sc_wsel;
	struct proc	*sc_asyncproc;	/* Process to notify if async */

	/* Hardware state */
	struct hwstate		sc_hwdefault;
	struct hwstate		sc_hwcurrent;
};

static int	bppmatch(struct device *, struct cfdata *, void *);
static void	bppattach(struct device *, struct device *, void *);
static int	bppintr		(void *);
static void	bpp_setparams(struct bpp_softc *, struct hwstate *);

CFATTACH_DECL(bpp, sizeof(struct bpp_softc),
    bppmatch, bppattach, NULL, NULL);

extern struct cfdriver bpp_cd;

dev_type_open(bppopen);
dev_type_close(bppclose);
dev_type_write(bppwrite);
dev_type_ioctl(bppioctl);
dev_type_poll(bpppoll);
dev_type_kqfilter(bppkqfilter);

const struct cdevsw bpp_cdevsw = {
	bppopen, bppclose, noread, bppwrite, bppioctl,
	nostop, notty, bpppoll, nommap, bppkqfilter, D_TTY
};

#define BPPUNIT(dev)	(minor(dev))


int
bppmatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct sbus_attach_args *sa = aux;

	return (strcmp("SUNW,bpp", sa->sa_name) == 0);
}

void
bppattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct sbus_attach_args *sa = aux;
	struct bpp_softc *dsc = (void *)self;
	struct lsi64854_softc *sc = &dsc->sc_lsi64854;
	int burst, sbusburst;
	int node;

	selinit(&dsc->sc_rsel);
	selinit(&dsc->sc_wsel);

	sc->sc_bustag = sa->sa_bustag;
	sc->sc_dmatag = sa->sa_dmatag;
	node = sa->sa_node;

	/* Map device registers */
	if (sbus_bus_map(sa->sa_bustag,
			 sa->sa_slot, sa->sa_offset, sa->sa_size,
			 0, &sc->sc_regs) != 0) {
		printf("%s: cannot map registers\n", self->dv_xname);
		return;
	}

	/*
	 * Get transfer burst size from PROM and plug it into the
	 * controller registers. This is needed on the Sun4m; do
	 * others need it too?
	 */
	sbusburst = ((struct sbus_softc *)parent)->sc_burst;
	if (sbusburst == 0)
		sbusburst = SBUS_BURST_32 - 1; /* 1->16 */

	burst = prom_getpropint(node, "burst-sizes", -1);
	if (burst == -1)
		/* take SBus burst sizes */
		burst = sbusburst;

	/* Clamp at parent's burst sizes */
	burst &= sbusburst;
	sc->sc_burst = (burst & SBUS_BURST_32) ? 32 :
		       (burst & SBUS_BURST_16) ? 16 : 0;

	/* Join the Sbus device family */
	dsc->sc_sd.sd_reset = (void *)0;
	sbus_establish(&dsc->sc_sd, self);

	/* Initialize the DMA channel */
	sc->sc_channel = L64854_CHANNEL_PP;
	lsi64854_attach(sc);

	/* Establish interrupt handler */
	if (sa->sa_nintr) {
		sc->sc_intrchain = bppintr;
		sc->sc_intrchainarg = dsc;
		(void)bus_intr_establish(sa->sa_bustag, sa->sa_pri, IPL_TTY,
					 bppintr, sc);
	}

	/* Allocate buffer XXX - should actually use dmamap_uio() */
	dsc->sc_bufsz = 1024;
	dsc->sc_buf = malloc(dsc->sc_bufsz, M_DEVBUF, M_NOWAIT);

	/* XXX read default state */
	{
	bus_space_handle_t h = sc->sc_regs;
	struct hwstate *hw = &dsc->sc_hwdefault;
	int ack_rate = sa->sa_frequency/1000000;

	hw->hw_hcr = bus_space_read_2(sc->sc_bustag, h, L64854_REG_HCR);
	hw->hw_ocr = bus_space_read_2(sc->sc_bustag, h, L64854_REG_OCR);
	hw->hw_tcr = bus_space_read_1(sc->sc_bustag, h, L64854_REG_TCR);
	hw->hw_or = bus_space_read_1(sc->sc_bustag, h, L64854_REG_OR);

	DPRINTF(("bpp: hcr %x ocr %x tcr %x or %x\n",
		 hw->hw_hcr, hw->hw_ocr, hw->hw_tcr, hw->hw_or));
	/* Set these to sane values */
	hw->hw_hcr = ((ack_rate<<BPP_HCR_DSS_SHFT)&BPP_HCR_DSS_MASK)
		| ((ack_rate<<BPP_HCR_DSW_SHFT)&BPP_HCR_DSW_MASK);
	hw->hw_ocr |= BPP_OCR_ACK_OP;
	}
}

void
bpp_setparams(sc, hw)
	struct bpp_softc *sc;
	struct hwstate *hw;
{
	u_int16_t irq;
	bus_space_tag_t t = sc->sc_lsi64854.sc_bustag;
	bus_space_handle_t h = sc->sc_lsi64854.sc_regs;

	bus_space_write_2(t, h, L64854_REG_HCR, hw->hw_hcr);
	bus_space_write_2(t, h, L64854_REG_OCR, hw->hw_ocr);
	bus_space_write_1(t, h, L64854_REG_TCR, hw->hw_tcr);
	bus_space_write_1(t, h, L64854_REG_OR, hw->hw_or);

	/* Only change IRP settings in interrupt status register */
	irq = bus_space_read_2(t, h, L64854_REG_ICR);
	irq &= ~BPP_ALLIRP;
	irq |= (hw->hw_irq & BPP_ALLIRP);
	bus_space_write_2(t, h, L64854_REG_ICR, irq);
	DPRINTF(("bpp_setparams: hcr %x ocr %x tcr %x or %x, irq %x\n",
		 hw->hw_hcr, hw->hw_ocr, hw->hw_tcr, hw->hw_or, irq));
}

int
bppopen(dev, flags, mode, l)
	dev_t dev;
	int flags, mode;
	struct lwp *l;
{
	int unit = BPPUNIT(dev);
	struct bpp_softc *sc;
	struct lsi64854_softc *lsi;
	u_int16_t irq;
	int s;

	if (unit >= bpp_cd.cd_ndevs)
		return (ENXIO);
	sc = bpp_cd.cd_devs[unit];

	if ((sc->sc_flags & (BPP_OPEN|BPP_XCLUDE)) == (BPP_OPEN|BPP_XCLUDE))
		return (EBUSY);

	lsi = &sc->sc_lsi64854;

	/* Set default parameters */
	sc->sc_hwcurrent = sc->sc_hwdefault;
	s = splbpp();
	bpp_setparams(sc, &sc->sc_hwdefault);
	splx(s);

	/* Enable interrupts */
	irq = BPP_ERR_IRQ_EN;
	irq |= sc->sc_hwdefault.hw_irq;
	bus_space_write_2(lsi->sc_bustag, lsi->sc_regs, L64854_REG_ICR, irq);
	return (0);
}

int
bppclose(dev, flags, mode, l)
	dev_t dev;
	int flags, mode;
	struct lwp *l;
{
	struct bpp_softc *sc = bpp_cd.cd_devs[BPPUNIT(dev)];
	struct lsi64854_softc *lsi = &sc->sc_lsi64854;
	u_int16_t irq;

	/* Turn off all interrupt enables */
	irq = sc->sc_hwdefault.hw_irq | BPP_ALLIRQ;
	irq &= ~BPP_ALLEN;
	bus_space_write_2(lsi->sc_bustag, lsi->sc_regs, L64854_REG_ICR, irq);

	sc->sc_asyncproc = NULL;
	sc->sc_flags = 0;
	return (0);
}

int
bppwrite(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{
	struct bpp_softc *sc = bpp_cd.cd_devs[BPPUNIT(dev)];
	struct lsi64854_softc *lsi = &sc->sc_lsi64854;
	int error = 0;
	int s;

	/*
	 * Wait until the DMA engine is free.
	 */
	s = splbpp();
	while ((sc->sc_flags & BPP_LOCKED) != 0) {
		if ((flags & IO_NDELAY) != 0) {
			splx(s);
			return (EWOULDBLOCK);
		}

		sc->sc_flags |= BPP_WANT;
		error = tsleep(sc->sc_buf, PZERO|PCATCH, "bppwrite", 0);
		if (error != 0) {
			splx(s);
			return (error);
		}
	}
	sc->sc_flags |= BPP_LOCKED;
	splx(s);

	/*
	 * Move data from user space into our private buffer
	 * and start DMA.
	 */
	while (uio->uio_resid > 0) {
		void *bp = sc->sc_buf;
		size_t len = min(sc->sc_bufsz, uio->uio_resid);

		if ((error = uiomove(bp, len, uio)) != 0)
			break;

		while (len > 0) {
			u_int8_t tcr;
			size_t size = len;
			DMA_SETUP(lsi, &bp, &len, 0, &size);

#ifdef DEBUG
			if (bppdebug) {
				int i;
				unsigned char *b = bp;
				printf("bpp: writing %ld : ", len);
				for (i=0; i<len; i++) printf("%c(0x%x)", b[i],
				    b[i]);
				printf("\n");
			}
#endif

			/* Clear direction control bit */
			tcr = bus_space_read_1(lsi->sc_bustag, lsi->sc_regs,
						L64854_REG_TCR);
			tcr &= ~BPP_TCR_DIR;
			bus_space_write_1(lsi->sc_bustag, lsi->sc_regs,
					  L64854_REG_TCR, tcr);

			/* Enable DMA */
			s = splbpp();
			DMA_GO(lsi);
			error = tsleep(sc, PZERO|PCATCH, "bppdma", 0);
			splx(s);
			if (error != 0)
				goto out;

			/* Bail out if bottom half reported an error */
			if ((error = sc->sc_error) != 0)
				goto out;

			/*
			 * lsi64854_pp_intr() does this part.
			 *
			 * len -= size;
			 */
		}
	}

out:
	DPRINTF(("bpp done %x\n", error));
	s = splbpp();
	sc->sc_flags &= ~BPP_LOCKED;
	if ((sc->sc_flags & BPP_WANT) != 0) {
		sc->sc_flags &= ~BPP_WANT;
		wakeup(sc->sc_buf);
	}
	splx(s);
	return (error);
}

/* move to header: */
#define BPPIOCSPARAM	_IOW('P', 0x1, struct hwstate)
#define BPPIOCGPARAM	_IOR('P', 0x2, struct hwstate)

int
bppioctl(dev, cmd, data, flag, l)
	dev_t	dev;
	u_long	cmd;
	void *	data;
	int	flag;
	struct	lwp *l;
{
	struct bpp_softc *sc = bpp_cd.cd_devs[BPPUNIT(dev)];
	struct proc *p = l->l_proc;
	struct hwstate *hw, *chw;
	int error = 0;
	int s;

	switch(cmd) {
	case BPPIOCSPARAM:
		chw = &sc->sc_hwcurrent;
		hw = (struct hwstate *)data;

		/*
		 * Extract and store user-settable bits.
		 */
#define _bpp_set(reg,mask) do {		\
	chw->reg &= ~(mask);		\
	chw->reg |= (hw->reg & (mask));	\
} while (0)
		_bpp_set(hw_hcr, BPP_HCR_DSS_MASK|BPP_HCR_DSW_MASK);
		_bpp_set(hw_ocr, BPP_OCR_USER);
		_bpp_set(hw_tcr, BPP_TCR_USER);
		_bpp_set(hw_or,  BPP_OR_USER);
		_bpp_set(hw_irq, BPP_IRQ_USER);
#undef _bpp_set

		/* Apply settings */
		s = splbpp();
		bpp_setparams(sc, chw);
		splx(s);
		break;
	case BPPIOCGPARAM:
		*((struct hwstate *)data) = sc->sc_hwcurrent;
		break;
	case TIOCEXCL:
		s = splbpp();
		sc->sc_flags |= BPP_XCLUDE;
		splx(s);
		break;
	case TIOCNXCL:
		s = splbpp();
		sc->sc_flags &= ~BPP_XCLUDE;
		splx(s);
		break;
	case FIOASYNC:
		s = splbpp();
		if (*(int *)data) {
			if (sc->sc_asyncproc != NULL)
				error = EBUSY;
			else
				sc->sc_asyncproc = p;
		} else
			sc->sc_asyncproc = NULL;
		splx(s);
		break;
	default:
		break;
	}

	return (error);
}

int
bpppoll(dev, events, l)
	dev_t dev;
	int events;
	struct lwp *l;
{
	struct bpp_softc *sc = bpp_cd.cd_devs[BPPUNIT(dev)];
	int revents = 0;

	if (events & (POLLIN | POLLRDNORM)) {
		/* read is not yet implemented */
	}

	if (events & (POLLOUT | POLLWRNORM)) {
		if ((sc->sc_flags & BPP_LOCKED) == 0)
			revents |= (POLLOUT | POLLWRNORM);
	}

	if (revents == 0) {
		if (events & (POLLIN | POLLRDNORM))
			selrecord(l, &sc->sc_rsel);
		if (events & (POLLOUT | POLLWRNORM))
			selrecord(l, &sc->sc_wsel);
	}

	return (revents);
}

static void
filt_bpprdetach(struct knote *kn)
{
	struct bpp_softc *sc = kn->kn_hook;
	int s;

	s = splbpp();
	SLIST_REMOVE(&sc->sc_rsel.sel_klist, kn, knote, kn_selnext);
	splx(s);
}

static int
filt_bppread(struct knote *kn, long hint)
{
	/* XXX Read not yet implemented. */
	return (0);
}

static const struct filterops bppread_filtops =
	{ 1, NULL, filt_bpprdetach, filt_bppread };

static void
filt_bppwdetach(struct knote *kn)
{
	struct bpp_softc *sc = kn->kn_hook;
	int s;

	s = splbpp();
	SLIST_REMOVE(&sc->sc_wsel.sel_klist, kn, knote, kn_selnext);
	splx(s);
}

static int
filt_bpfwrite(struct knote *kn, long hint)
{
	struct bpp_softc *sc = kn->kn_hook;

	if (sc->sc_flags & BPP_LOCKED)
		return (0);

	kn->kn_data = 0;	/* XXXLUKEM (thorpej): what to put here? */
	return (1);
}

static const struct filterops bppwrite_filtops =
	{ 1, NULL, filt_bppwdetach, filt_bpfwrite };

int
bppkqfilter(dev_t dev, struct knote *kn)
{
	struct bpp_softc *sc = bpp_cd.cd_devs[BPPUNIT(dev)];
	struct klist *klist;
	int s;

	switch (kn->kn_filter) {
	case EVFILT_READ:
		klist = &sc->sc_rsel.sel_klist;
		kn->kn_fop = &bppread_filtops;
		break;

	case EVFILT_WRITE:
		klist = &sc->sc_wsel.sel_klist;
		kn->kn_fop = &bppwrite_filtops;
		break;

	default:
		return (EINVAL);
	}

	kn->kn_hook = sc;

	s = splbpp();
	SLIST_INSERT_HEAD(klist, kn, kn_selnext);
	splx(s);

	return (0);
}

int
bppintr(arg)
	void *arg;
{
	struct bpp_softc *sc = arg;
	struct lsi64854_softc *lsi = &sc->sc_lsi64854;
	u_int16_t irq;

	/* First handle any possible DMA interrupts */
	if (lsi64854_pp_intr((void *)lsi) == -1)
		sc->sc_error = 1;

	irq = bus_space_read_2(lsi->sc_bustag, lsi->sc_regs, L64854_REG_ICR);
	/* Ack all interrupts */
	bus_space_write_2(lsi->sc_bustag, lsi->sc_regs, L64854_REG_ICR,
			  irq | BPP_ALLIRQ);

	DPRINTF(("bpp_intr: %x\n", irq));
	/* Did our device interrupt? */
	if ((irq & BPP_ALLIRQ) == 0)
		return (0);

	if ((sc->sc_flags & BPP_LOCKED) != 0)
		wakeup(sc);
	else if ((sc->sc_flags & BPP_WANT) != 0) {
		sc->sc_flags &= ~BPP_WANT;
		wakeup(sc->sc_buf);
	} else {
		selnotify(&sc->sc_wsel, 0, 0);
		if (sc->sc_asyncproc != NULL) {
			mutex_enter(&proclist_mutex);
			psignal(sc->sc_asyncproc, SIGIO);
			mutex_exit(&proclist_mutex);
		}
	}
	return (1);
}
