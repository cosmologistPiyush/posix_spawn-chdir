/*	$NetBSD: clock_pcc.c,v 1.1 1996/04/26 18:59:58 chuck Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Glue for the Peripheral Channel Controller timers and the
 * Mostek clock chip found on the MVME-147.
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <machine/psl.h>
#include <machine/cpu.h>

#include <mvme68k/mvme68k/clockreg.h>
#include <mvme68k/mvme68k/clockvar.h>

#include <mvme68k/dev/pccreg.h>
#include <mvme68k/dev/pccvar.h>

int	clock_pcc_match __P((struct device *, void *, void *));
void	clock_pcc_attach __P((struct device *, struct device *, void *));
int	clock_pcc_intr __P((void *));
void	clock_pcc_initclocks __P((void));

u_char	clock_pcc_lvl;

struct cfattach clock_pcc_ca = {
	sizeof(struct device), clock_pcc_match, clock_pcc_attach
};

int
clock_pcc_match(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct cfdata *cf = match;
	struct pcc_attach_args *pa = aux;
	static int clock_pcc_matched;

	/* Only one clock, please. */
	if (clock_pcc_matched)
		return (0);

	if (strcmp(pa->pa_name, clock_cd.cd_name))
		return (0);

	clock_pcc_matched = 1;
	pa->pa_ipl = cf->pcccf_ipl;

	return (1);
}

void
clock_pcc_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct pcc_attach_args *pa = aux;
	caddr_t nvram, clockregs;

	if (pa->pa_ipl != CLOCK_LEVEL)
		panic("clock_pcc_attach: wrong interrupt level");

	nvram = PCC_VADDR(pa->pa_offset);
	clockregs = PCC_VADDR(pa->pa_offset + PCC_RTC_OFF);

	/* Do common portions of clock config. */
	clock_config(self, clockregs, nvram, MK48T02_SIZE,
	    clock_pcc_initclocks);

	/* Attach the interrupt handler. */
	pccintr_establish(PCCV_TIMER1, clock_pcc_intr, pa->pa_ipl, NULL);
	/* XXX timer2 */
	clock_pcc_lvl = pa->pa_ipl | PCC_IENABLE | PCC_TIMERACK;
}

void
clock_pcc_initclocks()
{

	sys_pcc->t1_pload = PCC_TIMER100HZ;
	sys_pcc->t1_cr = PCC_TIMERCLEAR;
	sys_pcc->t1_cr = PCC_TIMERSTART;
	sys_pcc->t1_int = clock_pcc_lvl;
}

int
clock_pcc_intr(frame)
	void *frame;
{

	sys_pcc->t1_int = clock_pcc_lvl;
	hardclock(frame);
	return (1);
}
