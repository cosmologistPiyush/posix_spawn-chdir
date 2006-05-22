/*	$NetBSD: proc.h,v 1.28 2006/05/22 13:44:53 yamt Exp $	*/

/*
 * Copyright (c) 1991 Regents of the University of California.
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
 * 3. Neither the name of the University nor the names of its contributors
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
 *
 *	@(#)proc.h	7.1 (Berkeley) 5/15/91
 */

#ifndef _I386_PROC_H_
#define _I386_PROC_H_

#ifdef _KERNEL_OPT
#include "opt_noredzone.h"
#endif

#include <sys/user.h> /* for sizeof(struct user) */
#include <machine/frame.h>

/*
 * Machine-dependent part of the proc structure for i386.
 */
struct mdlwp {
	struct	trapframe *md_regs;	/* registers on current frame */
	int	md_flags;		/* machine-dependent flags */
	int	md_tss_sel;		/* TSS selector */
};

/* md_flags */
#define	MDL_USEDFPU	0x0001	/* has used the FPU */

struct mdproc {
	int	md_flags;
	void	(*md_syscall)(struct trapframe *);
					/* Syscall handling function */
	volatile int md_astpending;	/* AST pending for this process */
};

/* md_flags */
#define MDP_USEDMTRR	0x0002	/* has set volatile MTRRs */

/* kernel stack params */
#define	UAREA_USER_OFFSET	(USPACE - ALIGN(sizeof(struct user)))
#define	KSTACK_LOWEST_ADDR(l)	((ccaddr_t)USER_TO_UAREA((l)->l_addr))
#define	KSTACK_SIZE		UAREA_USER_OFFSET

#endif /* _I386_PROC_H_ */
