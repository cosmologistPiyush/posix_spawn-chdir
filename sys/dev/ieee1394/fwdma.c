/*	$NetBSD: fwdma.c,v 1.4 2006/04/30 14:14:06 kiyohara Exp $	*/
/*-
 * Copyright (c) 2003
 * 	Hidetoshi Shimokawa. All rights reserved.
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
 *
 *	This product includes software developed by Hidetoshi Shimokawa.
 *
 * 4. Neither the name of the author nor the names of its contributors
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
 */

#include <sys/cdefs.h>
#ifdef __FBSDID
__FBSDID("$FreeBSD: /repoman/r/ncvs/src/sys/dev/firewire/fwdma.c,v 1.7 2005/01/06 01:42:41 imp Exp $");
#endif

#if defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/malloc.h>
#if defined(__FreeBSD__) && __FreeBSD_version >= 501102 
#include <sys/lock.h>
#include <sys/mutex.h>
#endif

#include <sys/bus.h>
#include <machine/bus.h>

#ifdef __DragonFly__
#include <bus/firewire/fw_port.h>
#include <bus/firewire/firewire.h>
#include <bus/firewire/firewirereg.h>
#include <bus/firewire/fwdma.h>
#else
#include <dev/firewire/fw_port.h>
#include <dev/firewire/firewire.h>
#include <dev/firewire/firewirereg.h>
#include <dev/firewire/fwdma.h>
#endif
#elif defined(__NetBSD__)
#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/malloc.h>

#include <machine/bus.h>
#include <machine/vmparam.h>

#include <dev/ieee1394/fw_port.h>
#include <dev/ieee1394/firewire.h>
#include <dev/ieee1394/firewirereg.h>
#include <dev/ieee1394/fwdma.h>
#endif

static void
fwdma_map_cb(void *arg, bus_dma_segment_t *segs, int nseg, int error)
{
	bus_addr_t *baddr;

	if (error)
		printf("fwdma_map_cb: error=%d\n", error);
	baddr = (bus_addr_t *)arg;
	*baddr = segs->ds_addr;
}

void *
fwdma_malloc(struct firewire_comm *fc, int alignment, bus_size_t size,
	struct fwdma_alloc *dma, int flag)
{
	int err;

	dma->v_addr = NULL;
	err = fw_bus_dma_tag_create(
		/*parent*/ fc->dmat,
		/*alignment*/ alignment,
		/*boundary*/ 0,
		/*lowaddr*/ BUS_SPACE_MAXADDR_32BIT,
		/*highaddr*/ BUS_SPACE_MAXADDR,
		/*filter*/NULL, /*filterarg*/NULL,
		/*maxsize*/ size,
		/*nsegments*/ 1,
		/*maxsegsz*/ BUS_SPACE_MAXSIZE_32BIT,
		/*flags*/ BUS_DMA_ALLOCNOW,
		/*lockfunc*/busdma_lock_mutex,
		/*lockarg*/&Giant,
		&dma->fw_dma_tag);
	if (err) {
		printf("fwdma_malloc: failed(1)\n");
		return(NULL);
	}

	err = fw_bus_dmamem_alloc(dma->fw_dma_tag, &dma->v_addr,
		flag, &dma->dma_map);
	if (err) {
		printf("fwdma_malloc: failed(2)\n");
		fw_bus_dma_tag_destroy(dma->fw_dma_tag);
		return(NULL);
	}

	err = fw_bus_dmamap_load(dma->fw_dma_tag, dma->dma_map, dma->v_addr,
		size, fwdma_map_cb, &dma->bus_addr, flag);
	if (err != 0) {
		printf("fwdma_malloc: failed(3)\n");
		fw_bus_dmamem_free(dma->fw_dma_tag, dma->v_addr, dma->dma_map);
		fw_bus_dma_tag_destroy(dma->fw_dma_tag);
		return(NULL);
	}

	return(dma->v_addr);
}

void
fwdma_free(struct firewire_comm *fc, struct fwdma_alloc *dma)
{
        fw_bus_dmamap_unload(dma->fw_dma_tag, dma->dma_map);
	fw_bus_dmamem_free(dma->fw_dma_tag, dma->v_addr, dma->dma_map);
	fw_bus_dma_tag_destroy(dma->fw_dma_tag);
}


void *
fwdma_malloc_size(fw_bus_dma_tag_t dmat, bus_dmamap_t *dmamap,
	bus_size_t size, bus_addr_t *bus_addr, int flag)
{
	void *v_addr;

	if (fw_bus_dmamem_alloc(dmat, &v_addr, flag, dmamap)) {
		printf("fwdma_malloc_size: failed(1)\n");
		return(NULL);
	}
	if (fw_bus_dmamap_load(dmat, *dmamap, v_addr, size,
			fwdma_map_cb, bus_addr, flag)) {
		printf("fwdma_malloc_size: failed(2)\n");
		fw_bus_dmamem_free(dmat, v_addr, *dmamap);
		return(NULL);
	}
	return(v_addr);
}

void
fwdma_free_size(fw_bus_dma_tag_t dmat, bus_dmamap_t dmamap,
		void *vaddr, bus_size_t size)
{
	fw_bus_dmamap_unload(dmat, dmamap);
	fw_bus_dmamem_free(dmat, vaddr, dmamap);
} 

/*
 * Allocate multisegment dma buffers
 * each segment size is eqaul to ssize except last segment.
 */
struct fwdma_alloc_multi *
fwdma_malloc_multiseg(struct firewire_comm *fc, int alignment,
		int esize, int n, int flag)
{
	struct fwdma_alloc_multi *am;
	struct fwdma_seg *seg;
	bus_size_t ssize;
	int nseg;

	if (esize > PAGE_SIZE) {
		/* round up to PAGE_SIZE */
		esize = ssize = roundup2(esize, PAGE_SIZE);
		nseg = n;
	} else {
		/* allocate PAGE_SIZE segment for small elements */
		ssize = rounddown(PAGE_SIZE, esize);
		nseg = howmany(n, ssize / esize);
	}
	am = (struct fwdma_alloc_multi *)malloc(sizeof(struct fwdma_alloc_multi)
			+ sizeof(struct fwdma_seg)*nseg, M_FW, M_WAITOK);
	if (am == NULL) {
		printf("fwdma_malloc_multiseg: malloc failed\n");
		return(NULL);
	}
	am->ssize = ssize;
	am->esize = esize;
	am->nseg = 0;
	if (fw_bus_dma_tag_create(
			/*parent*/ fc->dmat,
			/*alignment*/ alignment,
			/*boundary*/ 0,
			/*lowaddr*/ BUS_SPACE_MAXADDR_32BIT,
			/*highaddr*/ BUS_SPACE_MAXADDR,
			/*filter*/NULL, /*filterarg*/NULL,
			/*maxsize*/ ssize,
			/*nsegments*/ 1,
			/*maxsegsz*/ BUS_SPACE_MAXSIZE_32BIT,
			/*flags*/ BUS_DMA_ALLOCNOW,
			/*lockfunc*/busdma_lock_mutex,
			/*lockarg*/&Giant,
			&am->fw_dma_tag)) {
		printf("fwdma_malloc_multiseg: tag_create failed\n");
		free(am, M_FW);
		return(NULL);
	}

#if 0
#if defined(__DragonFly__) || __FreeBSD_version < 500000
	printf("malloc_multi: ssize=%d nseg=%d\n", ssize, nseg);
#else
	printf("malloc_multi: ssize=%td nseg=%d\n", ssize, nseg);
#endif
#endif
	for (seg = &am->seg[0]; nseg --; seg ++) {
		seg->v_addr = fwdma_malloc_size(am->fw_dma_tag, &seg->dma_map,
			ssize, &seg->bus_addr, flag);
		if (seg->v_addr == NULL) {
			printf("fwdma_malloc_multi: malloc_size failed %d\n",
				am->nseg);
			fwdma_free_multiseg(am);
			return(NULL);
		}
		am->nseg++;
	}
	return(am);
}

void
fwdma_free_multiseg(struct fwdma_alloc_multi *am)
{
	struct fwdma_seg *seg;

	for (seg = &am->seg[0]; am->nseg --; seg ++) {
		fwdma_free_size(am->fw_dma_tag, seg->dma_map,
			seg->v_addr, am->ssize);
	}
	fw_bus_dma_tag_destroy(am->fw_dma_tag);
	free(am, M_FW);
}
