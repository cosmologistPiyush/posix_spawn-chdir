/*	$NetBSD: rumpnfsd.c,v 1.5 2010/09/02 15:13:55 pooka Exp $	*/

/*-
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
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

#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

void *mountd_main(void *);
void *rpcbind_main(void *);
int nfsd_main(int, char **);

sem_t gensem;

#include "../../../net/config/netconfig.c"
#include "../../common/h_fsmacros.h"
#include "svc_fdset.h"

#include <rump/rump.h>
#include <rump/rump_syscalls.h>

int
main(int argc, char *argv[])
{
	const char *ethername;
	const char *serveraddr;
	const char *netmask;
	const char *exportpath;
	const char *imagename;
	char ifname[IFNAMSIZ];
	void *fsarg;
	pthread_t t;
	int rv;

	/* use defaults? */
	if (argc == 1) {
		ethername = "etherbus";
		serveraddr = "10.3.2.1";
		netmask = "255.255.255.0";
		exportpath = "/myexport";
		imagename = "ffs.img";
	} else {
		ethername = argv[1];
		serveraddr = argv[2];
		netmask = argv[3];
		exportpath = argv[4];
		imagename = argv[5];
	}

	rump_init();
	init_fdsets();

	rv = rump_pub_etfs_register("/etc/exports", "./exports", RUMP_ETFS_REG);
	if (rv) {
		errx(1, "register /etc/exports: %s", strerror(rv));
	}

	/* mini-mtree for mountd */
	rump_sys_mkdir("/var", 0777);
	rump_sys_mkdir("/var/run", 0777);
	rump_sys_mkdir("/var/db", 0777);

	if (ffs_fstest_newfs(NULL, &fsarg,
	    imagename, FSTEST_IMGSIZE, NULL) != 0)
		err(1, "newfs failed");
	if (ffs_fstest_mount(NULL, fsarg, exportpath, 0) != 0)
		err(1, "mount failed");

#if 0
	/*
	 * Serve from host instead of dedicated mount?
	 * THIS IS MORE EVIL THAN MURRAY THE DEMONIC TALKING SKULL!
	 */

	if (ukfs_modload("/usr/lib/librumpfs_syspuffs.so") < 1)
		errx(1, "modload");

	mount_syspuffs_parseargs(__arraycount(pnullarg), pnullarg,
	    &args, &mntflags, canon_dev, canon_dir);
	if ((ukfs = ukfs_mount(MOUNT_PUFFS, "/", UKFS_DEFAULTMP, MNT_RDONLY,
	    &args, sizeof(args))) == NULL)
		err(1, "mount");

	if (ukfs_modload("/usr/lib/librumpfs_nfsserver.so") < 1)
		errx(1, "modload");
#endif

	if (sem_init(&gensem, 1, 0) == -1)
		err(1, "gensem init");

	/* create interface */
	netcfg_rump_makeshmif(ethername, ifname);
	netcfg_rump_if(ifname, serveraddr, netmask);

	/*
	 * No syslogging, thanks.
	 * XXX: "0" does not modify the mask, so pick something
	 * which is unlikely to cause any logging
	 */
	setlogmask(0x10000000);

	if (pthread_create(&t, NULL, rpcbind_main, NULL) == -1)
		err(1, "rpcbind");
	sem_wait(&gensem);

	if (pthread_create(&t, NULL, mountd_main, NULL) == -1)
		err(1, "mountd");
	sem_wait(&gensem);

	rv = 0;
	/* signal the other process we're almost done */
	if (write(3, &rv, 4) != 4)
		errx(1, "magic write failed");

	{
	char *nfsargv[] = { __UNCONST("nfsd"), NULL };
	nfsd_main(1, nfsargv);
	}
	/*NOTREACHED*/

	return 0;
}
