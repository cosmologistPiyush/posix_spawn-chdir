/* $NetBSD: siotty.c,v 1.23 2008/06/13 09:58:06 cegger Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Tohru Nishimura.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: siotty.c,v 1.23 2008/06/13 09:58:06 cegger Exp $");

#include "opt_ddb.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/conf.h>
#include <sys/ioctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/tty.h>
#include <sys/uio.h>
#include <sys/callout.h>
#include <sys/fcntl.h>
#include <dev/cons.h>
#include <sys/kauth.h>

#include <machine/cpu.h>

#include <luna68k/dev/sioreg.h>
#include <luna68k/dev/siovar.h>

#define	TIOCM_BREAK 01000 /* non standard use */

static const u_int8_t ch0_regs[6] = {
	WR0_RSTINT,				/* reset E/S interrupt */
	WR1_RXALLS | WR1_TXENBL,	 	/* Rx per char, Tx */
	0,					/* */
	WR3_RX8BIT | WR3_RXENBL,		/* Rx */
	WR4_BAUD96 | WR4_STOP1,			/* Tx/Rx */
	WR5_TX8BIT | WR5_TXENBL | WR5_DTR | WR5_RTS, /* Tx */
};

static const struct speedtab siospeedtab[] = {
	{ 2400,	WR4_BAUD24, },
	{ 4800,	WR4_BAUD48, },
	{ 9600,	WR4_BAUD96, },
	{ -1,	0, },
};

struct siotty_softc {
	struct device	sc_dev;
	struct tty	*sc_tty;
	struct sioreg	*sc_ctl;
	u_int 		sc_flags;
	u_int8_t	sc_wr[6];
};

#include "siotty.h"
static void siostart __P((struct tty *));
static int  sioparam __P((struct tty *, struct termios *));
static void siottyintr __P((int));
static int  siomctl __P((struct siotty_softc *, int, int));

static int  siotty_match __P((struct device *, struct cfdata *, void *));
static void siotty_attach __P((struct device *, struct device *, void *));

CFATTACH_DECL(siotty, sizeof(struct siotty_softc),
    siotty_match, siotty_attach, NULL, NULL);
extern struct cfdriver siotty_cd;

dev_type_open(sioopen);
dev_type_close(sioclose);
dev_type_read(sioread);
dev_type_write(siowrite);
dev_type_ioctl(sioioctl);
dev_type_stop(siostop);
dev_type_tty(siotty);
dev_type_poll(siopoll);

const struct cdevsw siotty_cdevsw = {
	sioopen, sioclose, sioread, siowrite, sioioctl,
	siostop, siotty, siopoll, nommap, ttykqfilter, D_TTY
};

static int 
siotty_match(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void   *aux;
{
	struct sio_attach_args *args = aux;

	if (args->channel != 0) /* XXX allow tty on Ch.B XXX */
		return 0;
	return 1;
}

static void 
siotty_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct sio_softc *scp = (void *)parent;
	struct siotty_softc *sc = (void *)self;
	struct sio_attach_args *args = aux;

	sc->sc_ctl = (struct sioreg *)scp->scp_ctl + args->channel;
	bcopy(ch0_regs, sc->sc_wr, sizeof(ch0_regs));
	scp->scp_intr[args->channel] = siottyintr;

	if (args->hwflags == 1) {
		printf(" (console)");
		sc->sc_flags = TIOCFLAG_SOFTCAR;
	}
	else {
		setsioreg(sc->sc_ctl, WR0, WR0_CHANRST);
		setsioreg(sc->sc_ctl, WR2A, WR2_VEC86 | WR2_INTR_1);
		setsioreg(sc->sc_ctl, WR2B, 0);
		setsioreg(sc->sc_ctl, WR0, sc->sc_wr[WR0]);
		setsioreg(sc->sc_ctl, WR4, sc->sc_wr[WR4]);
		setsioreg(sc->sc_ctl, WR3, sc->sc_wr[WR3]);
		setsioreg(sc->sc_ctl, WR5, sc->sc_wr[WR5]);
		setsioreg(sc->sc_ctl, WR0, sc->sc_wr[WR0]);
	}
	setsioreg(sc->sc_ctl, WR1, sc->sc_wr[WR1]); /* now interrupt driven */

	printf("\n");
}

/*--------------------  low level routine --------------------*/

static void
siottyintr(int chan)
{
	struct siotty_softc *sc;
	struct sioreg *sio;
	struct tty *tp;
	unsigned int code;
	int rr;

	sc = device_lookup_private(&siotty_cd, chan);
	if (sc == NULL)
		return;

	tp = sc->sc_tty;
	sio = sc->sc_ctl;
	rr = getsiocsr(sio);
	if (rr & RR_RXRDY) {
		do {
			code = sio->sio_data;
			if (rr & (RR_FRAMING | RR_OVERRUN | RR_PARITY)) {
				sio->sio_cmd = WR0_ERRRST;
				if (sio->sio_stat & RR_FRAMING)
					code |= TTY_FE;
				else if (sio->sio_stat & RR_PARITY)
					code |= TTY_PE;
			}
			if (tp == NULL || (tp->t_state & TS_ISOPEN) == 0)
				continue;
#if 0 && defined(DDB) /* ?!?! fails to resume ?!?! */
			if ((rr & RR_BREAK) && tp->t_dev == cn_tab->cn_dev) {
				cpu_Debugger();
				return;
			}
#endif
			(*tp->t_linesw->l_rint)(code, tp);
		} while ((rr = getsiocsr(sio)) & RR_RXRDY);
	}
	if (rr & RR_TXRDY) {
		sio->sio_cmd = WR0_RSTPEND;
		if (tp != NULL) {
			tp->t_state &= ~(TS_BUSY|TS_FLUSH);
			(*tp->t_linesw->l_start)(tp);
		}
	}
}

static void
siostart(struct tty *tp)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(tp->t_dev));
	int s, c;
 
	s = spltty();
	if (tp->t_state & (TS_BUSY|TS_TIMEOUT|TS_TTSTOP))
		goto out;
	if (!ttypull(tp))
		goto out;
	tp->t_state |= TS_BUSY;
	while (getsiocsr(sc->sc_ctl) & RR_TXRDY) {
		if ((c = getc(&tp->t_outq)) == -1)
			break;
		sc->sc_ctl->sio_data = c;
	}
out:
	splx(s);
}

void
siostop(tp, flag)
	struct tty *tp;
	int flag;
{
	int s;

        s = spltty();
        if (TS_BUSY == (tp->t_state & (TS_BUSY|TS_TTSTOP))) {
                /*
                 * Device is transmitting; must stop it.
                 */
		tp->t_state |= TS_FLUSH;
        }
        splx(s);
}

static int
sioparam(struct tty *tp, struct termios *t)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(tp->t_dev));
	int wr4, s;

	if (t->c_ispeed && t->c_ispeed != t->c_ospeed)
		return EINVAL;
	wr4 = ttspeedtab(t->c_ospeed, siospeedtab);
	if (wr4 < 0)
		return EINVAL;

	if (sc->sc_flags & TIOCFLAG_SOFTCAR) {
		t->c_cflag |= CLOCAL;
		t->c_cflag &= ~HUPCL;
	}
	if (sc->sc_flags & TIOCFLAG_CLOCAL)
		t->c_cflag |= CLOCAL;

	/*
	 * If there were no changes, don't do anything.  This avoids dropping
	 * input and improves performance when all we did was frob things like
	 * VMIN and VTIME.
	 */
	if (tp->t_ospeed == t->c_ospeed && tp->t_cflag == t->c_cflag)
		return 0;

	tp->t_ispeed = t->c_ispeed;
	tp->t_ospeed = t->c_ospeed;
	tp->t_cflag = t->c_cflag;

	sc->sc_wr[WR3] &= 0x3f;
	sc->sc_wr[WR5] &= 0x9f;
	switch (tp->t_cflag & CSIZE) {
	case CS7:
		sc->sc_wr[WR3] |= WR3_RX7BIT; sc->sc_wr[WR5] |= WR5_TX7BIT;
		break;
	case CS8:
		sc->sc_wr[WR3] |= WR3_RX8BIT; sc->sc_wr[WR5] |= WR5_TX8BIT;
		break;
	}
	if (tp->t_cflag & PARENB) {
		wr4 |= WR4_PARENAB;
		if ((tp->t_cflag & PARODD) == 0)
			wr4 |= WR4_EPARITY;
	}
	wr4 |= (tp->t_cflag & CSTOPB) ? WR4_STOP2 : WR4_STOP1;	
	sc->sc_wr[WR4] = wr4;

	s = spltty();
	setsioreg(sc->sc_ctl, WR4, sc->sc_wr[WR4]);
	setsioreg(sc->sc_ctl, WR3, sc->sc_wr[WR3]);
	setsioreg(sc->sc_ctl, WR5, sc->sc_wr[WR5]);
	splx(s);

	return 0;
}

static int
siomctl(sc, control, op)
	struct siotty_softc *sc;
	int control, op;
{
	int val, s, wr5, rr;

	val = 0;
	if (control & TIOCM_BREAK)
		val |= WR5_BREAK;
	if (control & TIOCM_DTR)
		val |= WR5_DTR;
	if (control & TIOCM_RTS)
		val |= WR5_RTS;
	s = spltty();
	wr5 = sc->sc_wr[WR5];
	switch (op) {
	case DMSET:
		wr5 &= ~(WR5_BREAK|WR5_DTR|WR5_RTS);
		/* FALLTHRU */
	case DMBIS:
		wr5 |= val;
		break;
	case DMBIC:
		wr5 &= ~val;
		break;
	case DMGET:
		val = 0;
		rr = getsiocsr(sc->sc_ctl);
		if (wr5 & WR5_DTR)
			val |= TIOCM_DTR;
		if (wr5 & WR5_RTS)
			val |= TIOCM_RTS;
		if (rr & RR_CTS)
			val |= TIOCM_CTS;
		if (rr & RR_DCD)
			val |= TIOCM_CD;
		goto done;
	}
	sc->sc_wr[WR5] = wr5;
	setsioreg(sc->sc_ctl, WR5, wr5);
	val = 0;
  done:
	splx(s);
	return val;
}

/*--------------------  cdevsw[] interface --------------------*/

int
sioopen(dev_t dev, int flag, int mode, struct lwp *l)
{
	struct siotty_softc *sc;
	struct tty *tp;
	int error;

	sc = device_lookup_private(&siotty_cd, minor(dev));
	if (sc == NULL)
		return ENXIO;
	if ((tp = sc->sc_tty) == NULL) {
		tp = sc->sc_tty = ttymalloc();
		tty_attach(tp);
	}		

	tp->t_oproc = siostart;
	tp->t_param = sioparam;
	tp->t_hwiflow = NULL /* XXX siohwiflow XXX */;
	tp->t_dev = dev;

	if (kauth_authorize_device_tty(l->l_cred, KAUTH_DEVICE_TTY_OPEN, tp))
		return (EBUSY);

	if ((tp->t_state & TS_ISOPEN) == 0 && tp->t_wopen == 0) {
		struct termios t;

		t.c_ispeed = t.c_ospeed = TTYDEF_SPEED;
		t.c_cflag = TTYDEF_CFLAG;
		tp->t_ospeed = 0; /* force register update */
		(void)sioparam(tp, &t);
		tp->t_iflag = TTYDEF_IFLAG;
		tp->t_oflag = TTYDEF_OFLAG;
		tp->t_lflag = TTYDEF_LFLAG;
		ttychars(tp);
		ttsetwater(tp);
		/* raise RTS and DTR here; but, DTR lead is not wired */
		/* then check DCD condition; but, DCD lead is not wired */
		tp->t_state |= TS_CARR_ON; /* assume detected all the time */
#if 0
		if ((sc->sc_flags & TIOCFLAG_SOFTCAR)
		    || (tp->t_cflag & MDMBUF)
		    || (getsiocsr(sc->sc_ctl) & RR_DCD))
			tp->t_state |= TS_CARR_ON;
		else
			tp->t_state &= ~TS_CARR_ON;
#endif
	}

	error = ttyopen(tp, 0, (flag & O_NONBLOCK));
	if (error > 0)
		return error;
	return (*tp->t_linesw->l_open)(dev, tp);
}
 
int
sioclose(dev_t dev, int flag, int mode, struct lwp *l)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(dev));
	struct tty *tp = sc->sc_tty;
	int s;

	(*tp->t_linesw->l_close)(tp, flag);

	s = spltty();
	siomctl(sc, TIOCM_BREAK, DMBIC);
#if 0 /* because unable to feed DTR signal */
	if ((tp->t_cflag & HUPCL)
	    || tp->t_wopen || (tp->t_state & TS_ISOPEN) == 0) {
		siomctl(sc, TIOCM_DTR, DMBIC);
		/* Yield CPU time to others for 1 second, then ... */
		siomctl(sc, TIOCM_DTR, DMBIS);
	}
#endif
	splx(s);
	return ttyclose(tp);
}
 
int
sioread(dev_t dev, struct uio *uio, int flag)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(dev));
	struct tty *tp = sc->sc_tty;
 
	return (*tp->t_linesw->l_read)(tp, uio, flag);
}
 
int
siowrite(dev_t dev, struct uio *uio, int flag)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(dev));
	struct tty *tp = sc->sc_tty;
 
	return (*tp->t_linesw->l_write)(tp, uio, flag);
}

int
siopoll(dev_t dev, int events, struct lwp *l)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(dev));
	struct tty *tp = sc->sc_tty;
 
	return ((*tp->t_linesw->l_poll)(tp, events, l));
}

int
sioioctl(dev_t dev, u_long cmd, void *data, int flag, struct lwp *l)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(dev));
	struct tty *tp = sc->sc_tty;
	int error;

	error = (*tp->t_linesw->l_ioctl)(tp, cmd, data, flag, l);
	if (error != EPASSTHROUGH)
		return error;

	error = ttioctl(tp, cmd, data, flag, l);
	if (error != EPASSTHROUGH)
		return error;

	/* the last resort for TIOC ioctl tranversing */
	switch (cmd) {
	case TIOCSBRK: /* Set the hardware into BREAK condition */
		siomctl(sc, TIOCM_BREAK, DMBIS);
		break;
	case TIOCCBRK: /* Clear the hardware BREAK condition */
		siomctl(sc, TIOCM_BREAK, DMBIC);
		break;
	case TIOCSDTR: /* Assert DTR signal */
		siomctl(sc, TIOCM_DTR|TIOCM_RTS, DMBIS);
		break;
	case TIOCCDTR: /* Clear DTR signal */
		siomctl(sc, TIOCM_DTR|TIOCM_RTS, DMBIC);
		break;
	case TIOCMSET: /* Set modem state replacing current one */
		siomctl(sc, *(int *)data, DMSET);
		break;
	case TIOCMGET: /* Return current modem state */
		*(int *)data = siomctl(sc, 0, DMGET);
		break;
	case TIOCMBIS: /* Set individual bits of modem state */
		siomctl(sc, *(int *)data, DMBIS);
		break;
	case TIOCMBIC: /* Clear individual bits of modem state */
		siomctl(sc, *(int *)data, DMBIC);
		break;
	case TIOCSFLAGS: /* Instruct how serial port behaves */
		sc->sc_flags = *(int *)data;
		break;
	case TIOCGFLAGS: /* Return current serial port state */
		*(int *)data = sc->sc_flags;
		break;
	default:
		return EPASSTHROUGH;
	}
	return 0;
}

/* ARSGUSED */
struct tty *
siotty(dev_t dev)
{
	struct siotty_softc *sc = device_lookup_private(&siotty_cd,minor(dev));
 
	return sc->sc_tty;
}

/*--------------------  miscelleneous routine --------------------*/

/* EXPORT */ void
setsioreg(struct sioreg *sio, int regno, int val)
{
	if (regno != 0)
		sio->sio_cmd = regno;	/* DELAY(); */
	sio->sio_cmd = val;		/* DELAY(); */
}

/* EXPORT */ int
getsiocsr(struct sioreg *sio)
{
	int val;

	val = sio->sio_stat << 8;	/* DELAY(); */
	sio->sio_cmd = 1;		/* DELAY(); */
	val |= sio->sio_stat;		/* DELAY(); */
	return val;
}

/*---------------------  console interface ----------------------*/

void syscnattach __P((int));
int  syscngetc __P((dev_t));
void syscnputc __P((dev_t, int));

struct consdev syscons = {
	NULL,
	NULL,
	syscngetc,
	syscnputc,
	nullcnpollc,
	NULL,
	NULL,
	NULL,
	NODEV,
	CN_REMOTE,
};

/* EXPORT */ void
syscnattach(channel)
	int channel;
{
/*
 * Channel A is immediately initialized with 9600N1 right after cold
 * boot/reset/poweron.  ROM monitor emits one line message on CH.A.
 */
	struct sioreg *sio;
	sio = (struct sioreg *)0x51000000 + channel;

	syscons.cn_dev = makedev(cdevsw_lookup_major(&siotty_cdevsw),
				 channel);
	cn_tab = &syscons;

	setsioreg(sio, WR0, WR0_CHANRST);
	setsioreg(sio, WR2A, WR2_VEC86 | WR2_INTR_1);
	setsioreg(sio, WR2B, 0);
	setsioreg(sio, WR0, ch0_regs[WR0]);
	setsioreg(sio, WR4, ch0_regs[WR4]);
	setsioreg(sio, WR3, ch0_regs[WR3]);
	setsioreg(sio, WR5, ch0_regs[WR5]);
	setsioreg(sio, WR0, ch0_regs[WR0]);
}

/* EXPORT */ int
syscngetc(dev)
	dev_t dev;
{
	struct sioreg *sio;
	int s, c;

	sio = (struct sioreg *)0x51000000 + ((int)dev & 0x1);
	s = splhigh();
	while ((getsiocsr(sio) & RR_RXRDY) == 0)
		;
	c = sio->sio_data;
	splx(s);

	return c;
}

/* EXPORT */ void
syscnputc(dev, c)
	dev_t dev;
	int c;
{
	struct sioreg *sio;
	int s;

	sio = (struct sioreg *)0x51000000 + ((int)dev & 0x1);
	s = splhigh();
	while ((getsiocsr(sio) & RR_TXRDY) == 0)
		;
	sio->sio_cmd = WR0_RSTPEND;
	sio->sio_data = c;
	splx(s);
}
