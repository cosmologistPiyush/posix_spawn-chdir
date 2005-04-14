/*	$NetBSD: stdarg.h,v 1.22 2005/04/14 20:06:15 matt Exp $	*/

/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)stdarg.h	8.1 (Berkeley) 6/10/93
 */

#ifndef _M68K_STDARG_H_
#define	_M68K_STDARG_H_

#include <machine/ansi.h>
#include <sys/featuretest.h>

typedef _BSD_VA_LIST_	va_list;

#ifdef __lint__

#define va_start(ap, last)      ((ap) = *(va_list *)0)
#define va_arg(ap, type)        (*(type *)(void *)&(ap))
#define va_end(ap)
#define __va_copy(dest, src)    ((dest) = (src))

#elif __GNUC_PREREQ__(3,0)

#define va_start(ap, last)	__builtin_va_start(ap, last)
#define va_end(ap)		__builtin_va_end(ap)
#define va_arg(ap, type)	__builtin_va_arg(ap, type)
#define __va_copy(dest, src)	__builtin_va_copy(dest, src)

#else

#define	__va_size(type) \
	(((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

#define	va_start(ap, last) \
	((ap) = (va_list)__builtin_next_arg(last))

#define	va_arg(ap, type) \
	(*(type *)(void *)((ap) += __va_size(type),			\
			   (ap) - (sizeof(type) < sizeof(long) &&	\
				   sizeof(type) != __va_size(type) ?	\
				   sizeof(type) : __va_size(type))))

#define	__va_copy(dest, src) \
	((dest) = (src))

#define	va_end(ap)	
#endif

#if !defined(_ANSI_SOURCE) && \
    (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE) || \
     defined(_ISOC99_SOURCE) || (__STDC_VERSION__ - 0) >= 199901L)
#define	va_copy(dest, src)	__va_copy(dest, src)
#endif

#endif /* !_M68K_STDARG_H_ */
