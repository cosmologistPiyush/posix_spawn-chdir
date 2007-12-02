/*	$NetBSD: conf.c,v 1.4 2007/12/02 05:40:25 tsutsui Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
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
 *	@(#)conf.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/types.h>
#include <lib/libsa/stand.h>
#include <lib/libsa/dosfs.h>
#include <lib/libsa/ufs.h>
#include <lib/libsa/lfs.h>
#include <lib/libsa/cd9660.h>
#ifdef SUPPORT_USTARFS
#include <lib/libsa/ustarfs.h>
#endif
#ifdef BOOTNET
#include <netinet/in.h>
#include <lib/libsa/dev_net.h>
#include <lib/libsa/nfs.h>
#endif
#include "disk.h"

#ifndef LIBSA_SINGLE_DEVICE

#ifdef LIBSA_NO_DEV_CLOSE
#define diskclose /*(()(struct open_file*))*/0
#endif

#ifdef LIBSA_NO_DEV_IOCTL
#define diskioctl /*(()(struct open_file*, u_long, void*))*/0
#else
#define	diskioctl		noioctl
#endif

struct devsw devsw[] = {
	{ "disk", diskstrategy, diskopen, diskclose, diskioctl },	/* 0 */
#ifdef BOOTNET
	{ "tftp", net_strategy, net_open, net_close, net_ioctl },	/* 1 */
#endif
};

int ndevs = __arraycount(devsw);
#endif


#ifndef LIBSA_SINGLE_FILESYSTEM
#ifdef LIBSA_NO_FS_CLOSE
#define ufs_close	0
#define lfsv1_close	0
#define lfsv2_close	0
#define cd9660_close	0
#define ustarfs_close	0
#define nfs_close	0
#endif
#ifdef LIBSA_NO_FS_WRITE
#define ufs_write	0
#define lfsv1_write	0
#define lfsv2_write	0
#define cd9660_write	0
#define ustarfs_write	0
#define nfs_write	0
#endif

struct fs_ops file_system[] = {
	FS_OPS(ffsv1),
	FS_OPS(ffsv2),
	FS_OPS(dosfs),
	FS_OPS(lfsv1),
	FS_OPS(lfsv2),
	FS_OPS(cd9660),
#ifdef SUPPORT_USTARFS
	FS_OPS(ustarfs),
#endif
#ifdef BOOTNET
	FS_OPS(nfs),
#endif
};

int nfsys = __arraycount(file_system);
#endif

#ifdef BOOTNET
extern struct netif_driver arc_netif_driver;

struct netif_driver *netif_drivers[] = {
	&arc_netif_driver,
};
int n_netif_drivers = __arraycount(netif_drivers);
#endif
