/*	$NetBSD: rump_nfs.c,v 1.7 2009/01/11 20:31:03 pooka Exp $	*/

/*
 * Copyright (c) 2008 Antti Kantee.  All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/mount.h>

#include <nfs/nfsmount.h>

#include <rump/rump.h>
#include <rump/p2k.h>

#include <err.h>
#include <puffs.h>
#include <stdlib.h>
#include <string.h>

#include "mount_nfs.h"

int
main(int argc, char *argv[])
{
	struct nfs_args args;
	char canon_dev[MAXPATHLEN], canon_dir[MAXPATHLEN];
	int rv, mntflags;

#if 0
	extern int rumpns_nfs_niothreads; /* XXX */
	char *thr;
	/*
	 * XXX: but we can't call rump init before we detach (fork()) in
	 * p2k_run_fs() lest we lose our threads.
	 */
	thr = getenv("RUMP_THREADS");
	if (!thr || !*thr)
		rumpns_nfs_niothreads = 0;
#else
	setenv("RUMP_THREADS", "1", 1);
#endif

	setprogname(argv[0]);
	mount_nfs_parseargs(argc, argv, &args, &mntflags, canon_dev, canon_dir);

	rv = p2k_run_fs(MOUNT_NFS, canon_dev, canon_dir, mntflags, &args,
	    sizeof(args), 0);
	if (rv == -1)
		err(1, "mount");

	return 0;
}
