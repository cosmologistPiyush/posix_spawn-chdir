/*	$NetBSD: if_arp.h,v 1.11 1997/03/25 02:30:52 jonathan Exp $	*/

/*
 * Copyright (c) 1986, 1993
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 *	@(#)if_arp.h	8.1 (Berkeley) 6/10/93
 */

#ifndef _IF_ARP_H_
#define _IF_ARP_H_
/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */
struct	arphdr {
	u_int16_t ar_hrd;	/* format of hardware address */
#define ARPHRD_ETHER 	1	/* ethernet hardware format */
#define ARPHRD_ARCNET 	7	/* ethernet hardware format */
#define ARPHRD_FRELAY 	15	/* frame relay hardware format */
#define ARPHRD_STRIP 	23	/* Ricochet Starmode Radio hardware format */
	u_int16_t ar_pro;	/* format of protocol address */
	u_int8_t  ar_hln;	/* length of hardware address */
	u_int8_t  ar_pln;	/* length of protocol address */
	u_int16_t ar_op;	/* one of: */
#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
#define	ARPOP_REVREQUEST 3	/* request protocol address given hardware */
#define	ARPOP_REVREPLY	4	/* response giving protocol address */
#define	ARPOP_INVREQUEST 8 	/* request to identify peer */
#define	ARPOP_INVREPLY	9	/* response identifying peer */
/*
 * The remaining fields are variable in size,
 * according to the sizes above.
 */
#ifdef COMMENT_ONLY
	u_int8_t  ar_sha[];	/* sender hardware address */
	u_int8_t  ar_spa[];	/* sender protocol address */
	u_int8_t  ar_tha[];	/* target hardware address */
	u_int8_t  ar_tpa[];	/* target protocol address */
#endif
	u_int8_t  ar_remain[0];	/* minimum size, normally bigger */
#define ar_sha(ap) (((ap)->ar_remain)+0)
#define ar_spa(ap) (((ap)->ar_remain)+(ap)->ar_hln)
#define ar_tha(ap) (((ap)->ar_remain)+(ap)->ar_hln+(ap)->ar_pln)
#define ar_tpa(ap) (((ap)->ar_remain)+2*(ap)->ar_hln+(ap)->ar_pln)
};


/*
 * ARP ioctl request
 */
struct arpreq {
	struct	sockaddr arp_pa;		/* protocol address */
	struct	sockaddr arp_ha;		/* hardware address */
	int	arp_flags;			/* flags */
};
/*  arp_flags and at_flags field values */
#define	ATF_INUSE	0x01	/* entry in use */
#define ATF_COM		0x02	/* completed entry (enaddr valid) */
#define	ATF_PERM	0x04	/* permanent entry */
#define	ATF_PUBL	0x08	/* publish entry (respond for other host) */
#define	ATF_USETRAILERS	0x10	/* has requested trailers */

#endif _IF_ARP_H_
