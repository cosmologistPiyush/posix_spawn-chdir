/*	$NetBSD: intr.h,v 1.8 2008/12/13 21:13:30 ad Exp $	*/

/*-
 * Copyright (c) 2007 The NetBSD Foundation, Inc.
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

#ifndef _SYS_INTR_H_
#define	_SYS_INTR_H_

#include <machine/intr.h>

#ifdef _KERNEL

struct cpu_info;

/* Public interface. */
void	*softint_establish(u_int, void (*)(void *), void *);
void	softint_disestablish(void *);
void	softint_schedule(void *);

/* MI hooks. */
void	softint_init(struct cpu_info *);
lwp_t	*softint_picklwp(void);
void	softint_overlay(void);
void	softint_block(lwp_t *);

/* MD-MI interface. */
void	softint_init_md(lwp_t *, u_int, uintptr_t *);
void	softint_trigger(uintptr_t);
void	softint_dispatch(lwp_t *, int);

/* Flags for softint_establish(). */
#define	SOFTINT_BIO	0x0000
#define	SOFTINT_CLOCK	0x0001
#define	SOFTINT_SERIAL	0x0002
#define	SOFTINT_NET	0x0003
#define	SOFTINT_MPSAFE	0x0100

/* Implementation private flags. */
#define	SOFTINT_PENDING	0x1000
#define	SOFTINT_ACTIVE	0x2000

#define	SOFTINT_COUNT	0x0004
#define	SOFTINT_LVLMASK	0x00ff
#define	SOFTINT_IMPMASK	0xf000

extern u_int	softint_timing;
extern int	safepri;

/*
 * Historical aliases.  XXX Audio devices should run at
 * IPL_SCHED, but they need to acquire kernel_lock.
 */
#define	IPL_BIO		IPL_VM
#define	IPL_NET		IPL_VM
#define	IPL_TTY		IPL_VM
#define	IPL_LPT		IPL_VM
#define	IPL_AUDIO	IPL_VM
#define	IPL_CLOCK	IPL_SCHED
#define	IPL_IPI		IPL_HIGH
#define	IPL_SERIAL	IPL_HIGH

#define	splbio()	splvm()
#define	splnet()	splvm()
#define	spltty()	splvm()
#define	spllpt()	splvm()
#define	splaudio()	splvm()
#define	splclock()	splsched()
#define	splipi()	splhigh()
#define	splserial()	splhigh()

#endif	/* _KERNEL */

#endif	/* _SYS_INTR_H_ */
