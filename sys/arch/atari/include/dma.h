/*	$NetBSD: dma.h,v 1.2 1995/03/26 07:24:35 leo Exp $	*/

/*
 * Copyright (c) 1995 Leo Weppelman.
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
 *      This product includes software developed by Leo Weppelman.
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

#ifndef _MACHINE_DMA_H
#define _MACHINE_DMA_H

/*
 * Atari TT hardware:
 * FDC/ACSI DMA circuitry
 */

#define	DMA	((struct dma *)AD_DMA)

struct dma {
	volatile short	dma_gap[2];	/* reserved			*/
	volatile short	dma_data;	/* controller data path		*/
	volatile short	dma_mode;	/* mode register		*/
	volatile char	dma_addr[6];	/* base address H/M/L		*/
	volatile short	dma_drvmode;	/* floppy density settings	*/
};

#define	dma_nsec      dma_data		/* sector count			*/
#define	dma_stat      dma_mode		/* status register		*/

/*
 * Mode register bits
 */
/*		0x0001		*//* not used				*/
#define	A0	0x0002		/* signal A0 to fdc/hdc			*/
#define	A1	0x0004		/* signal A1 to fdc/hdc			*/
#define	HDC	0x0008		/* must be on if accessing hdc		*/
#define	SCREG	0x0010		/* access sector count register		*/
/*		0x0020		*//* reserved				*/
#define	NODMA	0x0040		/* no DMA (yet)				*/
#define	FDC	0x0080		/* must be on if accessing fdc		*/
#define	WRBIT	0x0100		/* write to fdc/hdc via dma_data	*/

/*
 * Status register bits
 */
#define	DMAOK	0x0001		/* something wrong			*/
#define	SCNOT0	0x0002		/* sector count not 0			*/
#define	DATREQ	0x0004		/* FDC data request signal		*/

/*
 * Indices into dma_addr.
 * Access low byte of 16 bits.
 * Fill low/mid/high in this order.
 */
#define	AD_HIGH	1
#define	AD_MID	3
#define	AD_LOW	5

/*
 * Defines for 'dmadrv_mode'.
 */
#define	FDC_HDSET	1	/* Set FDC for High density		*/
#define	FDC_HDSIG	2	/* Signal HD present to drive		*/
#endif /* _MACHINE_DMA_H */
