/* 	$NetBSD: linux_signal.h,v 1.6 1998/10/01 00:57:30 erh Exp $	*/

/*
 * Copyright (c) 1998 Eric Haszlakiewicz
 * Copyright (c) 1995 Frank van der Linden
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
 *      This product includes software developed for the NetBSD Project
 *      by Frank van der Linden
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

/* Note: Machine dependant portions of this file exist. */

#ifndef _COMMON_LINUX_SIGNAL_H
#define _COMMON_LINUX_SIGNAL_H

#ifdef _KERNEL
extern int native_to_linux_sig[];
extern int linux_to_native_sig[];
__BEGIN_DECLS
void linux_old_to_native_sigset __P((const linux_old_sigset_t *, sigset_t *));
void native_to_linux_old_sigset __P((const sigset_t *, linux_old_sigset_t *));

#if 0
/* XXX Need these if sizeof(linux_old_sigset_t) != sizeof(linux_sigset_t) */
void linux_to_native_sigset __P((const linux_sigset_t *, sigset_t *));
void native_to_linux_sigset __P((const sigset_t *, linux_sigset_t *));
#endif

#define linux_to_native_sigset(x,y) \
			linux_old_to_native_sigset((linux_old_sigset_t *)x, y)
#define native_to_linux_sigset(x,y) \
			native_to_linux_old_sigset(x, (linux_old_sigset_t *)y)

void linux_old_to_native_sigaction __P((struct linux_old_sigaction *, \
							struct sigaction *));
void native_to_linux_old_sigaction __P((struct sigaction *, \
						struct linux_old_sigaction *));

void linux_to_native_sigaction __P((struct linux_sigaction *, \
							struct sigaction *));
void native_to_linux_sigaction __P((struct sigaction *, \
						struct linux_sigaction *));

__END_DECLS
#endif /* !_KERNEL */

#endif /* !_COMMON_LINUX_SIGNAL_H */
