/*
 * Copyright (c) 1995
 *	The Regents of the University of California.  All rights reserved.
 *
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * ttcompat.c -- convert sgtty flags to termios
 *	originally from /sys/kern/tty_compat.c
 */

#include <sys/param.h>
#include <sys/types.h>

#include <unistd.h>
#include <sys/ioctl_compat.h>
#include <termios.h>
#include <syslog.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "extern.h"

/* Macros to clear/set/test flags. */
#define	SET(t, f)	(t) |= (f)
#define	CLR(t, f)	(t) &= ~(f)
#define	ISSET(t, f)	((t) & (f))

void
sttyclearflags(tp, flags)
struct termios *tp;
long flags;
{
	register long iflag = tp->c_iflag;
	register long oflag = tp->c_oflag;
	register long lflag = tp->c_lflag;
	register long cflag = tp->c_cflag;

	if (ISSET(flags, TANDEM))
		CLR(iflag, IXOFF);
	if (ISSET(flags, ECHO))
		CLR(lflag, ECHO);
	if (ISSET(flags, CRMOD)) {
		CLR(iflag, ICRNL);
		CLR(oflag, ONLCR);
	}
	if (ISSET(flags, XTABS))
		CLR(oflag, OXTABS);


	if (ISSET(flags, RAW)) {
		SET(iflag, BRKINT|IXON|IMAXBEL);
		SET(lflag, ISIG|IEXTEN);
		if (ISSET(flags, CBREAK))
			CLR(lflag, ICANON);
		else
			SET(lflag, ICANON);
	}

	switch (ISSET(flags, ANYP)) {
	case EVENP:
		SET(iflag, INPCK);
		SET(cflag, PARODD);
		break;
	case ODDP:
		SET(iflag, INPCK);
		CLR(cflag, PARODD);
		break;
	}

	if (ISSET(flags, RAW|LITOUT|PASS8)) {
		CLR(cflag, CSIZE);
		SET(cflag, CS7|PARENB);
		SET(iflag, ISTRIP);
		SET(oflag, OPOST);
	}

	tp->c_iflag = iflag;
	tp->c_oflag = oflag;
	tp->c_lflag = lflag;
	tp->c_cflag = cflag;
}

void
sttysetflags(tp, flags)
struct termios *tp;
long flags;
{
	register long iflag = tp->c_iflag;
	register long oflag = tp->c_oflag;
	register long lflag = tp->c_lflag;
	register long cflag = tp->c_cflag;

	if (ISSET(flags, TANDEM))
		SET(iflag, IXOFF);
	if (ISSET(flags, ECHO))
		SET(lflag, ECHO);
	if (ISSET(flags, CRMOD)) {
		SET(iflag, ICRNL);
		SET(oflag, ONLCR);
	}
	if (ISSET(flags, XTABS))
		SET(oflag, OXTABS);

	if (ISSET(flags, RAW)) {
		CLR(iflag, XOFF);
		CLR(lflag, ISIG|ICANON|IEXTEN);
	}

	switch (ISSET(flags, ANYP)) {
	case EVENP:
		SET(iflag, INPCK);
		CLR(cflag, PARODD);
		break;
	case ODDP:
		SET(iflag, INPCK);
		SET(cflag, PARODD);
		break;
	}

	if (ISSET(flags, RAW|LITOUT|PASS8)) {
		CLR(cflag, CSIZE|PARENB);
		SET(cflag, CS8);
		if (!ISSET(flags, RAW|PASS8))
			SET(iflag, ISTRIP);
		else
			CLR(iflag, ISTRIP);
		if (!ISSET(flags, RAW|LITOUT))
			SET(oflag, OPOST);
		else
			CLR(oflag, OPOST);
	}

	tp->c_iflag = iflag;
	tp->c_oflag = oflag;
	tp->c_lflag = lflag;
	tp->c_cflag = cflag;
}

void
sttyclearlflags(tp, flags)
struct termios *tp;
long flags;
{
	register long iflag = tp->c_iflag;
	register long oflag = tp->c_oflag;
	register long lflag = tp->c_lflag;
	register long cflag = tp->c_cflag;

	/* Nothing we can do with CRTBS. */
	if (ISSET(flags, PRTERA))
		CLR(lflag, ECHOPRT);
	if (ISSET(flags, CRTERA))
		CLR(lflag, ECHOE);
	/* Nothing we can do with TILDE. */
	if (ISSET(flags, MDMBUF))
		CLR(cflag, MDMBUF);
	if (ISSET(flags, NOHANG))
		SET(cflag, HUPCL);
	if (ISSET(flags, CRTKIL))
		CLR(lflag, ECHOKE);
	if (ISSET(flags, CTLECH))
		CLR(lflag, ECHOCTL);
	if (!ISSET(flags, DECCTQ))
		CLR(iflag, IXANY);
	CLR(lflag, ISSET(flags, TOSTOP|FLUSHO|PENDIN|NOFLSH));

	if (ISSET(flags, RAW|LITOUT|PASS8)) {
		CLR(cflag, CSIZE);
		SET(cflag, CS7|PARENB);
		SET(iflag, ISTRIP);
		SET(oflag, OPOST);
	}

	tp->c_iflag = iflag;
	tp->c_oflag = oflag;
	tp->c_lflag = lflag;
	tp->c_cflag = cflag;
}

void
sttysetlflags(tp, flags)
struct termios *tp;
long flags;
{
	register long iflag = tp->c_iflag;
	register long oflag = tp->c_oflag;
	register long lflag = tp->c_lflag;
	register long cflag = tp->c_cflag;

	/* Nothing we can do with CRTBS. */
	if (ISSET(flags, PRTERA))
		SET(lflag, ECHOPRT);
	if (ISSET(flags, CRTERA))
		SET(lflag, ECHOE);
	/* Nothing we can do with TILDE. */
	if (ISSET(flags, MDMBUF))
		SET(cflag, MDMBUF);
	if (ISSET(flags, NOHANG))
		CLR(cflag, HUPCL);
	if (ISSET(flags, CRTKIL))
		SET(lflag, ECHOKE);
	if (ISSET(flags, CTLECH))
		SET(lflag, ECHOCTL);
	if (!ISSET(flags, DECCTQ))
		SET(iflag, IXANY);
	CLR(lflag, TOSTOP|FLUSHO|PENDIN|NOFLSH);
	SET(lflag, ISSET(flags, TOSTOP|FLUSHO|PENDIN|NOFLSH));

	if (ISSET(flags, RAW|LITOUT|PASS8)) {
		CLR(cflag, CSIZE|PARENB);
		SET(cflag, CS8);
		if (!ISSET(flags, RAW|PASS8))
			SET(iflag, ISTRIP);
		else
			CLR(iflag, ISTRIP);
		if (!ISSET(flags, RAW|LITOUT))
			SET(oflag, OPOST);
		else
			CLR(oflag, OPOST);
	}

	tp->c_iflag = iflag;
	tp->c_oflag = oflag;
	tp->c_lflag = lflag;
	tp->c_cflag = cflag;
}
