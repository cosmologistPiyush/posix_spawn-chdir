/*	$NetBSD: if_levar.h,v 1.3 1997/10/09 07:41:03 jtc Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Leo Weppelman.
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


/*
 * LANCE register offsets.
 */
#define	LER_RDP		0	/* Data port			*/
#define	LER_RAP		2	/* Register select port		*/
#define	LER_IVEC	7	/* Interrupt vector		*/
#define	LER_EEPROM	13	/* PAM's Eeprom enable port	*/
#define	LER_MEME	15	/* PAM's Mem enable port	*/

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * arpcom.ac_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */
struct le_softc {
	struct	am7990_softc	sc_am7990;	/* glue to MI code */

	bus_space_tag_t		sc_iot;		/* glue to bus code	*/
	bus_space_tag_t		sc_memt;
	bus_space_handle_t	sc_ioh;
	bus_space_handle_t	sc_memh;

	struct	intrhand	*sc_intr;
	int			sc_type;	/* Type of board found	*/
	int			sc_splval;	/* XXX: hw. splval	*/
};

/*
 * Board Type:
 */
#define	LE_PAM		0
#define	LE_OLD_RIEBL	1
#define	LE_NEW_RIEBL	2

/*
 * Determine type of RIEBL card by magic
 */
#define RIEBL_MAGIC		0x09051990UL
#define RIEBL_MAGIC_ADDR	0xee8a

/*
 * Where the RIEBL MAC address is stored
 */
#define	RIEBL_MAC_ADDR		0xee8e

/*
 * ... and the RIEBL interrupt vector
 */
#define	RIEBL_IVEC_ADDR		0xfffe

/*
 * ... and reserved area
 */
#define	RIEBL_RES_START		0xee70
#define	RIEBL_RES_END		0xeec0
