/*	$NetBSD: mcontext.h,v 1.12 2011/02/23 02:58:39 joerg Exp $	*/

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein.
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

#ifndef _AMD64_MCONTEXT_H_
#define _AMD64_MCONTEXT_H_

#ifdef __x86_64__

#include <machine/frame_regs.h>

/*
 * General register state
 */
#define GREG_OFFSETS(reg, REG, idx) _REG_##REG = idx,
enum { _FRAME_GREG(GREG_OFFSETS) _NGREG = 26 };
#undef GREG_OFFSETS

typedef	unsigned long	__greg_t;
typedef	__greg_t	__gregset_t[_NGREG];

/* These names are for compatibility */
#define	_REG_URSP	_REG_RSP
#define	_REG_RFL	_REG_RFLAGS

/*
 * Floating point register state
 */
typedef char __fpregset_t[512];

/*
 * The padding below is to make __fpregs have a 16-byte aligned offset
 * within ucontext_t.
 */

typedef struct {
	__gregset_t	__gregs;
	long 		__pad;
	__fpregset_t	__fpregs;
} mcontext_t;

#define _UC_UCONTEXT_ALIGN	(~0xf)

#define _UC_MACHINE_SP(uc)	((uc)->uc_mcontext.__gregs[_REG_RSP] - 128)
#define _UC_MACHINE_PC(uc)	((uc)->uc_mcontext.__gregs[_REG_RIP])
#define _UC_MACHINE_INTRV(uc)	((uc)->uc_mcontext.__gregs[_REG_RAX])

#define	_UC_MACHINE_SET_PC(uc, pc)	_UC_MACHINE_PC(uc) = (pc)

/*
 * mcontext extensions to handle signal delivery.
 */
#define _UC_SETSTACK	0x00010000
#define _UC_CLRSTACK	0x00020000

#define	__UCONTEXT_SIZE	784

#ifdef _KERNEL

/*
 * 32bit context definitions.
 */

#define _NGREG32	19
typedef unsigned int	__greg32_t;
typedef __greg32_t	__gregset32_t[_NGREG32];

#define _REG32_GS	0
#define _REG32_FS	1
#define _REG32_ES	2
#define _REG32_DS	3
#define _REG32_EDI	4
#define _REG32_ESI	5
#define _REG32_EBP	6
#define _REG32_ESP	7
#define _REG32_EBX	8
#define _REG32_EDX	9
#define _REG32_ECX	10
#define _REG32_EAX	11
#define _REG32_TRAPNO	12
#define _REG32_ERR	13
#define _REG32_EIP	14
#define _REG32_CS	15
#define _REG32_EFL	16
#define _REG32_UESP	17
#define _REG32_SS	18

#define _UC_MACHINE32_SP(uc)	((uc)->uc_mcontext.__gregs[_REG32_UESP])

/*
 * Floating point register state
 */
typedef struct {
	union {
		struct {
			int	__fp_state[27];	/* Environment and registers */
			int	__fp_status;	/* Software status word */
		} __fpchip_state;
		struct {
			char	__fp_emul[246];
			char	__fp_epad[2];
		} __fp_emul_space;
		struct {
			char	__fp_xmm[512];
		} __fp_xmm_state;
		int	__fp_fpregs[128];
	} __fp_reg_set;
	int	__fp_wregs[33];			/* Weitek? */
} __fpregset32_t;

typedef struct {
	__gregset32_t	__gregs;
	__fpregset32_t	__fpregs;
} mcontext32_t;

#define _UC_MACHINE32_PAD	5
#define	__UCONTEXT32_SIZE	776

struct trapframe;
struct lwp;
int check_mcontext(struct lwp *, const mcontext_t *, struct trapframe *);

#endif /* _KERNEL */

#else	/*	__x86_64__	*/

#include <i386/mcontext.h>

#endif	/*	__x86_64__	*/

#endif	/* !_AMD64_MCONTEXT_H_ */
