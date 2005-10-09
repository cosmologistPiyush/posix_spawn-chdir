/*	$NetBSD: sched.c,v 1.4 2005/10/09 11:17:28 kleink Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Nathan J. Williams.
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
__RCSID("$NetBSD: sched.c,v 1.4 2005/10/09 11:17:28 kleink Exp $");

#include <errno.h>
#include <sched.h>

int	pthread__sched_binder;

/* ARGSUSED */
int
sched_setparam(pid_t pid, const struct sched_param *param)
{

	errno = ENOSYS;
	return -1;
}

/* ARGSUSED */
int
sched_getparam(pid_t pid, struct sched_param *param)
{

	errno = ENOSYS;
	return -1;
}

/* ARGSUSED */
int
sched_setscheduler(pid_t pid, int policy,
    const struct sched_param *param)
{

	errno = ENOSYS;
	return -1;
}

/* ARGSUSED */
int
sched_getscheduler(pid_t pid)
{

	errno = ENOSYS;
	return -1;
}

/* ARGSUSED */
int
sched_get_priority_max(int policy)
{

	errno = ENOSYS;
	return -1;
}

/* ARGSUSED */
int
sched_get_priority_min(int policy)
{

	errno = ENOSYS;
	return -1;
}

/* ARGSUSED */
int
sched_rr_get_interval(pid_t pid, struct timespec *interval)
{

	errno = ENOSYS;
	return -1;
}
