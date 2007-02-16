/*	$NetBSD: userret.h,v 1.3 2007/02/16 02:17:42 ad Exp $	*/

/*
 * Copyright (c) 1994 Ludd, University of Lule}, Sweden.
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
 *     This product includes software developed at Ludd, University of Lule}.
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

static __inline void
userret(struct lwp *, struct trapframe *, u_quad_t);

/*
 *	Common code used by various execption handlers to
 *	return to usermode.
 */
static __inline void
userret(struct lwp *l, struct trapframe *frame, u_quad_t oticks)
{
	struct proc *p = l->l_proc;

	/* Take pending signals. */
	for (;;) {
		if ((l->l_flag & L_USERRET) != 0)
			lwp_userret(l);
		if (!curcpu()->ci_want_resched)
			break;
		preempt();
	}

	/*
	 * If profiling, charge system time to the trapped pc.
	 */
	if ((p->p_stflag & PST_PROFIL) != 0) {
		extern int psratio;

		addupc_task(l, frame->pc,
		    (int)(p->p_sticks - oticks) * psratio);
	}

	curcpu()->ci_schedstate.spc_curpriority = l->l_priority;
}
