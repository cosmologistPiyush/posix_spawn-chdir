/*	$NetBSD: threadlib.h,v 1.1 2003/01/19 19:24:39 thorpej Exp $	*/

/*-
 * Copyright (c) 1997, 1998, 2003 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by J.T. Conklin, by Nathan J. Williams, and by Jason R. Thorpe.
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

#ifndef _THREADLIB_H_
#define	_THREADLIB_H_

/*
 * Abtract thread interface for thread-safe libraries.  These routines
 * will use stubs in libc if the application is not linked against the
 * pthread library, and the real function in the pthread library if it
 * is.
 */

#include <pthread.h>
#include <signal.h>

#define	mutex_t			pthread_mutex_t
#define	MUTEX_INITIALIZER	PTHREAD_MUTEX_INITIALIZER

#define	mutexattr_t		pthread_mutexattr_t

#define	cond_t			pthread_cond_t
#define	COND_INITIALIZER	PTHREAD_COND_INITIALIZER

#define	condattr_t		pthread_condattr_t

#define	rwlock_t		pthread_rwlock_t
#define	RWLOCK_INITIALIZER	PTHREAD_RWLOCK_INITIALIZER

#define	rwlockattr_t		pthread_rwlockattr_t

#define	thread_key_t		pthread_key_t

#define	thr_t			pthread_t

#define	once_t			pthread_once_t
#define	ONCE_INITIALIZER	PTHREAD_ONCE_INIT

#ifndef __LIBC_THREAD_STUBS

int	__libc_mutex_init(mutex_t *, const mutexattr_t *);
int	__libc_mutex_lock(mutex_t *);
int	__libc_mutex_trylock(mutex_t *);
int	__libc_mutex_unlock(mutex_t *);
int	__libc_mutex_destroy(mutex_t *);

#define	mutex_init(m, a)	__libc_mutex_init((m), (a))
#define	mutex_lock(m)		__libc_mutex_lock((m))
#define	mutex_trylock(m)	__libc_mutex_trylock((m))
#define	mutex_unlock(m)		__libc_mutex_unlock((m))
#define	mutex_destroy(m)	__libc_mutex_destroy((m))

int	__libc_cond_init(cond_t *, const condattr_t *);
int	__libc_cond_signal(cond_t *);
int	__libc_cond_broadcast(cond_t *);
int	__libc_cond_wait(cond_t *, mutex_t *);
int	__libc_cond_timedwait(cond_t *, mutex_t *, const struct timespec *);
int	__libc_cond_destroy(cond_t *);

#define	cond_init(c, t, a)     	__libc_cond_init((c), (a))
#define	cond_signal(c)		__libc_cond_signal((c))
#define	cond_broadcast(c)	__libc_cond_broadcast((c))
#define	cond_wait(c, m)		__libc_cond_wait((c), (m))
#define	cond_timedwait(c, m, t)	__libc_cond_timedwait((c), (m), (t))
#define	cond_destroy(c)		__libc_cond_destroy((c))

int	__libc_rwlock_init(rwlock_t *, const rwlockattr_t *);
int	__libc_rwlock_rdlock(rwlock_t *);
int	__libc_rwlock_wrlock(rwlock_t *);
int	__libc_rwlock_tryrdlock(rwlock_t *);
int	__libc_rwlock_trywrlock(rwlock_t *);
int	__libc_rwlock_unlock(rwlock_t *);
int	__libc_rwlock_destroy(rwlock_t *);

#define	rwlock_init(l, a)	__libc_rwlock_init((l), (a))
#define	rwlock_rdlock(l)	__libc_rwlock_rdlock((l))
#define	rwlock_wrlock(l)	__libc_rwlock_wrlock((l))
#define	rwlock_tryrdlock(l)	__libc_rwlock_tryrdlock((l))
#define	rwlock_trywrlock(l)	__libc_rwlock_trywrlock((l))
#define	rwlock_unlock(l)	__libc_rwlock_unlock((l))
#define	rwlock_destroy(l)	__libc_rwlock_destroy((l))

int	__libc_thr_keycreate(thread_key_t *, void (*)(void *));
int	__libc_thr_setspecific(thread_key_t, const void *);
void	*__libc_thr_getspecific(thread_key_t);
int	__libc_thr_keydelete(thread_key_t);

#define	thr_keycreate(k, d)	__libc_thr_keycreate((k), (d))
#define	thr_setspecific(k, p)	__libc_thr_setspecific((k), (p))
#define	thr_getspecific(k)	__libc_thr_getspecific((k))
#define	thr_keydelete(k)	__libc_thr_keydelete((k))

int	__libc_thr_once(once_t *, void (*)(void));
int	__libc_thr_sigsetmask(int, const sigset_t *, sigset_t *);
thr_t	__libc_thr_self(void);
int	*__libc_thr_errno(void);

#define	thr_once(o, f)		__libc_thr_once((o), (f))
#define	thr_sigsetmask(f, n, o)	__libc_thr_sigsetmask((f), (n), (o))
#define	thr_self()		__libc_thr_self()
#define	thr_errno()		__libc_thr_errno()

#endif /* __LIBC_THREAD_STUBS */

#endif /* _THREADLIB_H_ */
