/*	$NetBSD: bswap.h,v 1.1 2010/01/05 15:45:26 tsutsui Exp $	*/

/*-
 * Copyright (c) 2009 Izumi Tsutsui.  All rights reserved.
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

#include <sys/types.h>

#if HAVE_NBTOOL_CONFIG_H
#ifndef BYTE_ORDER
#ifdef WORDS_BIGENDIAN
#define BYTE_ORDER		BIG_ENDIAN
#else
#define BYTE_ORDER		LITTLE_ENDIAN
#endif
#endif
#endif

#ifndef TARGET_BYTE_ORDER
#define TARGET_BYTE_ORDER	BYTE_ORDER
#endif

#if TARGET_BYTE_ORDER != BYTE_ORDER
#define htotarget16(x)		bswap16(x)
#define target16toh(x)		bswap16(x)
#define htotarget32(x)		bswap32(x)
#define target32toh(x)		bswap32(x)
#define dkcksum_target(lp)	dkcksum_re(lp)

void htotargetlabel(struct disklabel *, struct disklabel *);
void targettohlabel(struct disklabel *, struct disklabel *);
uint16_t dkcksum_re(struct disklabel *);

#else
#define htotarget16(x)		(x)
#define target16toh(x)		(x)
#define htotarget32(x)		(x)
#define target32toh(x)		(x)
#define dkcksum_target(lp)	dkcksum(lp)
#define htotargetlabel(tlp, hlp)					\
	    do {*(tlp) = *(hlp);} while (/* CONSTCOND */0)
#define targettohlabel(hlp, tlp)					\
	    do {*(hlp) = *(tlp);} while (/* CONSTCOND */0)
#endif
