/*	$NetBSD: scnvar.h,v 1.2 1996/12/23 08:37:11 matthias Exp $	*/

/*
 * Copyright (c) 1996 Phil Budne.
 * Copyright (c) 1993 Philip A. Nelson.
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
 *	This product includes software developed by Philip A. Nelson.
 * 4. The name of Philip A. Nelson may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PHILIP NELSON ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL PHILIP NELSON BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	scnvar.h: definitions for pc532 2681/2692 duart driver
 */

/* Constants. */
#ifdef COMDEF_SPEED
#undef  TTYDEF_SPEED
#define TTYDEF_SPEED    COMDEF_SPEED	/* default baud rate */
#endif

#define SCN_FIRST_ADR	  0x28000000	/* address of first RS232 port */
#define SCN_FIRST_MAP_ADR 0xFFC80000	/* mapped address of first port */

#define SCN_SIZE	         0x8	/* address space for port */

#define SCN_CONSOLE 		   0	/* minor number of console */

#define SCN_CON_MAP_STAT  0xFFC80001	/* raw addresses for console */
#define SCN_CON_MAP_DATA  0xFFC80003	/* Mapped .... */
#define SCN_CON_MAP_ISR	  0xFFC80005

#define SCN_CON_STAT 	  0x28000001	/* raw addresses for console */
#define SCN_CON_DATA  	  0x28000003	/* Unmapped .... */
#define SCN_CON_ISR	  0x28000005

/* output port bits */
#define OP_RTSA		OP_OP0
#define OP_RTSB		OP_OP1
#define OP_DTRA		OP_OP2
#define OP_DTRB		OP_OP3
/* OP4-7 not connected */

/* input port bits */
#define IP_CTSA		IP_IP0
#define IP_CTSB		IP_IP1
#define IP_DCDB		IP_IP2
#define IP_DCDA		IP_IP3
/* IP4-6 not connected */

#define ACR_DELTA_CTSA	ACR_DELTA_IP0
#define ACR_DELTA_CTSB	ACR_DELTA_IP1
#define ACR_DELTA_DCDB	ACR_DELTA_IP2
#define ACR_DELTA_DCDA	ACR_DELTA_IP3

#define IPCR_CTSA	IPCR_IP0
#define IPCR_CTSB	IPCR_IP1
#define IPCR_DCDB	IPCR_IP2
#define IPCR_DCDA	IPCR_IP3
#define IPCR_DELTA_CTSA	IPCR_DELTA_IP0
#define IPCR_DELTA_CTSB	IPCR_DELTA_IP1
#define IPCR_DELTA_DCDB	IPCR_DELTA_IP2
#define IPCR_DELTA_DCDA	IPCR_DELTA_IP3

#define SP_GRP0		0x00
#define SP_GRP1		0x10
#define SP_BOTH		0x20

#define SCN_OP_BIS(SC,VAL) ((SC)->duart->base[DU_OPSET] = (VAL))
#define SCN_OP_BIC(SC,VAL) ((SC)->duart->base[DU_OPCLR] = (VAL))

#define SCN_DCD(SC) (((SC)->duart->base[DU_IP] & (SC)->sc_ip_dcd) == 0)

/* pc532 duarts are auto-sized to byte-wide */
#define CH_SZ		8
#define DUART_SZ	16
#define SCN_REG(n)	(n)		/* n'th reg at n'th byte!! */

/* The DUART description struct; for data common to both channels */
struct duart_info {
	volatile u_char *base;
	int	i_speed[2], o_speed[2];	/* Channel A and B speeds. */
	char	i_code[2], o_code[2];	/* Channel A and B speeds. */
	char	speed_grp;		/* BRG speed group */
	char	acr_bits;		/* ACR bits 0-6 */
	char	imr_int_bits;		/* IMR bits current set. */
	char	opcr_bits;		/* OPCR bits to set */
	char	hwflags;
#define SCN_HW_2692 1
#define SCN_HW_26C92 2
};

#define SCN_RING_BITS	9	/* 512 byte buffers */
#define SCN_RING_SIZE	(1<<SCN_RING_BITS)	/* must be a power of two */
#define SCN_RING_MASK	(SCN_RING_SIZE-1)
#define SCN_RING_THRESH	(SCN_RING_SIZE/2)
#define SCN_RING_HIWAT  (SCN_RING_SIZE - (SCN_RING_SIZE >> 2))

/* scn channel state */
struct scn_softc {
	struct device scn_dev;
	struct tty *scn_tty;
/* XXXPLB: prefix with scn_; */
	int     unit;			/* unit number of this line (base 0) */
	struct duart_info *duart;	/* pointer to duart struct */
	volatile u_char *chbase;	/* per-channel registers (CH_xxx) */
	u_char	lstatus;		/* last line status (sr) */
	u_char	scn_swflags;		/* from config / TIOCxFLAGS */
#define SCN_SW_SOFTCAR	0x01
#define SCN_SW_CLOCAL	0x02
#define SCN_SW_CRTSCTS	0x04
#define SCN_SW_MDMBUF	0x08		/* not implemented */

/* I wish there was a TS_DIALOUT flag! */
	u_char  scn_dialout;		/* set if open for dialout */
#define SCN_DIALOUT(SCN) ((SCN)->scn_dialout)
#define SCN_SETDIALOUT(SCN) (SCN)->scn_dialout = 1
#define SCN_CLRDIALOUT(SCN) (SCN)->scn_dialout = 0

/* bits in input port register */
	u_char  sc_ip_dcd;
	u_char  sc_ip_cts;

/* bits in output port registers */
	u_char  sc_op_rts;
	u_char  sc_op_dtr;

/* interrupt mask bits */
	u_char  sc_tx_int;

/* error counts */
	u_long	framing_errors; 	/* NYI */
	u_long	overrun_errors;
	u_long	parity_errors;		/* NYI */
	u_long  break_interrupts;	/* NYI */

/* ring buffer for rxrdy interrupt */
	u_int	sc_rbget;		/* ring buffer `get' index */
	volatile u_int sc_rbput;	/* ring buffer `put' index */
	int	sc_rbhiwat;
	short   sc_rbuf[SCN_RING_SIZE];	/* status + data */
	long	sc_fotime;		/* last fifo overrun message */
	long	sc_rotime;		/* last ring overrun message */
	u_long	ring_overruns;		/* number of ring buffer overruns */
	/* Flags to communicate with scntty_softint() */
	volatile char sc_rx_blocked;	/* input block at ring */
};
