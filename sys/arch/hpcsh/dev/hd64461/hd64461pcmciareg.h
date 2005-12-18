/*	$NetBSD: hd64461pcmciareg.h,v 1.3 2005/12/18 21:47:10 uwe Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
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

#ifndef _HPCSH_DEV_HD64461PCMCIAREG_H_
#define _HPCSH_DEV_HD64461PCMCIAREG_H_

/*
 * PCC0 SH7709 Area 6 (memory and I/O card)
 */

/* PCC0 Interface Status Register (R) */
#define HD64461_PCC0ISR_REG8				0xb0002000
#define HD64461_PCC0ISR_P0READY			HD64461_PCCISR_READY
#define HD64461_PCC0ISR_IREQ			HD64461_PCCISR_READY
#define HD64461_PCC0ISR_P0MWP			HD64461_PCCISR_MWP
#define HD64461_PCC0ISR_P0VS2			HD64461_PCCISR_VS2
#define HD64461_PCC0ISR_P0VS1			HD64461_PCCISR_VS1
#define HD64461_PCC0ISR_P0CD2			HD64461_PCCISR_CD2
#define HD64461_PCC0ISR_P0CD1			HD64461_PCCISR_CD1
#define HD64461_PCC0ISR_P0BVD2			HD64461_PCCISR_BVD2
#define HD64461_PCC0ISR_SPKR0			HD64461_PCCISR_BVD2
#define HD64461_PCC0ISR_P0BVD1			HD64461_PCCISR_BVD1
#define HD64461_PCC0ISR_STSCHG0			HD64461_PCCISR_BVD1

/* PCC0 General Contorol Register (R/W) */
#define HD64461_PCC0GCR_REG8				0xb0002002
#define HD64461_PCC0GCR_P0DRVE			HD64461_PCCGCR_DRVE
#define HD64461_PCC0GCR_P0PCCR			HD64461_PCCGCR_PCCR
#define HD64461_PCC0GCR_P0PCCT			HD64461_PCCGCR_PCCT
#define HD64461_PCC0GCR_P0VCC0			HD64461_PCCGCR_VCC0
#define HD64461_PCC0GCR_P0MMOD			HD64461_PCCGCR_MMOD
#define HD64461_PCC0GCR_P0MMOD_16M		HD64461_PCCGCR_MMOD_16M
#define HD64461_PCC0GCR_P0MMOD_32M		HD64461_PCCGCR_MMOD_32M
/* these bits meaning different for P0MMOD mode */
#define HD64461_PCC0GCR_P0PA25			HD64461_PCCGCR_PA25
#define HD64461_PCC0GCR_P0PA24			HD64461_PCCGCR_PA24
#define HD64461_PCC0GCR_P0REG			HD64461_PCCGCR_PREG

/* PCC0 Card Status Change Register (R/W) */
#define HD64461_PCC0CSCR_REG8				0xb0002004
#define HD64461_PCC0CSCR_P0SCDI			HD64461_PCCCSCR_SCDI
#define HD64461_PCC0CSCR_P0IREQ			0x20
#define HD64461_PCC0CSCR_P0SC			0x10
#define HD64461_PCC0CSCR_P0CDC			HD64461_PCCCSCR_CDC
#define HD64461_PCC0CSCR_P0RC			HD64461_PCCCSCR_RC
#define HD64461_PCC0CSCR_P0BW			HD64461_PCCCSCR_BW
#define HD64461_PCC0CSCR_P0BD			HD64461_PCCCSCR_BD

/* PCC0 Card Status Change Interrupt Enable Register (R/W) */
#define HD64461_PCC0CSCIER_REG8				0xb0002006
#define HD64461_PCC0CSCIER_P0CRE		HD64461_PCCCSCIER_CRE

#define HD64461_PCC0CSCIER_P0IREQE_MASK		0x60
#define HD64461_PCC0CSCIER_P0IREQE_NONE		0x00
#define HD64461_PCC0CSCIER_P0IREQE_LEVEL	0x20
#define HD64461_PCC0CSCIER_P0IREQE_FEDGE	0x40
#define HD64461_PCC0CSCIER_P0IREQE_REDGE	0x60

#define HD64461_PCC0CSCIER_P0SCE		0x10
#define HD64461_PCC0CSCIER_P0CDE		HD64461_PCCCSCIER_CDE
#define HD64461_PCC0CSCIER_P0RE			HD64461_PCCCSCIER_RE 
#define HD64461_PCC0CSCIER_P0BWE		HD64461_PCCCSCIER_BWE
#define HD64461_PCC0CSCIER_P0BDE		HD64461_PCCCSCIER_BDE

/* PCC0 Software Control Register (R/W) */
#define HD64461_PCC0SCR_REG8				0xb0002008
#define HD64461_PCC0SCR_P0VCC1			HD64461_PCCSCR_VCC1
#define HD64461_PCC0SCR_P0SWP			HD64461_PCCSCR_SWP

/*
 * PCC1 SH7709 Area 5 (memory card only)
 */
/* PCC1 Interface Status Register (R) */
#define HD64461_PCC1ISR_REG8				0xb0002010
#define HD64461_PCC1ISR_P1READY			HD64461_PCCISR_READY
#define HD64461_PCC1ISR_P1MWP			HD64461_PCCISR_MWP
#define HD64461_PCC1ISR_P1VS2			HD64461_PCCISR_VS2
#define HD64461_PCC1ISR_P1VS1			HD64461_PCCISR_VS1
#define HD64461_PCC1ISR_P1CD2			HD64461_PCCISR_CD2
#define HD64461_PCC1ISR_P1CD1			HD64461_PCCISR_CD1
#define HD64461_PCC1ISR_P1BVD2			HD64461_PCCISR_BVD2
#define HD64461_PCC1ISR_P1BVD1			HD64461_PCCISR_BVD1

/* PCC1 General Contorol Register (R/W) */
#define HD64461_PCC1GCR_REG8				0xb0002012
#define HD64461_PCC1GCR_P1DRVE			HD64461_PCCGCR_DRVE
#define HD64461_PCC1GCR_P1PCCR			HD64461_PCCGCR_PCCR
#define HD64461_PCC1GCR_RESERVED		HD64461_PCCGCR_PCCT
#define HD64461_PCC1GCR_P1VCC0			HD64461_PCCGCR_VCC0
#define HD64461_PCC1GCR_P1MMOD			HD64461_PCCGCR_MMOD
#define HD64461_PCC1GCR_P1MMOD_16M		HD64461_PCCGCR_MMOD_16M
#define HD64461_PCC1GCR_P1MMOD_32M		HD64461_PCCGCR_MMOD_32M
#define HD64461_PCC1GCR_P1PA25			HD64461_PCCGCR_PA25
#define HD64461_PCC1GCR_P1PA24			HD64461_PCCGCR_PA24
#define HD64461_PCC1GCR_P1REG			HD64461_PCCGCR_PREG

/* PCC1 Card Status Change Register (R/W) */
#define HD64461_PCC1CSCR_REG8				0xb0002014
#define HD64461_PCC1CSCR_P1SCDI			HD64461_PCCCSCR_SCDI
#define HD64461_PCC1CSCR_P1CDC			HD64461_PCCCSCR_CDC
#define HD64461_PCC1CSCR_P1RC			HD64461_PCCCSCR_RC
#define HD64461_PCC1CSCR_P1BW			HD64461_PCCCSCR_BW
#define HD64461_PCC1CSCR_P1BD			HD64461_PCCCSCR_BD

/* PCC1 Card Status Change Interrupt Enable Register (R/W) */
#define HD64461_PCC1CSCIER_REG8				0xb0002016
#define HD64461_PCC1CSCIER_P1CRE		HD64461_PCCCSCIER_CRE
#define HD64461_PCC1CSCIER_P1CDE		HD64461_PCCCSCIER_CDE
#define HD64461_PCC1CSCIER_P1RE			HD64461_PCCCSCIER_RE
#define HD64461_PCC1CSCIER_P1BWE		HD64461_PCCCSCIER_BWE
#define HD64461_PCC1CSCIER_P1BDE		HD64461_PCCCSCIER_BDE

/* PCC1 Software Control Register (R/W) */
#define HD64461_PCC1SCR_REG8				0xb0002018
#define HD64461_PCC1SCR_P1VCC1			HD64461_PCCSCR_VCC1
#define HD64461_PCC1SCR_P1SWP			HD64461_PCCSCR_SWP

/*
 * General Control
 */
/* PCC0 Output pins Control Register (R/W) */
#define HD64461_PCCP0OCR_REG8				0xb000202a
#define HD64461_PCCP0OCR_P0DEPLUP		0x80
#define HD64461_PCCP0OCR_P0AEPLUP		0x10

/* PCC1 Output pins Control Register (R/W) */
#define HD64461_PCCP1OCR_REG8				0xb000202c
#define HD64461_PCCP1OCR_P1RST8MA		0x08
#define HD64461_PCCP1OCR_P1RST4MA		0x04
#define HD64461_PCCP1OCR_P1RAS8MA		0x02
#define HD64461_PCCP1OCR_P1RAS4MA		0x01

/* PC Card General Control Register (R/W) */
#define HD64461_PCCPGCR_REG8				0xb000202e
#define HD64461_PCCPGCR_PSSDIR			0x02
#define HD64461_PCCPGCR_PSSRDWR			0x01

/*
 * common defines.
 */
#define HD64461_PCC0_REGBASE			HD64461_PCC0ISR_REG8
#define HD64461_PCC1_REGBASE			HD64461_PCC1ISR_REG8
#define HD64461_PCC_ISR_OFS			0x0
#define HD64461_PCC_GCR_OFS			0x2
#define HD64461_PCC_CSCR_OFS			0x4
#define HD64461_PCC_CSCIER_OFS			0x6
#define HD64461_PCC_SCR_OFS			0x8

#define HD64461_PCCISR(x)						\
	(((x) ?  HD64461_PCC1_REGBASE : HD64461_PCC0_REGBASE) +		\
	HD64461_PCC_ISR_OFS)
#define HD64461_PCCGCR(x)						\
	(((x) ?  HD64461_PCC1_REGBASE : HD64461_PCC0_REGBASE) +		\
	HD64461_PCC_GCR_OFS)
#define HD64461_PCCCSCR(x)						\
	(((x) ?  HD64461_PCC1_REGBASE : HD64461_PCC0_REGBASE) +		\
	HD64461_PCC_CSCR_OFS)
#define HD64461_PCCCSCIER(x)						\
	(((x) ?  HD64461_PCC1_REGBASE : HD64461_PCC0_REGBASE) +		\
	HD64461_PCC_CSCIER_OFS)
#define HD64461_PCCSCR(x)						\
	(((x) ?  HD64461_PCC1_REGBASE : HD64461_PCC0_REGBASE) +		\
	HD64461_PCC_SCR_OFS)

#define HD64461_PCCISR_READY			0x80
#define HD64461_PCCISR_MWP			0x40
#define HD64461_PCCISR_VS2			0x20
#define HD64461_PCCISR_VS1			0x10
#define HD64461_PCCISR_CD2			0x08
#define HD64461_PCCISR_CD1			0x04
#define HD64461_PCCISR_BVD2			0x02
#define HD64461_PCCISR_BVD1			0x01

#define HD64461_PCCGCR_DRVE			0x80
#define HD64461_PCCGCR_PCCR			0x40
#define HD64461_PCCGCR_PCCT			0x20
#define HD64461_PCCGCR_VCC0			0x10
#define HD64461_PCCGCR_MMOD			0x08
#define HD64461_PCCGCR_MMOD_16M			0x08
#define HD64461_PCCGCR_MMOD_32M			0x00
#define HD64461_PCCGCR_PA25			0x04
#define HD64461_PCCGCR_PA24			0x02
#define HD64461_PCCGCR_PREG			0x01

#define HD64461_PCCCSCR_SCDI			0x80
#define HD64461_PCCCSCR_CDC			0x08
#define HD64461_PCCCSCR_RC			0x04
#define HD64461_PCCCSCR_BW			0x02
#define HD64461_PCCCSCR_BD			0x01

#define HD64461_PCCCSCIER_CRE			0x80
#define HD64461_PCCCSCIER_CDE			0x08
#define HD64461_PCCCSCIER_RE			0x04
#define HD64461_PCCCSCIER_BWE			0x02
#define HD64461_PCCCSCIER_BDE			0x01

#define HD64461_PCCSCR_VCC1			0x02
#define HD64461_PCCSCR_SWP			0x01

#endif /* !_HPCSH_DEV_HD64461PCMCIAREG_H_ */
