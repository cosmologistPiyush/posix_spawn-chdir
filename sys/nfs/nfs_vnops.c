/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 *	from: @(#)nfs_vnops.c	7.60 (Berkeley) 5/24/91
 *	$Id: nfs_vnops.c,v 1.26 1994/03/27 09:09:21 cgd Exp $
 */

/*
 * vnode op calls for sun nfs version 2
 */

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/conf.h>
#include <sys/namei.h>
#include <sys/vnode.h>
#include <sys/dir.h>
#include <miscfs/specfs/specdev.h> /* XXX */
#include <miscfs/fifofs/fifo.h> /* XXX */

#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsmount.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>
#include <nfs/nfsiom.h>

/* Defs */
#define	TRUE	1
#define	FALSE	0

/*
 * Global vfs data structures for nfs
 */
struct vnodeops nfsv2_vnodeops = {
	nfs_lookup,		/* lookup */
	nfs_create,		/* create */
	nfs_mknod,		/* mknod */
	nfs_open,		/* open */
	nfs_close,		/* close */
	nfs_access,		/* access */
	nfs_getattr,		/* getattr */
	nfs_setattr,		/* setattr */
	nfs_read,		/* read */
	nfs_write,		/* write */
	nfs_ioctl,		/* ioctl */
	nfs_select,		/* select */
	nfs_mmap,		/* mmap */
	nfs_fsync,		/* fsync */
	nfs_seek,		/* seek */
	nfs_remove,		/* remove */
	nfs_link,		/* link */
	nfs_rename,		/* rename */
	nfs_mkdir,		/* mkdir */
	nfs_rmdir,		/* rmdir */
	nfs_symlink,		/* symlink */
	nfs_readdir,		/* readdir */
	nfs_readlink,		/* readlink */
	nfs_abortop,		/* abortop */
	nfs_inactive,		/* inactive */
	nfs_reclaim,		/* reclaim */
	nfs_lock,		/* lock */
	nfs_unlock,		/* unlock */
	nfs_bmap,		/* bmap */
	nfs_strategy,		/* strategy */
	nfs_print,		/* print */
	nfs_islocked,		/* islocked */
	nfs_advlock,		/* advlock */
};

/*
 * Special device vnode ops
 */
struct vnodeops spec_nfsv2nodeops = {
	spec_lookup,		/* lookup */
	spec_create,		/* create */
	spec_mknod,		/* mknod */
	spec_open,		/* open */
	nfsspec_close,		/* close */
	nfs_access,		/* access */
	nfs_getattr,		/* getattr */
	nfs_setattr,		/* setattr */
	nfsspec_read,		/* read */
	nfsspec_write,		/* write */
	spec_ioctl,		/* ioctl */
	spec_select,		/* select */
	spec_mmap,		/* mmap */
	spec_fsync,		/* fsync */
	spec_seek,		/* seek */
	spec_remove,		/* remove */
	spec_link,		/* link */
	spec_rename,		/* rename */
	spec_mkdir,		/* mkdir */
	spec_rmdir,		/* rmdir */
	spec_symlink,		/* symlink */
	spec_readdir,		/* readdir */
	spec_readlink,		/* readlink */
	spec_abortop,		/* abortop */
	nfs_inactive,		/* inactive */
	nfs_reclaim,		/* reclaim */
	nfs_lock,		/* lock */
	nfs_unlock,		/* unlock */
	spec_bmap,		/* bmap */
	spec_strategy,		/* strategy */
	nfs_print,		/* print */
	nfs_islocked,		/* islocked */
	spec_advlock,		/* advlock */
};

#ifdef FIFO
struct vnodeops fifo_nfsv2nodeops = {
	fifo_lookup,		/* lookup */
	fifo_create,		/* create */
	fifo_mknod,		/* mknod */
	fifo_open,		/* open */
	nfsfifo_close,		/* close */
	nfs_access,		/* access */
	nfs_getattr,		/* getattr */
	nfs_setattr,		/* setattr */
	nfsfifo_read,		/* read */
	nfsfifo_write,		/* write */
	fifo_ioctl,		/* ioctl */
	fifo_select,		/* select */
	fifo_mmap,		/* mmap */
	fifo_fsync,		/* fsync */
	fifo_seek,		/* seek */
	fifo_remove,		/* remove */
	fifo_link,		/* link */
	fifo_rename,		/* rename */
	fifo_mkdir,		/* mkdir */
	fifo_rmdir,		/* rmdir */
	fifo_symlink,		/* symlink */
	fifo_readdir,		/* readdir */
	fifo_readlink,		/* readlink */
	fifo_abortop,		/* abortop */
	nfs_inactive,		/* inactive */
	nfs_reclaim,		/* reclaim */
	nfs_lock,		/* lock */
	nfs_unlock,		/* unlock */
	fifo_bmap,		/* bmap */
	fifo_badop,		/* strategy */
	nfs_print,		/* print */
	nfs_islocked,		/* islocked */
	fifo_advlock,		/* advlock */
};
#endif /* FIFO */

/*
 * Global vars
 */
extern u_long nfs_procids[NFS_NPROCS];
extern u_long nfs_prog, nfs_vers;
extern char nfsiobuf[MAXPHYS+NBPG];
struct buf nfs_bqueue;		/* Queue head for nfsiod's */
struct proc *nfs_iodwant[NFS_MAXASYNCDAEMON];
enum vtype ntov_type[7] = { VNON, VREG, VDIR, VBLK, VCHR, VLNK, VNON };
int nfs_numasync = 0;

/*
 * nfs null call from vfs.
 */
nfs_null(vp, cred, p)
	struct vnode *vp;
	struct ucred *cred;
	struct proc *p;
{
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb;
	
	nfsm_reqhead(nfs_procids[NFSPROC_NULL], cred, 0);
	nfsm_request(vp, NFSPROC_NULL, p, 0);
	nfsm_reqdone;
	return (error);
}

/*
 * nfs access vnode op.
 * Essentially just get vattr and then imitate iaccess()
 */
nfs_access(vp, mode, cred, p)
	struct vnode *vp;
	int mode;
	register struct ucred *cred;
	struct proc *p;
{
	register struct vattr *vap;
	register gid_t *gp;
	struct vattr vattr;
	register int i;
	int error;

	/*
	 * If you're the super-user,
	 * you always get access.
	 */
	if (cred->cr_uid == 0)
		return (0);
	vap = &vattr;
	if (error = nfs_dogetattr(vp, vap, cred, 0, p))
		return (error);
	/*
	 * Access check is based on only one of owner, group, public.
	 * If not owner, then check group. If not a member of the
	 * group, then check public access.
	 */
	if (cred->cr_uid != vap->va_uid) {
		mode >>= 3;
		gp = cred->cr_groups;
		for (i = 0; i < cred->cr_ngroups; i++, gp++)
			if (vap->va_gid == *gp)
				goto found;
		mode >>= 3;
found:
		;
	}
	return ((vap->va_mode & mode) == mode ? 0 : EACCES);
}

/*
 * nfs open vnode op
 * Just check to see if the type is ok
 */
/* ARGSUSED */
nfs_open(vp, mode, cred, p)
	struct vnode *vp;
	int mode;
	struct ucred *cred;
	struct proc *p;
{
	register enum vtype vtyp;

	vtyp = vp->v_type;
	if (vtyp == VREG || vtyp == VDIR || vtyp == VLNK)
		return (0);
	else
		return (EACCES);
}

/*
 * nfs close vnode op
 * For reg files, invalidate any buffer cache entries.
 */
/* ARGSUSED */
nfs_close(vp, fflags, cred, p)
	register struct vnode *vp;
	int fflags;
	struct ucred *cred;
	struct proc *p;
{
	register struct nfsnode *np = VTONFS(vp);
	int error = 0;

	if (vp->v_type == VREG && (np->n_flag & NMODIFIED)) {
		nfs_lock(vp);
		np->n_flag &= ~NMODIFIED;
		vinvalbuf(vp, TRUE);
		np->n_attrstamp = 0;
		if (np->n_flag & NWRITEERR) {
			np->n_flag &= ~NWRITEERR;
			error = np->n_error;
		}
		nfs_unlock(vp);
	}
	return (error);
}

/*
 * nfs getattr call from vfs.
 */
nfs_getattr(vp, vap, cred, p)
	register struct vnode *vp;
	struct vattr *vap;
	struct ucred *cred;
	struct proc *p;
{
	return (nfs_dogetattr(vp, vap, cred, 0, p));
}

nfs_dogetattr(vp, vap, cred, tryhard, p)
	register struct vnode *vp;
	struct vattr *vap;
	struct ucred *cred;
	int tryhard;
	struct proc *p;
{
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np;
	
	/* First look in the cache.. */
	/* cred == NOCRED when we are called by mountroot */
	if (cred != NOCRED && nfs_getattrcache(vp, vap) == 0)
		return (0);

	np = VTONFS(vp);
	if (np->n_delayed_atime.tv_sec != VNOVAL ||
	    np->n_delayed_mtime.tv_sec != VNOVAL) {
		VATTR_NULL(vap);
		error = nfs_setattr(vp, vap, cred, p);
		bcopy((caddr_t)&np->n_vattr, (caddr_t)vap, sizeof(*vap));
		if ((np->n_flag & NMODIFIED) && (np->n_size > vap->va_size))
			vap->va_size = np->n_size;
		return error;
	}
	nfsstats.rpccnt[NFSPROC_GETATTR]++;
	nfsm_reqhead(nfs_procids[NFSPROC_GETATTR], cred, NFSX_FH);
	nfsm_fhtom(vp);
	nfsm_request(vp, NFSPROC_GETATTR, p, tryhard);
	nfsm_loadattr(vp, vap);
	nfsm_reqdone;
	return (error);
}

/*
 * nfs setattr call.
 */
nfs_setattr(vp, vap, cred, p)
	register struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
	struct proc *p;
{
	register struct nfsv2_sattr *sp;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np = VTONFS(vp);

	nfsstats.rpccnt[NFSPROC_SETATTR]++;
	nfsm_reqhead(nfs_procids[NFSPROC_SETATTR], cred, NFSX_FH+NFSX_SATTR);
	nfsm_fhtom(vp);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	if (vap->va_mode == (u_short)VNOVAL)
		sp->sa_mode = VNOVAL;
	else
		sp->sa_mode = vtonfs_mode(vp->v_type, vap->va_mode);
	if (vap->va_uid == VNOVAL)
		sp->sa_uid = VNOVAL;
	else
		sp->sa_uid = txdr_unsigned(vap->va_uid);
	if (vap->va_gid == VNOVAL)
		sp->sa_gid = VNOVAL;
	else
		sp->sa_gid = txdr_unsigned(vap->va_gid);
	sp->sa_size = txdr_unsigned(vap->va_size);
	/* jfw@ksr.com 6/2/93 */
#if 0 /* bad assumption; Suns (at least) make full use of usec field */
	sp->sa_atime.tv_sec = txdr_unsigned(vap->va_atime.tv_sec);
	sp->sa_atime.tv_usec = txdr_unsigned(vap->va_flags);
#else
	if (vap->va_atime.tv_sec == VNOVAL &&
	    np->n_delayed_atime.tv_sec != VNOVAL) {
		txdr_time(&np->n_delayed_atime, &sp->sa_atime);
	} else {
		txdr_time(&vap->va_atime, &sp->sa_atime);
	}
#endif
	if (vap->va_mtime.tv_sec == VNOVAL &&
	    np->n_delayed_mtime.tv_sec != VNOVAL) {
		txdr_time(&np->n_delayed_mtime, &sp->sa_mtime);
	} else {
		txdr_time(&vap->va_mtime, &sp->sa_mtime);
	}
	np->n_delayed_atime.tv_sec = VNOVAL;
	np->n_delayed_mtime.tv_sec = VNOVAL;

	if (vap->va_size != VNOVAL || vap->va_mtime.tv_sec != VNOVAL ||
	    vap->va_atime.tv_sec != VNOVAL) {
		if (np->n_flag & NMODIFIED) {
			np->n_flag &= ~NMODIFIED;
			if (vap->va_size == 0)
				vinvalbuf(vp, FALSE);
			else
				vinvalbuf(vp, TRUE);
			np->n_attrstamp = 0;
		}
	}
	nfsm_request(vp, NFSPROC_SETATTR, p, 1);
	nfsm_loadattr(vp, (struct vattr *)0);
	/* should we fill in any vap fields ?? */
	nfsm_reqdone;
	return (error);
}

/*
 * nfs lookup call, one step at a time...
 * First look in cache
 * If not found, unlock the directory nfsnode and do the rpc
 */
nfs_lookup(vp, ndp, p)
	register struct vnode *vp;
	register struct nameidata *ndp;
	struct proc *p;
{
	register struct vnode *vdp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *newvp;
	long len;
	nfsv2fh_t *fhp;
	struct nfsnode *np;
	int lockparent, wantparent, flag, error = 0;

	ndp->ni_dvp = vp;
	ndp->ni_vp = NULL;
	if (vp->v_type != VDIR)
		return (ENOTDIR);
	lockparent = ndp->ni_nameiop & LOCKPARENT;
	flag = ndp->ni_nameiop & OPMASK;
	wantparent = ndp->ni_nameiop & (LOCKPARENT|WANTPARENT);
	if ((error = cache_lookup(ndp)) && error != ENOENT) {
		struct vattr vattr;
		int vpid;

		vdp = ndp->ni_vp;
		vpid = vdp->v_id;
		/*
		 * See the comment starting `Step through' in ufs/ufs_lookup.c
		 * for an explanation of the locking protocol
		 */
		if (vp == vdp) {
			VREF(vdp);
			error = 0;
		} else if (ndp->ni_isdotdot) {
			nfs_unlock(vp);
			error = vget(vdp);
			if (!error && lockparent && *ndp->ni_next == '\0')
				nfs_lock(vp);
		} else {
			error = vget(vdp);
			if (!lockparent || error || *ndp->ni_next != '\0')
				nfs_unlock(vp);
		}
		if (!error) {
			if (vpid == vdp->v_id) {
			   if (!nfs_dogetattr(vdp, &vattr, ndp->ni_cred, 0, p)&&
			       vattr.va_ctime.tv_sec == VTONFS(vdp)->n_ctime) {
				nfsstats.lookupcache_hits++;
				if (flag != LOOKUP && *ndp->ni_next == 0)
					ndp->ni_nameiop |= SAVENAME;
				return (0);
			   }
			   cache_purge(vdp);
			}
			nfs_nput(vdp);
			if (lockparent && vdp != vp && *ndp->ni_next == '\0')
				nfs_unlock(vp);
		}
		ndp->ni_vp = NULLVP;
	} else
		nfs_unlock(vp);
	error = 0;
	nfsstats.lookupcache_misses++;
	nfsstats.rpccnt[NFSPROC_LOOKUP]++;
	len = ndp->ni_namelen;
	nfsm_reqhead(nfs_procids[NFSPROC_LOOKUP], ndp->ni_cred, NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len));
	nfsm_fhtom(vp);
	nfsm_strtom(ndp->ni_ptr, len, NFS_MAXNAMLEN);
	nfsm_request(vp, NFSPROC_LOOKUP, p, 0);
nfsmout:
	if (error) {
		if ((flag == CREATE || flag == RENAME) &&
		    *ndp->ni_next == '\0' && error == ENOENT)
			error = EJUSTRETURN;
		if (lockparent || (flag != CREATE && flag != RENAME) ||
		    *ndp->ni_next != 0)
			nfs_lock(vp);
		if (flag != LOOKUP && *ndp->ni_next == 0)
			ndp->ni_nameiop |= SAVENAME;
		return (error);
	}
	nfsm_disect(fhp,nfsv2fh_t *,NFSX_FH);

	/*
	 * Handle DELETE and RENAME cases...
	 */
	if (flag == DELETE && *ndp->ni_next == 0) {
		if (!bcmp(VTONFS(vp)->n_fh.fh_bytes, (caddr_t)fhp, NFSX_FH)) {
			VREF(vp);
			newvp = vp;
			np = VTONFS(vp);
		} else {
			if (error = nfs_nget(vp->v_mount, fhp, &np)) {
				nfs_lock(vp);
				m_freem(mrep);
				return (error);
			}
			newvp = NFSTOV(np);
		}
		if (error =
		    nfs_loadattrcache(&newvp, &md, &dpos, (struct vattr *)0)) {
			nfs_lock(vp);
			if (newvp != vp)
				nfs_nput(newvp);
			else
				vrele(vp);
			m_freem(mrep);
			return (error);
		}
		ndp->ni_vp = newvp;
		if (lockparent || vp == newvp)
			nfs_lock(vp);
		m_freem(mrep);
		ndp->ni_nameiop |= SAVENAME;
		return (0);
	}

	if (flag == RENAME && wantparent && *ndp->ni_next == 0) {
		if (!bcmp(VTONFS(vp)->n_fh.fh_bytes, (caddr_t)fhp, NFSX_FH)) {
			nfs_lock(vp);
			m_freem(mrep);
			return (EISDIR);
		}
		if (error = nfs_nget(vp->v_mount, fhp, &np)) {
			nfs_lock(vp);
			m_freem(mrep);
			return (error);
		}
		newvp = NFSTOV(np);
		if (error =
		    nfs_loadattrcache(&newvp, &md, &dpos, (struct vattr *)0)) {
			nfs_lock(vp);
			nfs_nput(newvp);
			m_freem(mrep);
			return (error);
		}
		ndp->ni_vp = newvp;
		if (lockparent)
			nfs_lock(vp);
		m_freem(mrep);
		ndp->ni_nameiop |= SAVENAME;
		return (0);
	}

	if (!bcmp(VTONFS(vp)->n_fh.fh_bytes, (caddr_t)fhp, NFSX_FH)) {
		VREF(vp);
		newvp = vp;
		np = VTONFS(vp);
	} else if (ndp->ni_isdotdot) {
		if (error = nfs_nget(vp->v_mount, fhp, &np)) {
			nfs_lock(vp);
			m_freem(mrep);
			return (error);
		}
		newvp = NFSTOV(np);
	} else {
		if (error = nfs_nget(vp->v_mount, fhp, &np)) {
			nfs_lock(vp);
			m_freem(mrep);
			return (error);
		}
		newvp = NFSTOV(np);
	}
	if (error = nfs_loadattrcache(&newvp, &md, &dpos, (struct vattr *)0)) {
		nfs_lock(vp);
		if (newvp != vp)
			nfs_nput(newvp);
		else
			vrele(vp);
		m_freem(mrep);
		return (error);
	}
	m_freem(mrep);

	if (vp == newvp || (lockparent && *ndp->ni_next == '\0'))
		nfs_lock(vp);
	ndp->ni_vp = newvp;
	if (flag != LOOKUP && *ndp->ni_next == 0)
		ndp->ni_nameiop |= SAVENAME;
	if (error == 0 && ndp->ni_makeentry) {
		np->n_ctime = np->n_vattr.va_ctime.tv_sec;
		cache_enter(ndp);
	}
	return (error);
}

/*
 * nfs read call.
 * Just call nfs_bioread() to do the work.
 */
nfs_read(vp, uiop, ioflag, cred)
	register struct vnode *vp;
	struct uio *uiop;
	int ioflag;
	struct ucred *cred;
{
	if (vp->v_type != VREG)
		return (EPERM);
	return (nfs_bioread(vp, uiop, ioflag, cred));
}

/*
 * nfs readlink call
 */
nfs_readlink(vp, uiop, cred)
	struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	if (vp->v_type != VLNK)
		return (EPERM);
	return (nfs_bioread(vp, uiop, 0, cred));
}

/*
 * Do a readlink rpc.
 * Called by nfs_doio() from below the buffer cache.
 */
nfs_readlinkrpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	long len;

	nfsstats.rpccnt[NFSPROC_READLINK]++;
	nfsm_reqhead(nfs_procids[NFSPROC_READLINK], cred, NFSX_FH);
	nfsm_fhtom(vp);
	nfsm_request(vp, NFSPROC_READLINK, uiop->uio_procp, 0);
	nfsm_strsiz(len, NFS_MAXPATHLEN);
	nfsm_mtouio(uiop, len);
	nfsm_reqdone;
	return (error);
}

/*
 * nfs read rpc call
 * Ditto above
 */
nfs_readrpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	long len, retlen, tsiz;

	nmp = VFSTONFS(vp->v_mount);
	tsiz = uiop->uio_resid;
	while (tsiz > 0) {
		nfsstats.rpccnt[NFSPROC_READ]++;
		len = (tsiz > nmp->nm_rsize) ? nmp->nm_rsize : tsiz;
		nfsm_reqhead(nfs_procids[NFSPROC_READ], cred, NFSX_FH+NFSX_UNSIGNED*3);
		nfsm_fhtom(vp);
		nfsm_build(tl, u_long *, NFSX_UNSIGNED*3);
		*tl++ = txdr_unsigned(uiop->uio_offset);
		*tl++ = txdr_unsigned(len);
		*tl = 0;
		nfsm_request(vp, NFSPROC_READ, uiop->uio_procp, 1);
		nfsm_loadattr(vp, (struct vattr *)0);
		nfsm_strsiz(retlen, nmp->nm_rsize);
		nfsm_mtouio(uiop, retlen);
		m_freem(mrep);
		if (retlen < len)
			tsiz = 0;
		else
			tsiz -= len;
	}
nfsmout:
	return (error);
}

/*
 * nfs write call
 */
nfs_writerpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	long len, tsiz;

	nmp = VFSTONFS(vp->v_mount);
	tsiz = uiop->uio_resid;
	while (tsiz > 0) {
		nfsstats.rpccnt[NFSPROC_WRITE]++;
		len = (tsiz > nmp->nm_wsize) ? nmp->nm_wsize : tsiz;
		nfsm_reqhead(nfs_procids[NFSPROC_WRITE], cred,
			NFSX_FH+NFSX_UNSIGNED*4);
		nfsm_fhtom(vp);
		nfsm_build(tl, u_long *, NFSX_UNSIGNED*4);
		*(tl+1) = txdr_unsigned(uiop->uio_offset);
		*(tl+3) = txdr_unsigned(len);
		nfsm_uiotom(uiop, len);
		nfsm_request(vp, NFSPROC_WRITE, uiop->uio_procp, 1);
		nfsm_loadattr(vp, (struct vattr *)0);
		m_freem(mrep);
		tsiz -= len;
	}
nfsmout:
	return (error);
}

/*
 * nfs mknod call
 * This is a kludge. Use a create rpc but with the IFMT bits of the mode
 * set to specify the file type and the size field for rdev.
 */
/* ARGSUSED */
nfs_mknod(ndp, vap, cred, p)
	struct nameidata *ndp;
	struct ucred *cred;
	register struct vattr *vap;
	struct proc *p;
{
	register struct nfsv2_sattr *sp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	u_long rdev;

	if (vap->va_type == VCHR || vap->va_type == VBLK)
		rdev = txdr_unsigned(vap->va_rdev);
#ifdef FIFO
	else if (vap->va_type == VFIFO)
		rdev = 0xffffffff;
#endif /* FIFO */
	else {
		VOP_ABORTOP(ndp);
		vput(ndp->ni_dvp);
		return (EOPNOTSUPP);
	}
	nfsstats.rpccnt[NFSPROC_CREATE]++;
	nfsm_reqhead(nfs_procids[NFSPROC_CREATE], ndp->ni_cred,
	  NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_namelen)+NFSX_SATTR);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_ptr, ndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(vap->va_type, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = rdev;
	/* or should these be VNOVAL ?? */
	txdr_time(&vap->va_atime, &sp->sa_atime);
	txdr_time(&vap->va_mtime, &sp->sa_mtime);
	nfsm_request(ndp->ni_dvp, NFSPROC_CREATE, p, 1);
	nfsm_reqdone;
	FREE(ndp->ni_pnbuf, M_NAMEI);
	VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
	nfs_nput(ndp->ni_dvp);
	return (error);
}

/*
 * nfs file create call
 */
nfs_create(ndp, vap, p)
	register struct nameidata *ndp;
	register struct vattr *vap;
	struct proc *p;
{
	register struct nfsv2_sattr *sp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	nfsstats.rpccnt[NFSPROC_CREATE]++;
	nfsm_reqhead(nfs_procids[NFSPROC_CREATE], ndp->ni_cred,
	  NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_namelen)+NFSX_SATTR);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_ptr, ndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(vap->va_type, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = txdr_unsigned(0);
	/* or should these be VNOVAL ?? */
	txdr_time(&vap->va_atime, &sp->sa_atime);
	txdr_time(&vap->va_mtime, &sp->sa_mtime);
	nfsm_request(ndp->ni_dvp, NFSPROC_CREATE, p, 1);
	nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp);
	nfsm_reqdone;
	FREE(ndp->ni_pnbuf, M_NAMEI);
	VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
	nfs_nput(ndp->ni_dvp);
	return (error);
}

/*
 * nfs file remove call
 * To try and make nfs semantics closer to ufs semantics, a file that has
 * other processes using the vnode is renamed instead of removed and then
 * removed later on the last close.
 * - If v_usecount > 1
 *	  If a rename is not already in the works
 *	     call nfs_sillyrename() to set it up
 *	  else
 *	     If link_count > 0
 *		do the remove rpc
 *     else
 *	  do the remove rpc
 */
nfs_remove(ndp, p)
	register struct nameidata *ndp;
	struct proc *p;
{
	register struct vnode *vp = ndp->ni_vp;
	register struct nfsnode *np = VTONFS(ndp->ni_vp);
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	if (vp->v_usecount > 1) {
		struct vattr va;
		if (!np->n_sillyrename)
			error = nfs_sillyrename(ndp, p);
		else if (nfs_getattr(vp, &va, ndp->ni_cred, p) == 0 &&
							va.va_nlink > 1)
			goto do_nfsremove;
	} else {
do_nfsremove:
		nfsstats.rpccnt[NFSPROC_REMOVE]++;
		nfsm_reqhead(nfs_procids[NFSPROC_REMOVE], ndp->ni_cred,
			NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_namelen));
		nfsm_fhtom(ndp->ni_dvp);
		nfsm_strtom(ndp->ni_ptr, ndp->ni_namelen, NFS_MAXNAMLEN);
		nfsm_request(ndp->ni_dvp, NFSPROC_REMOVE, p, 1);
		nfsm_reqdone;
		FREE(ndp->ni_pnbuf, M_NAMEI);
		VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
		/*
		 * Kludge City: If the first reply to the remove rpc is lost..
		 *   the reply to the retransmitted request will be ENOENT
		 *   since the file was in fact removed
		 *   Therefore, we cheat and return success.
		 */
		if (error == ENOENT)
			error = 0;
	}
	np->n_attrstamp = 0;
	if (ndp->ni_dvp == vp)
		vrele(vp);
	else
		nfs_nput(ndp->ni_dvp);
	nfs_nput(vp);
	return (error);
}

/*
 * nfs file remove rpc called from nfs_inactive
 */
nfs_removeit(sp, p)
	register struct sillyrename *sp;
	struct proc *p;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	nfsstats.rpccnt[NFSPROC_REMOVE]++;
	nfsm_reqhead(nfs_procids[NFSPROC_REMOVE], sp->s_cred,
		NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(sp->s_namlen));
	nfsm_fhtom(sp->s_dvp);
	nfsm_strtom(sp->s_name, sp->s_namlen, NFS_MAXNAMLEN);
	nfsm_request(sp->s_dvp, NFSPROC_REMOVE, p, 1);
	nfsm_reqdone;
	VTONFS(sp->s_dvp)->n_flag |= NMODIFIED;
	return (error);
}

/*
 * nfs file rename call
 */
nfs_rename(sndp, tndp, p)
	register struct nameidata *sndp, *tndp;
	struct proc *p;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *fvp, *tdvp, *tvp;

	/* Check for cross-device rename */
	fvp = sndp->ni_vp;
	tdvp = tndp->ni_dvp;
	tvp = tndp->ni_vp;
	if ((fvp->v_mount != tdvp->v_mount) ||
	    (tvp && (fvp->v_mount != tvp->v_mount))) {
		error = EXDEV;
		goto out;
	}

	nfsstats.rpccnt[NFSPROC_RENAME]++;
	nfsm_reqhead(nfs_procids[NFSPROC_RENAME], tndp->ni_cred,
		(NFSX_FH+NFSX_UNSIGNED)*2+nfsm_rndup(sndp->ni_namelen) +
		nfsm_rndup(tndp->ni_namelen)); /* or sndp->ni_cred?*/
	nfsm_fhtom(sndp->ni_dvp);
	nfsm_strtom(sndp->ni_ptr, sndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_fhtom(tndp->ni_dvp);
	nfsm_strtom(tndp->ni_ptr, tndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_request(sndp->ni_dvp, NFSPROC_RENAME, p, 1);
	nfsm_reqdone;
	VTONFS(sndp->ni_dvp)->n_flag |= NMODIFIED;
	VTONFS(tndp->ni_dvp)->n_flag |= NMODIFIED;
	if (sndp->ni_vp->v_type == VDIR) {
		if (tndp->ni_vp != NULL && tndp->ni_vp->v_type == VDIR)
			cache_purge(tndp->ni_dvp);
		cache_purge(sndp->ni_dvp);
	}

out:
	if (tndp->ni_dvp == tndp->ni_vp)
		vrele(tndp->ni_dvp);
	else
		vput(tndp->ni_dvp);
	if (tndp->ni_vp)
		vput(tndp->ni_vp);
	vrele(sndp->ni_dvp);
	vrele(sndp->ni_vp);
	/*
	 * Kludge: Map ENOENT => 0 assuming that it is a reply to a retry.
	 */
	if (error == ENOENT)
		error = 0;
	return (error);
}

/*
 * nfs file rename rpc called from nfs_remove() above
 */
nfs_renameit(sndp, sp, p)
	register struct nameidata *sndp;
	register struct sillyrename *sp;
	struct proc *p;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	nfsstats.rpccnt[NFSPROC_RENAME]++;
	nfsm_reqhead(nfs_procids[NFSPROC_RENAME], sp->s_cred,
		(NFSX_FH+NFSX_UNSIGNED)*2+nfsm_rndup(sndp->ni_namelen) +
		nfsm_rndup(sp->s_namlen)); /* or sndp->ni_cred?*/
	nfsm_fhtom(sndp->ni_dvp);
	nfsm_strtom(sndp->ni_ptr, sndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_fhtom(sp->s_dvp);
	nfsm_strtom(sp->s_name, sp->s_namlen, NFS_MAXNAMLEN);
	nfsm_request(sndp->ni_dvp, NFSPROC_RENAME, p, 1);
	nfsm_reqdone;
	FREE(sndp->ni_pnbuf, M_NAMEI);
	VTONFS(sndp->ni_dvp)->n_flag |= NMODIFIED;
	VTONFS(sp->s_dvp)->n_flag |= NMODIFIED;
	return (error);
}

/*
 * nfs hard link create call
 */
nfs_link(vp, ndp, p)
	register struct vnode *vp;
	register struct nameidata *ndp;
	struct proc *p;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	if (vp->v_mount != ndp->ni_dvp->v_mount) {
		VOP_ABORTOP(ndp);
		if (ndp->ni_dvp == ndp->ni_vp)
			vrele(ndp->ni_dvp);
		else
			vput(ndp->ni_dvp);
		if (ndp->ni_vp)
			vrele(ndp->ni_vp);
		return (EXDEV);
	}
	if (ndp->ni_dvp != vp)
		nfs_lock(vp);
	nfsstats.rpccnt[NFSPROC_LINK]++;
	nfsm_reqhead(nfs_procids[NFSPROC_LINK], ndp->ni_cred,
		NFSX_FH*2+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_namelen));
	nfsm_fhtom(vp);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_ptr, ndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_request(vp, NFSPROC_LINK, p, 1);
	nfsm_reqdone;
	FREE(ndp->ni_pnbuf, M_NAMEI);
	VTONFS(vp)->n_attrstamp = 0;
	VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
	if (ndp->ni_dvp != vp)
		nfs_unlock(vp);
	nfs_nput(ndp->ni_dvp);
	/*
	 * Kludge: Map EEXIST => 0 assuming that it is a reply to a retry.
	 */
	if (error == EEXIST)
		error = 0;
	return (error);
}

/*
 * nfs symbolic link create call
 */
nfs_symlink(ndp, vap, nm, p)
	struct nameidata *ndp;
	struct vattr *vap;
	char *nm;		/* is this the path ?? */
	struct proc *p;
{
	register struct nfsv2_sattr *sp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	nfsstats.rpccnt[NFSPROC_SYMLINK]++;
	nfsm_reqhead(nfs_procids[NFSPROC_SYMLINK], ndp->ni_cred,
	NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_namelen)+NFSX_UNSIGNED);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_ptr, ndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_strtom(nm, strlen(nm), NFS_MAXPATHLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(VLNK, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = txdr_unsigned(VNOVAL);
	txdr_time(&vap->va_atime, &sp->sa_atime);	/* or VNOVAL ?? */
	txdr_time(&vap->va_mtime, &sp->sa_mtime);	/* or VNOVAL ?? */
	nfsm_request(ndp->ni_dvp, NFSPROC_SYMLINK, p, 1);
	nfsm_reqdone;
	FREE(ndp->ni_pnbuf, M_NAMEI);
	VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
	nfs_nput(ndp->ni_dvp);
	/*
	 * Kludge: Map EEXIST => 0 assuming that it is a reply to a retry.
	 */
	if (error == EEXIST)
		error = 0;
	return (error);
}

/*
 * nfs make dir call
 */
nfs_mkdir(ndp, vap, p)
	register struct nameidata *ndp;
	struct vattr *vap;
	struct proc *p;
{
	register struct nfsv2_sattr *sp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	register int len;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0, firsttry = 1;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	len = ndp->ni_namelen;
	nfsstats.rpccnt[NFSPROC_MKDIR]++;
	nfsm_reqhead(nfs_procids[NFSPROC_MKDIR], ndp->ni_cred,
	  NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len)+NFSX_SATTR);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_ptr, len, NFS_MAXNAMLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(VDIR, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = txdr_unsigned(VNOVAL);
	txdr_time(&vap->va_atime, &sp->sa_atime);	/* or VNOVAL ?? */
	txdr_time(&vap->va_mtime, &sp->sa_mtime);	/* or VNOVAL ?? */
	nfsm_request(ndp->ni_dvp, NFSPROC_MKDIR, p, 1);
	nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp);
	nfsm_reqdone;
	VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
	/*
	 * Kludge: Map EEXIST => 0 assuming that you have a reply to a retry
	 * if we can succeed in looking up the directory.
	 * "firsttry" is necessary since the macros may "goto nfsmout" which
	 * is above the if on errors. (Ugh)
	 */
	if (error == EEXIST && firsttry) {
		firsttry = 0;
		error = 0;
		nfsstats.rpccnt[NFSPROC_LOOKUP]++;
		ndp->ni_vp = NULL;
		nfsm_reqhead(nfs_procids[NFSPROC_LOOKUP], ndp->ni_cred,
		    NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len));
		nfsm_fhtom(ndp->ni_dvp);
		nfsm_strtom(ndp->ni_ptr, len, NFS_MAXNAMLEN);
		nfsm_request(ndp->ni_dvp, NFSPROC_LOOKUP, p, 1);
		nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp);
		if (ndp->ni_vp->v_type != VDIR) {
			vput(ndp->ni_vp);
			error = EEXIST;
		}
		m_freem(mrep);
	}
	FREE(ndp->ni_pnbuf, M_NAMEI);
	nfs_nput(ndp->ni_dvp);
	return (error);
}

/*
 * nfs remove directory call
 */
nfs_rmdir(ndp, p)
	register struct nameidata *ndp;
	struct proc *p;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	if (ndp->ni_dvp == ndp->ni_vp) {
		vrele(ndp->ni_dvp);
		nfs_nput(ndp->ni_dvp);
		return (EINVAL);
	}
	nfsstats.rpccnt[NFSPROC_RMDIR]++;
	nfsm_reqhead(nfs_procids[NFSPROC_RMDIR], ndp->ni_cred,
		NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_namelen));
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_ptr, ndp->ni_namelen, NFS_MAXNAMLEN);
	nfsm_request(ndp->ni_dvp, NFSPROC_RMDIR, p, 1);
	nfsm_reqdone;
	FREE(ndp->ni_pnbuf, M_NAMEI);
	VTONFS(ndp->ni_dvp)->n_flag |= NMODIFIED;
	cache_purge(ndp->ni_dvp);
	cache_purge(ndp->ni_vp);
	nfs_nput(ndp->ni_vp);
	nfs_nput(ndp->ni_dvp);
	/*
	 * Kludge: Map ENOENT => 0 assuming that you have a reply to a retry.
	 */
	if (error == ENOENT)
		error = 0;
	return (error);
}

/*
 * nfs readdir call
 * Although cookie is defined as opaque, I translate it to/from net byte
 * order so that it looks more sensible. This appears consistent with the
 * Ultrix implementation of NFS.
 */
nfs_readdir(vp, uiop, cred, eofflagp, cookies, ncookies)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
	int *eofflagp;
	u_int *cookies;
	int ncookies;
{
	register struct nfsnode *np = VTONFS(vp);
	int tresid, error;
	struct vattr vattr;
	
	/*
	 * Since NFS mounting isn't propagated,
	 * we don't need to handle cookies here!
	 */
	if (cookies)
		panic("nfs_readdir");
	
	if (vp->v_type != VDIR)
		return (EPERM);
	/*
	 * First, check for hit on the EOF offset cache
	 */
	if (uiop->uio_offset != 0 && uiop->uio_offset == np->n_direofoffset &&
	    (np->n_flag & NMODIFIED) == 0 &&
	    nfs_dogetattr(vp, &vattr, cred, 0, uiop->uio_procp) == 0 &&
	    np->n_mtime == vattr.va_mtime.tv_sec) {
		*eofflagp = 1;
		nfsstats.direofcache_hits++;
		return (0);
	}

	/*
	 * Call nfs_bioread() to do the real work.
	 */
	tresid = uiop->uio_resid;
	error = nfs_bioread(vp, uiop, 0, cred);

	if (!error && uiop->uio_resid == tresid) {
		*eofflagp = 1;
		nfsstats.direofcache_misses++;
	} else
		*eofflagp = 0;
	return (error);
}

/*
 * Readdir rpc call.
 * Called from below the buffer cache by nfs_doio().
 */
nfs_readdirrpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register long len;
	register struct dirent *dp;
	register u_long *tl;
	register caddr_t cp;
	register long t1;
	long tlen, lastlen;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *md2;
	caddr_t dpos2;
	int siz;
	int more_dirs = 1;
	off_t off, savoff;
	struct dirent *savdp;
	struct nfsmount *nmp;
	struct nfsnode *np = VTONFS(vp);
	long tresid;

	nmp = VFSTONFS(vp->v_mount);
	tresid = uiop->uio_resid;
	/*
	 * Loop around doing readdir rpc's of size uio_resid or nm_rsize,
	 * whichever is smaller, truncated to a multiple of NFS_DIRBLKSIZ.
	 * The stopping criteria is EOF or buffer full.
	 */
	while (more_dirs && uiop->uio_resid >= NFS_DIRBLKSIZ) {
		nfsstats.rpccnt[NFSPROC_READDIR]++;
		nfsm_reqhead(nfs_procids[NFSPROC_READDIR], cred, xid);
		nfsm_fhtom(vp);
		nfsm_build(tl, u_long *, 2*NFSX_UNSIGNED);
		*tl++ = txdr_unsigned(uiop->uio_offset);
		*tl = txdr_unsigned(((uiop->uio_resid > nmp->nm_rsize) ?
			nmp->nm_rsize : uiop->uio_resid) & ~(NFS_DIRBLKSIZ-1));
		nfsm_request(vp, NFSPROC_READDIR, uiop->uio_procp, 0);
		siz = 0;
		nfsm_disect(tl, u_long *, NFSX_UNSIGNED);
		more_dirs = fxdr_unsigned(int, *tl);
	
		/* Save the position so that we can do nfsm_mtouio() later */
		dpos2 = dpos;
		md2 = md;
	
		/* loop thru the dir entries, doctoring them to 4bsd form */
		off = uiop->uio_offset;
#ifdef lint
		dp = (struct dirent *)0;
#endif /* lint */
		while (more_dirs && siz < uiop->uio_resid) {
			savoff = off;		/* Hold onto offset and dp */
			savdp = dp;
			nfsm_disecton(tl, u_long *, 2*NFSX_UNSIGNED);
			dp = (struct dirent *)tl;
			dp->d_fileno = fxdr_unsigned(u_long, *tl++);
			len = fxdr_unsigned(int, *tl);
			if (len <= 0 || len > NFS_MAXNAMLEN) {
				error = EBADRPC;
				m_freem(mrep);
				goto nfsmout;
			}
			dp->d_namlen = (u_short)len;
			nfsm_adv(len);		/* Point past name */
			tlen = nfsm_rndup(len);
			/*
			 * This should not be necessary, but some servers have
			 * broken XDR such that these bytes are not null filled.
			 */
			if (tlen != len) {
				*dpos = '\0';	/* Null-terminate */
				nfsm_adv(tlen - len);
				len = tlen;
			}
			nfsm_disecton(tl, u_long *, 2*NFSX_UNSIGNED);
			off = fxdr_unsigned(off_t, *tl);
			*tl++ = 0;	/* Ensures null termination of name */
			more_dirs = fxdr_unsigned(int, *tl);
			dp->d_reclen = len+4*NFSX_UNSIGNED;
			siz += dp->d_reclen;
		}
		/*
		 * If at end of rpc data, get the eof boolean
		 */
		if (!more_dirs) {
			nfsm_disecton(tl, u_long *, NFSX_UNSIGNED);
			more_dirs = (fxdr_unsigned(int, *tl) == 0);

			/*
			 * If at EOF, cache directory offset
			 */
			if (!more_dirs)
				np->n_direofoffset = off;
		}
		/*
		 * If there is too much to fit in the data buffer, use savoff and
		 * savdp to trim off the last record.
		 * --> we are not at eof
		 */
		if (siz > uiop->uio_resid) {
			off = savoff;
			siz -= dp->d_reclen;
			dp = savdp;
			more_dirs = 0;	/* Paranoia */
		}
		if (siz > 0) {
			lastlen = dp->d_reclen;
			md = md2;
			dpos = dpos2;
			nfsm_mtouio(uiop, siz);
			uiop->uio_offset = off;
		} else
			more_dirs = 0;	/* Ugh, never happens, but in case.. */
		m_freem(mrep);
	}
	/*
	 * Fill last record, iff any, out to a multiple of NFS_DIRBLKSIZ
	 * by increasing d_reclen for the last record.
	 */
	if (uiop->uio_resid < tresid) {
		len = uiop->uio_resid & (NFS_DIRBLKSIZ - 1);
		if (len > 0) {
			dp = (struct dirent *)
				(uiop->uio_iov->iov_base - lastlen);
			dp->d_reclen += len;
			uiop->uio_iov->iov_base += len;
			uiop->uio_iov->iov_len -= len;
			uiop->uio_resid -= len;
		}
	}
nfsmout:
	return (error);
}

static char hextoasc[] = "0123456789abcdef";

/*
 * Silly rename. To make the NFS filesystem that is stateless look a little
 * more like the "ufs" a remove of an active vnode is translated to a rename
 * to a funny looking filename that is removed by nfs_inactive on the
 * nfsnode. There is the potential for another process on a different client
 * to create the same funny name between the nfs_lookitup() fails and the
 * nfs_rename() completes, but...
 */
nfs_sillyrename(ndp, p)
	register struct nameidata *ndp;
	struct proc *p;
{
	register struct nfsnode *np;
	register struct sillyrename *sp;
	int error;
	short pid;

	np = VTONFS(ndp->ni_dvp);
	cache_purge(ndp->ni_dvp);
	MALLOC(sp, struct sillyrename *, sizeof (struct sillyrename),
		M_NFSREQ, M_WAITOK);
	bcopy((caddr_t)&np->n_fh, (caddr_t)&sp->s_fh, NFSX_FH);
	np = VTONFS(ndp->ni_vp);
	sp->s_cred = crdup(ndp->ni_cred);
	sp->s_dvp = ndp->ni_dvp;
	VREF(sp->s_dvp);

	/* Fudge together a funny name */
	pid = p->p_pid;
	bcopy(".nfsAxxxx4.4", sp->s_name, 13);
	sp->s_namlen = 12;
	sp->s_name[8] = hextoasc[pid & 0xf];
	sp->s_name[7] = hextoasc[(pid >> 4) & 0xf];
	sp->s_name[6] = hextoasc[(pid >> 8) & 0xf];
	sp->s_name[5] = hextoasc[(pid >> 12) & 0xf];

	/* Try lookitups until we get one that isn't there */
	while (nfs_lookitup(sp, (nfsv2fh_t *)0, p) == 0) {
		sp->s_name[4]++;
		if (sp->s_name[4] > 'z') {
			error = EINVAL;
			goto bad;
		}
	}
	if (error = nfs_renameit(ndp, sp, p))
		goto bad;
	nfs_lookitup(sp, &np->n_fh, p);
	np->n_sillyrename = sp;
	return (0);
bad:
	vrele(sp->s_dvp);
	crfree(sp->s_cred);
	free((caddr_t)sp, M_NFSREQ);
	return (error);
}

/*
 * Look up a file name for silly rename stuff.
 * Just like nfs_lookup() except that it doesn't load returned values
 * into the nfsnode table.
 * If fhp != NULL it copies the returned file handle out
 */
nfs_lookitup(sp, fhp, p)
	register struct sillyrename *sp;
	nfsv2fh_t *fhp;
	struct proc *p;
{
	register struct vnode *vp = sp->s_dvp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	long len;

	nfsstats.rpccnt[NFSPROC_LOOKUP]++;
	len = sp->s_namlen;
	nfsm_reqhead(nfs_procids[NFSPROC_LOOKUP], sp->s_cred, NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len));
	nfsm_fhtom(vp);
	nfsm_strtom(sp->s_name, len, NFS_MAXNAMLEN);
	nfsm_request(vp, NFSPROC_LOOKUP, p, 1);
	if (fhp != NULL) {
		nfsm_disect(cp, caddr_t, NFSX_FH);
		bcopy(cp, (caddr_t)fhp, NFSX_FH);
	}
	nfsm_reqdone;
	return (error);
}

/*
 * Kludge City..
 * - make nfs_bmap() essentially a no-op that does no translation
 * - do nfs_strategy() by faking physical I/O with nfs_readrpc/nfs_writerpc
 *   after mapping the physical addresses into Kernel Virtual space in the
 *   nfsiobuf area.
 *   (Maybe I could use the process's page mapping, but I was concerned that
 *    Kernel Write might not be enabled and also figured copyout() would do
 *    a lot more work than bcopy() and also it currently happens in the
 *    context of the swapper process (2).
 */
nfs_bmap(vp, bn, vpp, bnp)
	struct vnode *vp;
	daddr_t bn;
	struct vnode **vpp;
	daddr_t *bnp;
{
	if (vpp != NULL)
		*vpp = vp;
	if (bnp != NULL)
		*bnp = bn * btodb(vp->v_mount->mnt_stat.f_bsize);
	return (0);
}

/*
 * Strategy routine for phys. i/o
 * If the biod's are running, queue a request
 * otherwise just call nfs_doio() to get it done
 */
nfs_strategy(bp)
	register struct buf *bp;
{
	register struct buf *dp;
	register int i;
	int error = 0;
	int fnd = 0;

	/*
	 * Set b_proc. It seems a bit silly to do it here, but since bread()
	 * doesn't set it, I will.
	 * Set b_proc == NULL for asynchronous ops, since these may still
	 * be hanging about after the process terminates.
	 */
	if ((bp->b_flags & B_PHYS) == 0) {
		if (bp->b_flags & B_ASYNC)
			bp->b_proc = (struct proc *)0;
		else
			bp->b_proc = curproc;
	}
	/*
	 * If the op is asynchronous and an i/o daemon is waiting
	 * queue the request, wake it up and wait for completion
	 * otherwise just do it ourselves.
	 */
	if ((bp->b_flags & B_ASYNC) == 0 || nfs_numasync == 0)
		return (nfs_doio(bp));
	for (i = 0; i < NFS_MAXASYNCDAEMON; i++) {
		if (nfs_iodwant[i]) {
			dp = &nfs_bqueue;
			bp->b_actf = NULL;
			bp->b_actb = dp->b_actb;
			*dp->b_actb = bp;
			dp->b_actb = &bp->b_actf;
			fnd++;
			wakeup((caddr_t)&nfs_iodwant[i]);
			break;
		}
	}
	if (!fnd)
		error = nfs_doio(bp);
	return (error);
}

/*
 * Fun and games with i/o
 *
 * If the nfsiod's are not running, this is just called from nfs_strategy(),
 * otherwise it is called by the nfsiods to do what would normally be
 * partially disk interrupt driven.
 */
nfs_doio(bp)
	register struct buf *bp;
{
	register struct uio *uiop;
	register struct vnode *vp;
	struct nfsnode *np;
	struct ucred *cr;
	int error;
	struct uio uio;
	struct iovec io;

	vp = bp->b_vp;
	np = VTONFS(vp);
	uiop = &uio;
	uiop->uio_iov = &io;
	uiop->uio_iovcnt = 1;
	uiop->uio_segflg = UIO_SYSSPACE;
	/*
	 * Not to worry, `b_proc' will have been set to NULL
	 * for asynchronous IO.
	 */
	uiop->uio_procp = bp->b_proc;

	/*
	 * Always use credentials passed in the buffer header.
	 */
	if (bp->b_flags & B_PHYS) {
#if 0
		/*
		 * This cannot happen in the current (12/16/93) NetBSD kernel
		 * Is this an artifact of the pager implementation in
		 * previous incarnations of BSD ??
		 */
		if (bp->b_flags & B_DIRTY)
			uiop->uio_procp = pageproc;
#endif

		/* mapping was already done by vmapbuf */
		io.iov_base = bp->b_un.b_addr;
		io.iov_len = uiop->uio_resid = bp->b_bcount;
		uiop->uio_offset = bp->b_blkno * DEV_BSIZE;
		if (bp->b_flags & B_READ) {
			uiop->uio_rw = UIO_READ;
			nfsstats.read_physios++;
			bp->b_error = error = nfs_readrpc(vp, uiop, bp->b_rcred);
		} else {
			(void) vnode_pager_uncache(vp);
			uiop->uio_rw = UIO_WRITE;
			nfsstats.write_physios++;
			bp->b_error = error = nfs_writerpc(vp, uiop, bp->b_wcred);
		}

	} else {
		if (bp->b_flags & B_READ) {
			io.iov_len = uiop->uio_resid = bp->b_bcount;
			io.iov_base = bp->b_un.b_addr;
			uiop->uio_rw = UIO_READ;
			switch (vp->v_type) {
			case VREG:
				uiop->uio_offset = bp->b_blkno * DEV_BSIZE;
				nfsstats.read_bios++;
				error = nfs_readrpc(vp, uiop, bp->b_rcred);
				break;
			case VLNK:
				uiop->uio_offset = 0;
				nfsstats.readlink_bios++;
				error = nfs_readlinkrpc(vp, uiop, bp->b_rcred);
				break;
			case VDIR:
				uiop->uio_offset = bp->b_lblkno;
				nfsstats.readdir_bios++;
				error = nfs_readdirrpc(vp, uiop, bp->b_rcred);
				/*
				 * Save offset cookie in b_blkno.
				 */
				bp->b_blkno = uiop->uio_offset;
				break;
			};
			bp->b_error = error;
		} else {
			io.iov_len = uiop->uio_resid = bp->b_dirtyend
				- bp->b_dirtyoff;
			uiop->uio_offset = (bp->b_blkno * DEV_BSIZE)
				+ bp->b_dirtyoff;
			io.iov_base = bp->b_un.b_addr + bp->b_dirtyoff;
			uiop->uio_rw = UIO_WRITE;
			nfsstats.write_bios++;
			bp->b_error = error = nfs_writerpc(vp, uiop,
				bp->b_wcred);
			if (error) {
				np->n_error = error;
				np->n_flag |= NWRITEERR;
			}
			bp->b_dirtyoff = bp->b_dirtyend = 0;
		}
	}
	if (error)
		bp->b_flags |= B_ERROR;
	bp->b_resid = uiop->uio_resid;
	biodone(bp);
	return (error);
}

/*
 * Mmap a file
 *
 * NB Currently unsupported.
 */
/* ARGSUSED */
nfs_mmap(vp, fflags, cred, p)
	struct vnode *vp;
	int fflags;
	struct ucred *cred;
	struct proc *p;
{

	return (EINVAL);
}

/*
 * Flush all the blocks associated with a vnode.
 * 	Walk through the buffer pool and push any dirty pages
 *	associated with the vnode.
 */
/* ARGSUSED */
nfs_fsync(vp, fflags, cred, waitfor, p)
	register struct vnode *vp;
	int fflags;
	struct ucred *cred;
	int waitfor;
	struct proc *p;
{
	register struct nfsnode *np = VTONFS(vp);
	int error = 0;

	if (np->n_flag & NMODIFIED) {
		np->n_flag &= ~NMODIFIED;
		vflushbuf(vp, waitfor == MNT_WAIT ? B_SYNC : 0);
	}
	if (!error && (np->n_flag & NWRITEERR))
		error = np->n_error;
	return (error);
}

/*
 * NFS advisory byte-level locks.
 * Currently unsupported.
 */
nfs_advlock(vp, id, op, fl, flags)
	struct vnode *vp;
	caddr_t id;
	int op;
	struct flock *fl;
	int flags;
{
	register struct nfsnode *np = VTONFS(vp);

	return (lf_advlock(&(np->n_lockf), np->n_size, id, op, fl, flags));
}

/*
 * Print out the contents of an nfsnode.
 */
int
nfs_print(vp)
	struct vnode *vp;
{
	register struct nfsnode *np = VTONFS(vp);

	printf("tag VT_NFS, fileid %d fsid 0x%x",
		np->n_vattr.va_fileid, np->n_vattr.va_fsid);
#ifdef FIFO
	if (vp->v_type == VFIFO)
		fifo_printinfo(vp);
#endif /* FIFO */
	printf("%s\n", (np->n_flag & NLOCKED) ? " (LOCKED)" : "");
	if (np->n_lockholder == 0)
		return (0);
	printf("\towner pid %d", np->n_lockholder);
	if (np->n_lockwaiter)
		printf(" waiting pid %d", np->n_lockwaiter);
	printf("\n");
	return (0);
}

/*
 * Attribute cache routines.
 * nfs_loadattrcache() - loads or updates the cache contents from attributes
 *	that are on the mbuf list
 * nfs_getattrcache() - returns valid attributes if found in cache, returns
 *	error otherwise
 */

/*
 * Load the attribute cache (that lives in the nfsnode entry) with
 * the values on the mbuf list and
 * Iff vap not NULL
 *    copy the attributes to *vaper
 */
nfs_loadattrcache(vpp, mdp, dposp, vaper)
	struct vnode **vpp;
	struct mbuf **mdp;
	caddr_t *dposp;
	struct vattr *vaper;
{
	register struct vnode *vp = *vpp;
	register struct vattr *vap;
	register struct nfsv2_fattr *fp;
	extern struct vnodeops spec_nfsv2nodeops;
	register struct nfsnode *np;
	register long t1;
	caddr_t dpos, cp2;
	int error = 0;
	struct mbuf *md;
	enum vtype type;
	u_short mode;
	long rdev;
	struct timeval mtime;
	struct vnode *nvp;

	md = *mdp;
	dpos = *dposp;
	t1 = (mtod(md, caddr_t)+md->m_len)-dpos;
	if (error = nfsm_disct(&md, &dpos, NFSX_FATTR, t1, TRUE, &cp2))
		return (error);
	fp = (struct nfsv2_fattr *)cp2;
	type = nfstov_type(fp->fa_type);
	mode = fxdr_unsigned(u_short, fp->fa_mode);
	if (type == VNON)
		type = ntov_type[mode&0x7];
	rdev = fxdr_unsigned(long, fp->fa_rdev);
	fxdr_time(&fp->fa_mtime, &mtime);
	/*
	 * If v_type == VNON it is a new node, so fill in the v_type,
	 * n_mtime fields. Check to see if it represents a special 
	 * device, and if so, check for a possible alias. Once the
	 * correct vnode has been obtained, fill in the rest of the
	 * information.
	 */
	np = VTONFS(vp);
	if (vp->v_type == VNON) {
		if (type == VCHR && rdev == 0xffffffff)
			vp->v_type = type = VFIFO;
		else
			vp->v_type = type;
		if (vp->v_type == VFIFO) {
#ifdef FIFO
			extern struct vnodeops fifo_nfsv2nodeops;
			vp->v_op = &fifo_nfsv2nodeops;
#else
			return (EOPNOTSUPP);
#endif /* FIFO */
		}
		if (vp->v_type == VCHR || vp->v_type == VBLK) {
			vp->v_op = &spec_nfsv2nodeops;
			if (nvp = checkalias(vp, (dev_t)rdev, vp->v_mount)) {
				/*
				 * Reinitialize aliased node.
				 */
				np = VTONFS(nvp);
				np->n_vnode = nvp;
				np->n_flag = 0;
				nfs_lock(nvp);
				bcopy((caddr_t)&VTONFS(vp)->n_fh,
					(caddr_t)&np->n_fh, NFSX_FH);
				insque(np, nfs_hash(&np->n_fh));
				np->n_attrstamp = 0;
				np->n_sillyrename = (struct sillyrename *)0;
				/*
				 * Discard unneeded vnode and update actual one
				 */
				vput(vp);
				*vpp = nvp;
			}
		}
		np->n_mtime = mtime.tv_sec;
	}
	vap = &np->n_vattr;
	vap->va_type = type;
	vap->va_mode = (mode & 07777);
	vap->va_nlink = fxdr_unsigned(u_short, fp->fa_nlink);
	vap->va_uid = fxdr_unsigned(uid_t, fp->fa_uid);
	vap->va_gid = fxdr_unsigned(gid_t, fp->fa_gid);
	vap->va_size = fxdr_unsigned(u_long, fp->fa_size);
	if ((np->n_flag & NMODIFIED) == 0 || vap->va_size > np->n_size) {
		np->n_size = vap->va_size;
		vnode_pager_setsize(vp, np->n_size);
	}
	vap->va_size_rsv = 0;
	vap->va_blocksize = fxdr_unsigned(long, fp->fa_blocksize);
	vap->va_rdev = (dev_t)rdev;
	vap->va_bytes = fxdr_unsigned(long, fp->fa_blocks) * NFS_FABLKSIZE;
	vap->va_bytes_rsv = 0;
	vap->va_fsid = vp->v_mount->mnt_stat.f_fsid.val[0];
	vap->va_fileid = fxdr_unsigned(long, fp->fa_fileid);
	/* jfw@ksr.com 6/2/93 */
#if 0   /* bad assumption; Suns make full (and obvious) use of .usec fields */
	vap->va_atime.tv_sec = fxdr_unsigned(long, fp->fa_atime.tv_sec);
	vap->va_atime.tv_usec = 0;
	vap->va_flags = fxdr_unsigned(u_long, fp->fa_atime.tv_usec);
	vap->va_mtime = mtime;
	vap->va_ctime.tv_sec = fxdr_unsigned(long, fp->fa_ctime.tv_sec);
	vap->va_ctime.tv_usec = 0;
	vap->va_gen = fxdr_unsigned(u_long, fp->fa_ctime.tv_usec);
#else
	fxdr_time(&fp->fa_atime, &vap->va_atime);
	vap->va_mtime = mtime;
	fxdr_time(&fp->fa_ctime, &vap->va_ctime);
	vap->va_gen = 0;   /* can reliably learn nothing about this via NFS. */
	vap->va_flags = 0; /* can reliably learn nothing about this via NFS. */
#endif
	np->n_attrstamp = time.tv_sec;
	*dposp = dpos;
	*mdp = md;
	if (vaper != NULL) {
		bcopy((caddr_t)vap, (caddr_t)vaper, sizeof(*vap));
		if ((np->n_flag & NMODIFIED) && (np->n_size > vap->va_size))
			vaper->va_size = np->n_size;
	}
	return (0);
}

/*
 * Check the time stamp
 * If the cache is valid, copy contents to *vap and return 0
 * otherwise return an error
 */
nfs_getattrcache(vp, vap)
	register struct vnode *vp;
	struct vattr *vap;
{
	register struct nfsnode *np;

	np = VTONFS(vp);
	if ((time.tv_sec-np->n_attrstamp) < NFS_ATTRTIMEO) {
		nfsstats.attrcache_hits++;
		bcopy((caddr_t)&np->n_vattr,(caddr_t)vap,sizeof(struct vattr));
		if ((np->n_flag & NMODIFIED) == 0) {
			np->n_size = vap->va_size;
			vnode_pager_setsize(vp, np->n_size);
		} else if (np->n_size > vap->va_size)
			vap->va_size = np->n_size;
		if (np->n_delayed_atime.tv_sec != VNOVAL)
			vap->va_atime = np->n_delayed_atime; 
		if (np->n_delayed_mtime.tv_sec != VNOVAL)
			vap->va_mtime = np->n_delayed_mtime; 
		return (0);
	} else {
		nfsstats.attrcache_misses++;
		return (ENOENT);
	}
}

/*
 * Read wrapper for special devices.
 */
nfsspec_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	/*
	 * Set access time.
	 */
	VTONFS(vp)->n_delayed_atime = time;
	return (spec_read(vp, uio, ioflag, cred));
}

/*
 * Write wrapper for special devices.
 */
nfsspec_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	/*
	 * Set change time.
	 */
	VTONFS(vp)->n_delayed_mtime = time;
	return (spec_write(vp, uio, ioflag, cred));
}

/*
 * Close wrapper for special devices.
 *
 * Update the times then do device close.
 */
nfsspec_close(vp, fflag, cred, p)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
	struct proc *p;
{
	if (VTONFS(vp)->n_delayed_mtime.tv_sec != VNOVAL ||
	    VTONFS(vp)->n_delayed_atime.tv_sec != VNOVAL) {
		struct vattr va;
		VATTR_NULL(&va);
		nfs_setattr(vp, &va, cred, p);
	}
	return (spec_close(vp, fflag, cred, p));
}

#ifdef FIFO
/*
 * Read wrapper for special devices.
 */
nfsfifo_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	/*
	 * Set access time.
	 */
	VTONFS(vp)->n_delayed_atime = time;
	return (fifo_read(vp, uio, ioflag, cred));
}

/*
 * Write wrapper for special devices.
 */
nfsfifo_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	/*
	 * Set change time.
	 */
	VTONFS(vp)->n_delayed_mtime = time;
	return (fifo_write(vp, uio, ioflag, cred));
}

/*
 * Close wrapper for special devices.
 *
 * Update the times then do device close.
 */
nfsfifo_close(vp, fflag, cred, p)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
	struct proc *p;
{
	if (VTONFS(vp)->n_delayed_mtime.tv_sec != VNOVAL ||
	    VTONFS(vp)->n_delayed_atime.tv_sec != VNOVAL) {
		struct vattr va;
		VATTR_NULL(&va);
		nfs_setattr(vp, &va, cred, p);
	}
	return (fifo_close(vp, fflag, cred, p));
}
#endif /* FIFO */
