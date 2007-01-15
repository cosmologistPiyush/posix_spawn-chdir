/*	$NetBSD: sysctlfs.c,v 1.12 2007/01/15 00:40:37 pooka Exp $	*/

/*
 * Copyright (c) 2006, 2007  Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
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

/*
 * sysctlfs: mount sysctls as a file system tree
 *
 * XXXX: this is a very quick hack fs.  it's not even complete,
 * please don't use it as an example.  actually, this code is so bad that
 * it's nearly a laugh, it's nearly a laugh, but it's really a cry
 */

#include <sys/types.h>
#include <sys/sysctl.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <mntopts.h>
#include <puffs.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

PUFFSOP_PROTOS(sysctlfs)

#define N_HIERARCHY 16
struct sfsnode {
	struct sysctlnode sctln;
	struct puffs_node *dotdot;
	ino_t myid;
	int refcount;
};

typedef int SfsName[N_HIERARCHY];

struct sfsnode rn;
SfsName sname_root;
struct timespec fstime;

ino_t nextid = 3;

#define ISADIR(a) ((SYSCTL_TYPE(a->sctln.sysctl_flags) == CTLTYPE_NODE))
#define SFS_MAXFILE 8192
#define SFS_NODEPERDIR 128

static int sysctlfs_domount(struct puffs_usermount *);

static int
sysctlfs_pathbuild(struct puffs_usermount *pu, struct puffs_pathobj *parent,
	struct puffs_pathobj *comp, size_t offset, struct puffs_pathobj *res)
{
	SfsName *sname;

	assert(offset <= comp->po_len);
	assert(parent->po_len + comp->po_len < N_HIERARCHY); /* code uses +1 */

	sname = malloc(sizeof(SfsName));
	assert(sname != NULL);

	memcpy(sname, parent->po_path, parent->po_len * sizeof(int));
	memcpy(&sname[parent->po_len], (int *)comp->po_path + offset,
	    (comp->po_len - offset) * sizeof(int));

	res->po_path = sname;
	res->po_len = parent->po_len;

	return 0;
}

static int
sysctlfs_pathtransform(struct puffs_usermount *pu, struct puffs_pathobj *p,
	const struct puffs_cn *pcn, struct puffs_pathobj *res)
{

	res->po_path = NULL;
	res->po_len = 0;

	return 0;
}

static void
sysctlfs_pathfree(struct puffs_usermount *pu, struct puffs_pathobj *po)
{

	free(po->po_path);
}

static void *
pathcmp(struct puffs_usermount *pu, struct puffs_node *pn, void *arg)
{
	struct puffs_pathobj *po_cmp = arg;

	if (po_cmp->po_len != PNPLEN(pn))
		return NULL;

	if (memcmp(PNPATH(pn), po_cmp->po_path, PNPLEN(pn) * sizeof(int)) == 0)
		return pn;

	return NULL;
}

int
main(int argc, char *argv[])
{
	struct puffs_usermount *pu;
	struct puffs_ops *pops;
	mntoptparse_t mp;
	int mntflags, pflags, lflags;
	int ch;

	setprogname(argv[0]);

	if (argc < 2)
		errx(1, "usage: %s [-o mntopts] mountpath", getprogname());

	mntflags = pflags = lflags = 0;
	while ((ch = getopt(argc, argv, "o:s")) != -1) {
		switch (ch) {
		case 'o':
			mp = getmntopts(optarg, puffsmopts, &mntflags, &pflags);
			if (mp == NULL)
				err(1, "getmntopts");
			freemntopts(mp);
			break;
		case 's':
			lflags = PUFFSLOOP_NODAEMON;
			break;
		}
	}
	argv += optind;
	argc -= optind;
	pflags |= PUFFS_FLAG_BUILDPATH | PUFFS_KFLAG_NOCACHE;

	if (pflags & PUFFS_FLAG_OPDUMP)
		lflags |= PUFFSLOOP_NODAEMON;

	if (argc != 1)
		errx(1, "usage: %s [-o mntopts] mountpath", getprogname());

	PUFFSOP_INIT(pops);

	PUFFSOP_SETFSNOP(pops, unmount);
	PUFFSOP_SETFSNOP(pops, sync);
	PUFFSOP_SETFSNOP(pops, statvfs);

	/* XXX: theoretically should support reclaim */
	PUFFSOP_SET(pops, sysctlfs, node, lookup);
	PUFFSOP_SET(pops, sysctlfs, node, getattr);
	PUFFSOP_SET(pops, sysctlfs, node, setattr);
	PUFFSOP_SET(pops, sysctlfs, node, readdir);
	PUFFSOP_SET(pops, sysctlfs, node, read);
	PUFFSOP_SET(pops, sysctlfs, node, write);

	if ((pu = puffs_mount(pops, argv[0], mntflags, "sysctlfs", NULL,
	    pflags, 0)) == NULL)
		err(1, "mount");

	puffs_set_pathbuild(pu, sysctlfs_pathbuild);
	puffs_set_pathtransform(pu, sysctlfs_pathtransform);
	puffs_set_pathfree(pu, sysctlfs_pathfree);

	if (sysctlfs_domount(pu) != 0)
		errx(1, "domount");
	if (puffs_mainloop(pu, lflags) == -1)
		err(1, "mainloop");

	return 0;
}

static int
sysctlfs_domount(struct puffs_usermount *pu)
{
	struct puffs_pathobj *po_root;
	struct timeval tv_now;
	struct statvfs sb;

	rn.dotdot = NULL;
	rn.myid = 2;
	rn.sctln.sysctl_flags = CTLTYPE_NODE;
	rn.refcount = 2;

	gettimeofday(&tv_now, NULL);
	TIMEVAL_TO_TIMESPEC(&tv_now, &fstime);

	pu->pu_pn_root = puffs_pn_new(pu, &rn);
	assert(pu->pu_pn_root != NULL);

	po_root = puffs_getrootpathobj(pu);
	po_root->po_path = &sname_root;
	po_root->po_len = 0;

	puffs_zerostatvfs(&sb);
	if (puffs_start(pu, pu->pu_pn_root, &sb) == -1)
		return errno;

	return 0;
}

static void
doprint(struct sfsnode *sfs, struct puffs_pathobj *po,
	char *buf, size_t bufsize)
{
	size_t sz;

	assert(!ISADIR(sfs));

	memset(buf, 0, bufsize);
	switch (SYSCTL_TYPE(sfs->sctln.sysctl_flags)) {
	case CTLTYPE_INT: {
		int i;
		sz = sizeof(int);
		if (sysctl(po->po_path, po->po_len, &i, &sz, NULL, 0) == -1)
			break;
		snprintf(buf, bufsize, "%d", i);
		break;
	}
	case CTLTYPE_QUAD: {
		quad_t q;
		sz = sizeof(q);
		if (sysctl(po->po_path, po->po_len, &q, &sz, NULL, 0) == -1)
			break;
		snprintf(buf, bufsize, "%" PRId64, q);
		break;
	}
	case CTLTYPE_STRUCT:
		snprintf(buf, bufsize, "CTLTYPE_STRUCT: implement me and "
		    "score a cookie");
		break;
	case CTLTYPE_STRING: {
		sz = bufsize;
		if (sysctl(po->po_path, po->po_len, buf, &sz, NULL, 0) == -1)
			break;
		break;
	}
	default:
		snprintf(buf, bufsize, "invalid sysctl CTLTYPE");
		break;
	}
}

static int
getlinks(struct sfsnode *sfs, struct puffs_pathobj *po)
{
	struct sysctlnode sn[SFS_NODEPERDIR];
	struct sysctlnode qnode;
	SfsName *sname;
	size_t sl;

	if (!ISADIR(sfs))
		return 1;

	memset(&qnode, 0, sizeof(qnode));
	sl = sizeof(sn);
	qnode.sysctl_flags = SYSCTL_VERSION;
	sname = po->po_path;
	(*sname)[po->po_len] = CTL_QUERY;

	if (sysctl(*sname, po->po_len + 1, sn, &sl,
	    &qnode, sizeof(qnode)) == -1)
		return 0;

	return (sl / sizeof(sn[0])) + 2;
}

static int
getsize(struct sfsnode *sfs, struct puffs_pathobj *po)
{
	char buf[SFS_MAXFILE];

	if (ISADIR(sfs))
		return getlinks(sfs, po) * 16; /* totally arbitrary */

	doprint(sfs, po, buf, sizeof(buf));
	return strlen(buf) + 1;
}

/* fast & loose */
int
sysctlfs_node_lookup(struct puffs_cc *pcc, void *opc, void **newnode,
	enum vtype *newtype, voff_t *newsize, dev_t *newrdev,
	const struct puffs_cn *pcn)
{
	struct puffs_cn *p2cn = __UNCONST(pcn); /* XXX: fix the interface */
	struct sysctlnode sn[SFS_NODEPERDIR];
	struct sysctlnode qnode;
	struct puffs_pathobj po;
	struct puffs_node *pn_dir = opc;
	struct puffs_node *pn_new;
	struct sfsnode *sfs_dir = pn_dir->pn_data, *sfs_new;
	SfsName *sname;
	size_t sl;
	int i;

	assert(ISADIR(sfs_dir));

	/* XXX: not this way, breathes too much stupidity */
	if (pcn->pcn_flags & PUFFS_ISDOTDOT) {
		*newnode = sfs_dir->dotdot;
		*newtype = VDIR;
		*newsize = 0;
		return 0;
	}

	/* get all and compare.. yes, this is a tad silly, but ... */
	memset(&qnode, 0, sizeof(qnode));
	sl = SFS_NODEPERDIR * sizeof(struct sysctlnode);
	qnode.sysctl_flags = SYSCTL_VERSION;
	sname = PCNPATH(pcn);
	(*sname)[PCNPLEN(pcn)] = CTL_QUERY;

	if (sysctl(*sname, PCNPLEN(pcn) + 1, sn, &sl,
	    &qnode, sizeof(qnode)) == -1) {
		return ENOENT;
	}

	for (i = 0; i < sl / sizeof(struct sysctlnode); i++)
		if (strcmp(sn[i].sysctl_name, pcn->pcn_name) == 0)
			goto found;
	return ENOENT;

 found:
	(*sname)[PCNPLEN(pcn)] = sn[i].sysctl_num;
	p2cn->pcn_po_full.po_len++;

	po.po_path = sname;
	po.po_len = PCNPLEN(pcn);
	pn_new = puffs_pn_nodewalk(puffs_cc_getusermount(pcc), pathcmp, &po);
	if (pn_new) {
		sfs_new = pn_new->pn_data;
		goto gotit;
	}

	sfs_new = emalloc(sizeof(struct sfsnode));	
	sfs_new->sctln = sn[i];
	sfs_dir->refcount++;
	sfs_new->dotdot = pn_dir;
	sfs_new->myid = nextid++;

	pn_new = puffs_pn_new(puffs_cc_getusermount(pcc), sfs_new);
	assert(pn_new != NULL);

 gotit:
	*newnode = pn_new;
	if (ISADIR(sfs_new)) {
		*newtype = VDIR;
		*newsize = 0;
	} else {
		*newtype = VREG;
		*newsize = getsize(sfs_new, &p2cn->pcn_po_full);
	}

	return 0;
}

int
sysctlfs_node_getattr(struct puffs_cc *pcc, void *opc, struct vattr *va,
	const struct puffs_cred *pcr, pid_t pid)
{
	struct puffs_node *pn = opc;
	struct sfsnode *sfs = pn->pn_data;

	memset(va, 0, sizeof(struct vattr));

	if (ISADIR(sfs)) {
		va->va_type = VDIR;
		va->va_mode = 0777;
	} else {
		va->va_type = VREG;
		va->va_mode = 0666;
	}
	va->va_nlink = getlinks(sfs, &pn->pn_po);
	va->va_fileid = sfs->myid;
	va->va_size = getsize(sfs, &pn->pn_po);
	va->va_gen = 1;
	va->va_rdev = PUFFS_VNOVAL;
	va->va_blocksize = 512;
	va->va_filerev = 1;

	va->va_atime = va->va_mtime = va->va_ctime = va->va_birthtime = fstime;

	return 0;
}

int
sysctlfs_node_setattr(struct puffs_cc *pcc, void *opc,
	const struct vattr *va, const struct puffs_cred *pcr, pid_t pid)
{

	/* dummy, but required for write */
	return 0;
}

int
sysctlfs_node_readdir(struct puffs_cc *pcc, void *opc,
	struct dirent *dent, const struct puffs_cred *pcr,
	off_t *readoff, size_t *reslen)
{
	struct sysctlnode sn[SFS_NODEPERDIR];
	struct sysctlnode qnode;
	struct puffs_node *pn_dir = opc;
	struct sfsnode *sfs_dir = pn_dir->pn_data;
	SfsName *sname;
	size_t sl;
	enum vtype vt;
	int i;

 again:
	if (*readoff == DENT_DOT || *readoff == DENT_DOTDOT) {
		puffs_gendotdent(&dent, sfs_dir->myid, *readoff, reslen);
		(*readoff)++;
		goto again;
	}

	memset(&qnode, 0, sizeof(qnode));
	sl = SFS_NODEPERDIR * sizeof(struct sysctlnode);
	qnode.sysctl_flags = SYSCTL_VERSION;
	sname = PNPATH(pn_dir);
	(*sname)[PNPLEN(pn_dir)] = CTL_QUERY;

	if (sysctl(*sname, PNPLEN(pn_dir) + 1, sn, &sl,
	    &qnode, sizeof(qnode)) == -1)
		return ENOENT;

	for (i = DENT_ADJ(*readoff);
	    i < sl / sizeof(struct sysctlnode);
	    i++, (*readoff)++) {
		if (SYSCTL_TYPE(sn[i].sysctl_flags) == CTLTYPE_NODE)
			vt = VDIR;
		else
			vt = VREG;
		if (!puffs_nextdent(&dent, sn[i].sysctl_name, nextid++,
		    puffs_vtype2dt(vt), reslen))
			return 0;
	}

	return 0;
}

int
sysctlfs_node_read(struct puffs_cc *pcc, void *opc, uint8_t *buf,
	off_t offset, size_t *resid, const struct puffs_cred *pcr,
	int ioflag)
{
	char localbuf[SFS_MAXFILE];
	struct puffs_node *pn = opc;
	struct sfsnode *sfs = pn->pn_data;
	int xfer;

	if (ISADIR(sfs))
		return EISDIR;

	doprint(sfs, &pn->pn_po, localbuf, sizeof(localbuf));
	xfer = MIN(*resid, strlen(localbuf) - offset);

	if (xfer <= 0)
		return 0;

	memcpy(buf, localbuf + offset, xfer);
	*resid -= xfer;

	if (*resid) {
		buf[xfer] = '\n';
		(*resid)--;
	}

	return 0;
}

int
sysctlfs_node_write(struct puffs_cc *pcc, void *opc, uint8_t *buf,
	off_t offset, size_t *resid, const struct puffs_cred *cred,
	int ioflag)
{
	struct puffs_node *pn = opc;
	struct sfsnode *sfs = pn->pn_data;
	long long ll;
	int i, rv;

	if (ISADIR(sfs))
		return EISDIR;

	if (offset != 0)
		return EINVAL;

	if (ioflag & PUFFS_IO_APPEND)
		return EINVAL;

	switch (SYSCTL_TYPE(sfs->sctln.sysctl_flags)) {
	case CTLTYPE_INT:
		if (sscanf((const char *)buf, "%d", &i) != 1)
			return EINVAL;
		rv = sysctl(PNPATH(pn), PNPLEN(pn), NULL, NULL,
		    &i, sizeof(int));
		break;
	case CTLTYPE_QUAD:
		if (sscanf((const char *)buf, "%lld", &ll) != 1)
			return EINVAL;
		rv =  sysctl(PNPATH(pn), PNPLEN(pn), NULL, NULL,
		    &ll, sizeof(long long));
		break;
	case CTLTYPE_STRING:
		rv = sysctl(PNPATH(pn), PNPLEN(pn), NULL, NULL, buf, *resid);
		break;
	default:
		rv = EINVAL;
		break;
	}

	if (rv)
		return rv;

	*resid = 0;
	return 0;
}
