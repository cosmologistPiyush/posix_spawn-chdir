/*
 * Written by Paul Popelka (paulp@uts.amdahl.com)
 * 
 * You can do anything you want with this software, just don't say you wrote
 * it, and don't remove this notice.
 * 
 * This software is provided "as is".
 * 
 * The author supplies this software to be publicly redistributed on the
 * understanding that the author is not responsible for the correct
 * functioning of this software in any circumstances and is not liable for
 * any damages caused by this software.
 * 
 * October 1992
 * 
 *	msdosfs_vnops.c,v 1.1 1993/08/13 11:35:40 cgd Exp
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/resourcevar.h>	/* defines plimit structure in proc struct */
#include <sys/kernel.h>
#include <sys/file.h>		/* define FWRITE ... */
#include <sys/stat.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <miscfs/specfs/specdev.h> /* XXX */	/* defines v_rdev */
#include <sys/malloc.h>
#include <sys/dir.h>		/* defines dirent structure */

#include <msdosfs/bpb.h>
#include <msdosfs/direntry.h>
#include <msdosfs/denode.h>
#include <msdosfs/msdosfsmount.h>
#include <msdosfs/fat.h>
/*
 * Some general notes:
 * 
 * In the ufs filesystem the inodes, superblocks, and indirect blocks are
 * read/written using the vnode for the filesystem. Blocks that represent
 * the contents of a file are read/written using the vnode for the file
 * (including directories when they are read/written as files). This
 * presents problems for the dos filesystem because data that should be in
 * an inode (if dos had them) resides in the directory itself.  Since we
 * must update directory entries without the benefit of having the vnode
 * for the directory we must use the vnode for the filesystem.  This means
 * that when a directory is actually read/written (via read, write, or
 * readdir, or seek) we must use the vnode for the filesystem instead of
 * the vnode for the directory as would happen in ufs. This is to insure we
 * retreive the correct block from the buffer cache since the hash value is
 * based upon the vnode address and the desired block number.
 */

/*
 * Create a regular file. On entry the directory to contain the file being
 * created is locked.  We must release before we return. We must also free
 * the pathname buffer pointed at by ndp->ni_pnbuf, always on error, or
 * only if the SAVESTART bit in ni_nameiop is clear on success.
 */
int
msdosfs_create(ndp, vap, p)
	struct nameidata *ndp;
	struct vattr *vap;
	struct proc *p;
{
	struct denode ndirent;
	struct denode *dep;
	struct denode *pdep = VTODE(ndp->ni_dvp);
	int error;

#if defined(MSDOSFSDEBUG)
	printf("msdosfs_create(ndp %08x, vap %08x, p %08x\n", ndp, vap, p);
#endif				/* defined(MSDOSFSDEBUG) */

	/*
	 * Create a directory entry for the file, then call createde() to
	 * have it installed. NOTE: DOS files are always executable.  We
	 * use the absence of the owner write bit to make the file
	 * readonly.
	 */
	bzero(&ndirent, sizeof(ndirent));
	unix2dostime(&time, &ndirent.de_Date, &ndirent.de_Time);
	unix2dosfn((u_char *) ndp->ni_ptr, ndirent.de_Name, ndp->ni_namelen);
	ndirent.de_Attributes = (vap->va_mode & VWRITE) ? 0 : ATTR_READONLY;
	ndirent.de_StartCluster = 0;
	ndirent.de_FileSize = 0;
	ndirent.de_pmp = pdep->de_pmp;
	ndirent.de_dev = pdep->de_dev;
	ndirent.de_devvp = pdep->de_devvp;
	if ((error = createde(&ndirent, ndp, &dep)) == 0) {
		ndp->ni_vp = DETOV(dep);
		if ((ndp->ni_nameiop & SAVESTART) == 0)
			free(ndp->ni_pnbuf, M_NAMEI);
	}
	else {
		free(ndp->ni_pnbuf, M_NAMEI);
	}
	deput(pdep);		/* release parent dir */
	return error;
}

int
msdosfs_mknod(ndp, vap, cred, p)
	struct nameidata *ndp;
	struct vattr *vap;
	struct ucred *cred;
	struct proc *p;
{
	int error;
	struct denode *pdep = VTODE(ndp->ni_dvp);

	switch (vap->va_type) {
	case VDIR:
		error = msdosfs_mkdir(ndp, vap, p);
		break;

		/*
		 * msdosfs_create() sets ndp->ni_vp.
		 */
	case VREG:
		error = msdosfs_create(ndp, vap, p);
		break;

	default:
		error = EINVAL;
		free(ndp->ni_pnbuf, M_NAMEI);
		deput(pdep);
		break;
	}
	return error;
}

int
msdosfs_open(vp, mode, cred, p)
	struct vnode *vp;
	int mode;
	struct ucred *cred;
	struct proc *p;
{
	return 0;
}

int
msdosfs_close(vp, fflag, cred, p)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
	struct proc *p;
{
	struct denode *dep = VTODE(vp);

	if (vp->v_usecount > 1 && !(dep->de_flag & DELOCKED))
		DETIMES(dep, &time);
	return 0;
}

int
msdosfs_access(vp, mode, cred, p)
	struct vnode *vp;
	int mode;
	struct ucred *cred;
	struct proc *p;
{
	int dosmode;
	struct denode *dep = VTODE(vp);

	/*
	 * Root gets to do anything.  Even execute a file without the x-bit
	 * on?  But, for dos filesystems every file is executable.  I may
	 * regret this.
	 */
	if (cred->cr_uid == 0)
		return 0;

	/*
	 * mode is filled with a combination of VREAD, VWRITE, and/or VEXEC
	 * bits turned on.  In an octal number these are the Y in 0Y00.
	 * 
	 * Since the dos filesystem doesn't have the concept of file ownership
	 * we just give everybody read and execute access and write access
	 * if the readonly bit is off.
	 */
	dosmode = VEXEC | VREAD |
	    ((dep->de_Attributes & ATTR_READONLY) ? 0 : VWRITE);
	return ((dosmode & mode) != 0) ? 0 : EACCES;
}

int
msdosfs_getattr(vp, vap, cred, p)
	struct vnode *vp;
	struct vattr *vap;
	struct ucred *cred;
	struct proc *p;
{
	u_int cn;
	struct denode *dep = VTODE(vp);

	DETIMES(dep, &time);
	vap->va_fsid = dep->de_dev;
	/*
	 * The following computation of the fileid must be the same as that
	 * used in msdosfs_readdir() to compute d_fileno. If not, pwd
	 * doesn't work.
	 */
	if (dep->de_Attributes & ATTR_DIRECTORY) {
		if ((cn = dep->de_StartCluster) == MSDOSFSROOT)
			cn = 1;
	}
	else {
		if ((cn = dep->de_dirclust) == MSDOSFSROOT)
			cn = 1;
		cn = (cn << 16) | (dep->de_diroffset & 0xffff);
	}
	vap->va_fileid = cn;
	vap->va_mode = (dep->de_Attributes & ATTR_READONLY) ? 0555 : 0777;
	vap->va_mode &= dep->de_pmp->pm_mask;
	if (dep->de_Attributes & ATTR_DIRECTORY)
		vap->va_mode |= S_IFDIR;
	vap->va_nlink = 1;
	vap->va_gid = dep->de_pmp->pm_gid;
	vap->va_uid = dep->de_pmp->pm_uid;
	vap->va_rdev = 0;
	vap->va_size = dep->de_FileSize;
	vap->va_size_rsv = 0;
	dos2unixtime(dep->de_Date, dep->de_Time, &vap->va_atime);
	vap->va_atime.tv_usec = 0;
	vap->va_mtime.tv_sec = vap->va_atime.tv_sec;
	vap->va_mtime.tv_usec = 0;
#ifndef MSDOSFS_NODIRMOD
	if (vap->va_mode & S_IFDIR) {
		vap->va_mtime.tv_sec = time.tv_sec;
		vap->va_mtime.tv_usec = time.tv_usec;
	}
#endif
	vap->va_ctime.tv_sec = vap->va_atime.tv_sec;
	vap->va_ctime.tv_usec = 0;
	vap->va_flags = dep->de_flag;
	vap->va_gen = 0;
	vap->va_blocksize = dep->de_pmp->pm_bpcluster;
	vap->va_bytes = (dep->de_FileSize + dep->de_pmp->pm_crbomask) &
	    			~(dep->de_pmp->pm_crbomask);
	vap->va_bytes_rsv = 0;
	vap->va_type = vp->v_type;
	return 0;
}

int
msdosfs_setattr(vp, vap, cred, p)
	struct vnode *vp;
	struct vattr *vap;
	struct ucred *cred;
	struct proc *p;
{
	int error = 0;
	struct denode *dep = VTODE(vp);

#if defined(MSDOSFSDEBUG)
	printf("msdosfs_setattr(): vp %08x, vap %08x, cred %08x, p %08x\n",
	    vp, vap, cred, p);
#endif				/* defined(MSDOSFSDEBUG) */
	if ((vap->va_type != VNON) ||
	    (vap->va_nlink != VNOVAL) ||
	    (vap->va_fsid != VNOVAL) ||
	    (vap->va_fileid != VNOVAL) ||
	    (vap->va_blocksize != VNOVAL) ||
	    (vap->va_rdev != VNOVAL) ||
	    (vap->va_bytes != VNOVAL) ||
	    (vap->va_gen != VNOVAL) ||
	    (vap->va_uid != (u_short) VNOVAL) ||
	    (vap->va_gid != (u_short) VNOVAL) ||
	    (vap->va_atime.tv_sec != VNOVAL)) {
#if defined(MSDOSFSDEBUG)
		printf("msdosfs_setattr(): returning EINVAL\n");
		printf("    va_type %d, va_nlink %x, va_fsid %x, va_fileid %x\n",
		    vap->va_type, vap->va_nlink, vap->va_fsid, vap->va_fileid);
		printf("    va_blocksize %x, va_rdev %x, va_bytes %x, va_gen %x\n",
		    vap->va_blocksize, vap->va_rdev, vap->va_bytes, vap->va_gen);
		printf("    va_uid %x, va_gid %x, va_atime.tv_sec %x\n",
		    vap->va_uid, vap->va_gid, vap->va_atime.tv_sec);
#endif				/* defined(MSDOSFSDEBUG) */
		return EINVAL;
	}

	if (vap->va_size != VNOVAL) {
		if (vp->v_type == VDIR)
			return EISDIR;
		if (error = detrunc(dep, vap->va_size, 0))
			return error;
	}
	if (vap->va_mtime.tv_sec != VNOVAL) {
		dep->de_flag |= DEUPD;
		if (error = deupdat(dep, &vap->va_mtime, 1))
			return error;
	}

	/*
	 * DOS files only have the ability to have thier writability
	 * attribute set, so we use the owner write bit to set the readonly
	 * attribute.
	 */
	if (vap->va_mode != (u_short) VNOVAL) {
		/* We ignore the read and execute bits */
		if (vap->va_mode & VWRITE)
			dep->de_Attributes &= ~ATTR_READONLY;
		else
			dep->de_Attributes |= ATTR_READONLY;
		dep->de_flag |= DEUPD;
	}

	if (vap->va_flags != VNOVAL) {
		if (error = suser(cred, &p->p_acflag))
			return error;
		if (cred->cr_uid == 0)
			dep->de_flag = vap->va_flags;
		else {
			dep->de_flag &= 0xffff0000;
			dep->de_flag |= (vap->va_flags & 0xffff);
		}
		dep->de_flag |= DEUPD;
	}
	return error;
}


int
msdosfs_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	int error = 0;
	int diff;
	int isadir;
	long n;
	long on;
	daddr_t bn;
	daddr_t lbn;
	daddr_t rablock;
	struct buf *bp;
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;

	/*
	 * If they didn't ask for any data, then we are done.
	 */
	if (uio->uio_resid == 0)
		return 0;
	if (uio->uio_offset < 0)
		return EINVAL;

	isadir = dep->de_Attributes & ATTR_DIRECTORY;
	do {
		lbn = uio->uio_offset >> pmp->pm_cnshift;
		on = uio->uio_offset & pmp->pm_crbomask;
		n = MIN((u_long) (pmp->pm_bpcluster - on), uio->uio_resid);
		diff = dep->de_FileSize - uio->uio_offset;
		if (diff <= 0)
			return 0;
		/* convert cluster # to block # if a directory */
		if (isadir) {
			error = pcbmap(dep, lbn, &lbn, 0);
			if (error)
				return error;
		}
		if (diff < n)
			n = diff;
		/*
		 * If we are operating on a directory file then be sure to
		 * do i/o with the vnode for the filesystem instead of the
		 * vnode for the directory.
		 */
		if (isadir) {
			error = bread(pmp->pm_devvp, lbn, pmp->pm_bpcluster,
			    NOCRED, &bp);
		}
		else {
			rablock = lbn + 1;
			if (vp->v_lastr + 1 == lbn &&
			    rablock * pmp->pm_bpcluster < dep->de_FileSize) {
				error = breada(vp, lbn, pmp->pm_bpcluster,
				    rablock, pmp->pm_bpcluster, NOCRED, &bp);
			}
			else {
				error = bread(vp, lbn, pmp->pm_bpcluster, NOCRED,
				    &bp);
			}
			vp->v_lastr = lbn;
		}
		n = MIN(n, pmp->pm_bpcluster - bp->b_resid);
		if (error) {
			brelse(bp);
			return error;
		}
		error = uiomove(bp->b_un.b_addr + on, (int) n, uio);
		/*
		 * If we have read everything from this block or have read
		 * to end of file then we are done with this block.  Mark
		 * it to say the buffer can be reused if need be.
		 */
#if 0
		if (n + on == pmp->pm_bpcluster ||
		    uio->uio_offset == dep->de_FileSize)
			bp->b_flags |= B_AGE;
#endif
		brelse(bp);
	} while (error == 0 && uio->uio_resid > 0 && n != 0);
	return error;
}

/*
 * Write data to a file or directory.
 */
int
msdosfs_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	int n;
	int isadir;
	int croffset;
	int resid;
	int osize;
	int error;
	u_long cluster;
	u_long nextcluster;
	u_long lastcluster;
	daddr_t bn;
	struct buf *bp;
	struct proc *p = uio->uio_procp;
	struct vnode *thisvp;
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;

#if defined(MSDOSFSDEBUG)
	printf("msdosfs_write(vp %08x, uio %08x, ioflag %08x, cred %08x\n",
	    vp, uio, ioflag, cred);
	printf("msdosfs_write(): diroff %d, dirclust %d, startcluster %d\n",
	    dep->de_diroffset, dep->de_dirclust, dep->de_StartCluster);
#endif				/* defined(MSDOSFSDEBUG) */

	switch (vp->v_type) {
	case VREG:
		if (ioflag & IO_APPEND)
			uio->uio_offset = dep->de_FileSize;
		isadir = 0;
		thisvp = vp;
		break;

	case VDIR:
		if ((ioflag & IO_SYNC) == 0)
			panic("msdosfs_write(): non-sync directory update");
		isadir = 1;
		thisvp = pmp->pm_devvp;
		break;

	default:
		panic("msdosfs_write(): bad file type");
		break;
	}

	if (uio->uio_offset < 0) {
		return EINVAL;
	}
	if (uio->uio_resid == 0)
		return 0;

	/*
	 * If they've exceeded their filesize limit, tell them about it.
	 */
	if (vp->v_type == VREG && p &&
	    ((uio->uio_offset + uio->uio_resid) >
		p->p_rlimit[RLIMIT_FSIZE].rlim_cur)) {
		psignal(p, SIGXFSZ);
		return EFBIG;
	}

	/*
	 * If attempting to write beyond the end of the root directory we
	 * stop that here because the root directory can not grow.
	 */
	if ((dep->de_Attributes & ATTR_DIRECTORY) &&
	    dep->de_StartCluster == MSDOSFSROOT &&
	    (uio->uio_offset + uio->uio_resid) > dep->de_FileSize)
		return ENOSPC;

	/*
	 * If the offset we are starting the write at is beyond the end of
	 * the file, then they've done a seek.  Unix filesystems allow
	 * files with holes in them, DOS doesn't so we must fill the hole
	 * with zeroed blocks.  We do this by calling our seek function.
	 * This could probably be cleaned up someday.
	 */
	if (uio->uio_offset > dep->de_FileSize) {
		error = msdosfs_seek(vp, (off_t) 0, uio->uio_offset, cred);
		if (error)
			return error;
	}

	/*
	 * Remember some values in case the write fails.
	 */
	resid = uio->uio_resid;
	osize = dep->de_FileSize;

	do {
		bn = uio->uio_offset >> pmp->pm_cnshift;
		/*
		 * If we are appending to the file and we are on a cluster
		 * boundary, then allocate a new cluster and chain it onto
		 * the file.
		 */
		if (uio->uio_offset == dep->de_FileSize &&
		    (uio->uio_offset & pmp->pm_crbomask) == 0) {
			if (error = extendfile(dep, &bp, 0))
				break;
		}
		else {
			/*
			 * The block we need to write into exists, so just
			 * read it in.
			 */
			if (isadir) {
				error = pcbmap(dep, bn, &bn, 0);
				if (error)
					return error;
			}
			error = bread(thisvp, bn, pmp->pm_bpcluster, cred, &bp);
			if (error)
				return error;
		}
		croffset = uio->uio_offset & pmp->pm_crbomask;
		n = MIN(uio->uio_resid, pmp->pm_bpcluster - croffset);
		if (uio->uio_offset + n > dep->de_FileSize) {
			dep->de_FileSize = uio->uio_offset + n;
			vnode_pager_setsize(vp, dep->de_FileSize);	/* why? */
		}
		(void) vnode_pager_uncache(vp);	/* why not? */
		/*
		 * Should these vnode_pager_* functions be done on dir
		 * files?
		 */

		/*
		 * Copy the data from user space into the buf header.
		 */
		error = uiomove(bp->b_un.b_addr + croffset, n, uio);

		/*
		 * If they want this synchronous then write it and wait for
		 * it.  Otherwise, if on a cluster boundary write it
		 * asynchronously so we can move on to the next block
		 * without delay.  Otherwise do a delayed write because we
		 * may want to write somemore into the block later.
		 */
		if (ioflag & IO_SYNC)
			(void) bwrite(bp);
		else if (n + croffset == pmp->pm_bpcluster) {
			bp->b_flags |= B_AGE;
			bawrite(bp);
		}
		else
			bdwrite(bp);
		dep->de_flag |= DEUPD;
	} while (error == 0 && uio->uio_resid > 0);

	/*
	 * If the write failed and they want us to, truncate the file back
	 * to the size it was before the write was attempted.
	 */
	if (error && (ioflag & IO_UNIT)) {
		detrunc(dep, osize, ioflag & IO_SYNC);
		uio->uio_offset -= resid - uio->uio_resid;
		uio->uio_resid = resid;
	}
	if (!error && (ioflag & IO_UNIT))
		error = deupdat(dep, &time, 1);
	return error;
}

int
msdosfs_ioctl(vp, com, data, fflag, cred, p)
	struct vnode *vp;
	int com;
	caddr_t data;
	int fflag;
	struct ucred *cred;
	struct proc *p;
{
	return ENOTTY;
}

int
msdosfs_select(vp, which, fflags, cred, p)
	struct vnode *vp;
	int which;
	int fflags;
	struct ucred *cred;
	struct proc *p;
{
	return 1;		/* DOS filesystems never block? */
}

int
msdosfs_mmap(vp, fflags, cred, p)
	struct vnode *vp;
	int fflags;
	struct ucred *cred;
	struct proc *p;
{
	return EINVAL;
}

/*
 * Flush the blocks of a file to disk.
 * 
 * This function is worthless for vnodes that represent directories. Maybe we
 * could just do a sync if they try an fsync on a directory file.
 */
int
msdosfs_fsync(vp, fflags, cred, waitfor, p)
	struct vnode *vp;
	int fflags;
	struct ucred *cred;
	int waitfor;
	struct proc *p;
{
	struct denode *dep = VTODE(vp);

	if (fflags & FWRITE)
		dep->de_flag |= DEUPD;
	/*
	 * Does this call to vflushbuf() do anything?  I can find no code
	 * anywhere that sets v_dirtyblkhd in the vnode, which vflushbuf()
	 * seems to depend upon.
	 */
	vflushbuf(vp, waitfor == MNT_WAIT ? B_SYNC : 0);
	return deupdat(dep, &time, waitfor == MNT_WAIT);
}

/*
 * Since the dos filesystem does not allow files with holes in them we must
 * fill the file with zeroed blocks when a seek past the end of file
 * happens.
 * 
 * It seems that nothing in the kernel calls the filesystem specific file seek
 * functions.  And, someone on the net told me that NFS never sends
 * announcements of seeks to the server.  So, if msdosfs ever becomes NFS
 * mountable it will have to use other means to fill in holes in what would
 * be a sparse file. (This appears fixed since msdosfs_write() calls seek
 * before writing if the offset is past EOF)
 */
int
msdosfs_seek(vp, oldoff, newoff, cred)
	struct vnode *vp;
	off_t oldoff;
	off_t newoff;
	struct ucred *cred;
{
	int error = 0;
	off_t foff;
	daddr_t bn;
	u_long cluster;
	u_long lastcluster;
	struct buf *bp;
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;

#if defined(MSDOSFSDEBUG)
	printf("msdosfs_seek(vp %08x, oldoff %d, newoff %d, cred %08x)\n",
	    vp, oldoff, newoff, cred);
#endif				/* defined(MSDOSFSDEBUG) */

	/*
	 * Compute the offset of the first byte after the last block in the
	 * file. If seeking beyond the end of file then fill the file with
	 * zeroed blocks up to the seek address.
	 */
	foff = (dep->de_FileSize + (pmp->pm_bpcluster - 1)) & ~pmp->pm_crbomask;
#if defined(MSDOSFSDEBUG)
	printf("seek: newoff %d > foff %d\n", newoff, foff);
#endif				/* defined(MSDOSFSDEBUG) */
	if (newoff > foff) {
		/*
		 * If this is the root directory and we are attempting to
		 * seek beyond the end disallow it.  DOS filesystem root
		 * directories can not grow.
		 */
		if (vp->v_flag & VROOT)
			return EINVAL;
		/*
		 * If this is a directory and the caller is not root, then
		 * do not let them seek beyond the end of file.  If we
		 * allowed this then users could cause directories to grow.
		 * Is this really that important?
		 */
		if (dep->de_Attributes & ATTR_DIRECTORY) {
			if (error = suser(cred, NULL)) {
				return error;
			}
		}
		/*
		 * Allocate and chain together as many clusters as are
		 * needed to get to newoff.
		 */
		while (foff < newoff) {
			if (error = extendfile(dep, &bp, 0))
				return error;
			dep->de_flag |= DEUPD;
			bdwrite(bp);
			foff += pmp->pm_bpcluster;
			dep->de_FileSize += pmp->pm_bpcluster;
		}		/* end while() */
		dep->de_FileSize = newoff;
		return deupdat(dep, &time);
	}
	return 0;
}

int
msdosfs_remove(ndp, p)
	struct nameidata *ndp;
	struct proc *p;
{
	int error;
	struct denode *dep = VTODE(ndp->ni_vp);
	struct denode *ddep = VTODE(ndp->ni_dvp);

	error = removede(ndp);
#if defined(MSDOSFSDEBUG)
	printf("msdosfs_remove(), dep %08x, v_usecount %d\n", dep, ndp->ni_vp->v_usecount);
#endif				/* defined(MSDOSFSDEBUG) */
	if (ddep == dep)
		vrele(DETOV(dep));
	else
		deput(dep);	/* causes msdosfs_inactive() to be called
				 * via vrele() */
	deput(ddep);
	return error;
}

/*
 * DOS filesystems don't know what links are. But since we already called
 * msdosfs_lookup() with create and lockparent, the parent is locked so we
 * have to free it before we return the error.
 */
int
msdosfs_link(vp, ndp, p)
	struct vnode *vp;
	struct nameidata *ndp;
	struct proc *p;
{
	struct denode *pdep = VTODE(ndp->ni_dvp);

	free(ndp->ni_pnbuf, M_NAMEI);
	deput(pdep);
	return EINVAL;
}

/*
 * Renames on files require moving the denode to a new hash queue since the
 * denode's location is used to compute which hash queue to put the file
 * in. Unless it is a rename in place.  For example "mv a b".
 * 
 * What follows is the basic algorithm:
 * 
 * if (file move) { if (dest file exists) { remove dest file } if (dest and
 * src in same directory) { rewrite name in existing directory slot } else
 * { write new entry in dest directory update offset and dirclust in denode
 * move denode to new hash chain clear old directory entry } } else {
 * directory move if (dest directory exists) { if (dest is not empty) {
 * return ENOTEMPTY } remove dest directory } if (dest and src in same
 * directory) { rewrite name in existing entry } else { be sure dest is not
 * a child of src directory write entry in dest directory update "." and
 * ".." in moved directory update offset and dirclust in denode move denode
 * to new hash chain clear old directory entry for moved directory } }
 * 
 * On entry: source's parent directory is unlocked source file or directory is
 * unlocked destination's parent directory is locked destination file or
 * directory is locked if it exists
 * 
 * On exit: all denodes should be released Notes: I'm not sure how the memory
 * containing the pathnames pointed at by the nameidata structures is
 * freed, there may be some memory bleeding for each rename done.
 */
int
msdosfs_rename(fndp, tndp, p)
	struct nameidata *fndp;
	struct nameidata *tndp;
	struct proc *p;
{
	u_char toname[11];
	int error;
	int newparent = 0;
	int sourceisadirectory = 0;
	u_long to_dirclust;
	u_long to_diroffset;
	u_long cn;
	daddr_t bn;
	struct denode *fddep;	/* from file's parent directory	 */
	struct denode *fdep;	/* from file or directory	 */
	struct denode *tddep;	/* to file's parent directory	 */
	struct denode *tdep;	/* to file or directory		 */
	struct msdosfsmount *pmp;
	struct direntry *dotdotp;
	struct direntry *ep;
	struct buf *bp;

#if defined(MSDOSFSDEBUG)
	printf("msdosfs_rename(fndp %08x, tndp %08x, p %08x\n", fndp, tndp, p);
#endif				/* defined(MSDOSFSDEBUG) */
	fddep = VTODE(fndp->ni_dvp);
	fdep = VTODE(fndp->ni_vp);
	tddep = VTODE(tndp->ni_dvp);
	tdep = tndp->ni_vp ? VTODE(tndp->ni_vp) : NULL;
	pmp = fddep->de_pmp;

#ifdef __NetBSD__
	/* Check for cross-device rename */
	if ((fndp->ni_vp->v_mount != tndp->ni_dvp->v_mount) ||
	    (tndp->ni_vp && (fndp->ni_vp->v_mount != tndp->ni_vp->v_mount))) {
		error = EXDEV;
		goto bad;
	}
#endif

	/*
	 * Convert the filename in tdnp into a dos filename. We copy this
	 * into the denode and directory entry for the destination
	 * file/directory.
	 */
	unix2dosfn((u_char *) tndp->ni_ptr, toname, tndp->ni_namelen);

	/*
	 * At this point this is the lock state of the denodes: fddep
	 * referenced fdep  referenced tddep locked tdep  locked if it
	 * exists
	 */

	/*
	 * Be sure we are not renaming ".", "..", or an alias of ".". This
	 * leads to a crippled directory tree.  It's pretty tough to do a
	 * "ls" or "pwd" with the "." directory entry missing, and "cd .."
	 * doesn't work if the ".." entry is missing.
	 */
	if (fdep->de_Attributes & ATTR_DIRECTORY) {
		if ((fndp->ni_namelen == 1 && fndp->ni_ptr[0] == '.') ||
		    fddep == fdep ||	/* won't happen ? */
		    fndp->ni_isdotdot) {
			VOP_ABORTOP(tndp);
			vput(tndp->ni_dvp);
			if (tndp->ni_vp)
				vput(tndp->ni_vp);
			VOP_ABORTOP(fndp);
			vrele(fndp->ni_dvp);
			vrele(fndp->ni_vp);
			return EINVAL;
		}
		sourceisadirectory = 1;
	}

	/*
	 * If we are renaming a directory, and the directory is being moved
	 * to another directory, then we must be sure the destination
	 * directory is not in the subtree of the source directory.  This
	 * could orphan everything under the source directory.
	 * doscheckpath() unlocks the destination's parent directory so we
	 * must look it up again to relock it.
	 */
	if (fddep->de_StartCluster != tddep->de_StartCluster)
		newparent = 1;
	if (sourceisadirectory && newparent) {
		if (tdep) {
			deput(tdep);
			tdep = NULL;
		}
		/* doscheckpath() deput()'s tddep */
		error = doscheckpath(fdep, tddep, tndp->ni_cred);
		tddep = NULL;
		if (error) {
			goto bad;
		}
		if ((tndp->ni_nameiop & SAVESTART) == 0)
			panic("msdosfs_rename(): lost to startdir");
		if (error = lookup(tndp, p)) {
			goto bad;
		}
		tddep = VTODE(tndp->ni_dvp);
		tdep = tndp->ni_vp ? VTODE(tndp->ni_vp) : NULL;
	}

	/*
	 * If the destination exists, then be sure its type (file or dir)
	 * matches that of the source.  And, if it is a directory make sure
	 * it is empty.  Then delete the destination.
	 */
	if (tdep) {
		if (tdep->de_Attributes & ATTR_DIRECTORY) {
			if (!sourceisadirectory) {
				error = ENOTDIR;
				goto bad;
			}
			if (!dosdirempty(tdep)) {
				error = ENOTEMPTY;
				goto bad;
			}
		}
		else {		/* destination is file */
			if (sourceisadirectory) {
				error = EISDIR;
				goto bad;
			}
		}
		to_dirclust = tdep->de_dirclust;
		to_diroffset = tdep->de_diroffset;
		if (error = removede(tndp)) {
			goto bad;
		}
		deput(tdep);
		tdep = NULL;

		/*
		 * Remember where the slot was for createde().
		 */
		tndp->ni_msdosfs.msdosfs_count = 1;
		tndp->ni_msdosfs.msdosfs_cluster = to_dirclust;
		tndp->ni_msdosfs.msdosfs_offset = to_diroffset;
	}

	/*
	 * If the source and destination are in the same directory then
	 * just read in the directory entry, change the name in the
	 * directory entry and write it back to disk.
	 */
	if (newparent == 0) {
		/* tddep and fddep point to the same denode here */
		DELOCK(fdep);	/* tddep is already locked */
		if (error = readep(fdep->de_pmp,
			fndp->ni_msdosfs.msdosfs_cluster,
			fndp->ni_msdosfs.msdosfs_offset,
			&bp, &ep)) {
			DEUNLOCK(fdep);
			goto bad;
		}
		bcopy(toname, ep->deName, 11);
		if (error = bwrite(bp)) {
			DEUNLOCK(fdep);
			goto bad;
		}
		bcopy(toname, fdep->de_Name, 11);	/* update denode */
		/*
		 * fdep locked fddep and tddep point to the same denode
		 * which is locked tdep is unlocked and unreferenced
		 */
	}
	else {
		u_long dirsize;

		/*
		 * If the source and destination are in different
		 * directories, then mark the entry in the source directory
		 * as deleted and write a new entry in the destination
		 * directory.  Then move the denode to the correct hash
		 * chain for its new location in the filesystem.  And, if
		 * we moved a directory, then update its .. entry to point
		 * to the new parent directory. If we moved a directory
		 * will also insure that the directory entry on disk has a
		 * filesize of zero.
		 */
		DELOCK(fdep);
		bcopy(toname, fdep->de_Name, 11);	/* update denode */
		if (fdep->de_Attributes & ATTR_DIRECTORY) {
			dirsize = fdep->de_FileSize;
			fdep->de_FileSize = 0;
		}
		error = createde(fdep, tndp, (struct denode **) 0);
		if (fdep->de_Attributes & ATTR_DIRECTORY) {
			fdep->de_FileSize = dirsize;
		}
		if (error) {
			/* should put back filename */
			DEUNLOCK(fdep);
			goto bad;
		}
		DELOCK(fddep);
		if (error = readep(fdep->de_pmp,
			fndp->ni_msdosfs.msdosfs_cluster,
			fndp->ni_msdosfs.msdosfs_offset,
			&bp, &ep)) {
			DEUNLOCK(fdep);
			DEUNLOCK(fddep);
			goto bad;
		}
		ep->deName[0] = SLOT_DELETED;
		if (error = bwrite(bp)) {
			DEUNLOCK(fdep);
			DEUNLOCK(fddep);
			goto bad;
		}
		fdep->de_dirclust = tndp->ni_msdosfs.msdosfs_cluster;
		fdep->de_diroffset = tndp->ni_msdosfs.msdosfs_offset;
		reinsert(fdep);
		DEUNLOCK(fddep);
	}
	/* fdep is still locked here */

	/*
	 * If we moved a directory to a new parent directory, then we must
	 * fixup the ".." entry in the moved directory.
	 */
	if (sourceisadirectory && newparent) {
		cn = fdep->de_StartCluster;
		if (cn == MSDOSFSROOT) {
			/* this should never happen */
			panic("msdosfs_rename(): updating .. in root directory?\n");
		}
		else {
			bn = cntobn(pmp, cn);
		}
		error = bread(pmp->pm_devvp, bn, pmp->pm_bpcluster,
		    NOCRED, &bp);
		if (error) {
			/* should really panic here, fs is corrupt */
			DEUNLOCK(fdep);
			goto bad;
		}
		dotdotp = (struct direntry *) bp->b_un.b_addr + 1;
		putushort(dotdotp->deStartCluster, tddep->de_StartCluster);
		error = bwrite(bp);
		DEUNLOCK(fdep);
		if (error) {
			/* should really panic here, fs is corrupt */
			goto bad;
		}
	}
	else {
		DEUNLOCK(fdep);
	}
bad:	;
	vrele(DETOV(fdep));
	vrele(DETOV(fddep));
	if (tdep)
		vput(DETOV(tdep));
	if (tddep)
		vput(DETOV(tddep));
	return error;
}

struct {
	struct direntry dot;
	struct direntry dotdot;
}      dosdirtemplate = {

	".       ", "   ",	/* the . entry */
	ATTR_DIRECTORY,		/* file attribute */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* resevered */
	210, 4, 210, 4,		/* time and date */
	0, 0,			/* startcluster */
	0, 0, 0, 0,		/* filesize */
	"..      ", "   ",	/* the .. entry */
	ATTR_DIRECTORY,		/* file attribute */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* resevered */
	210, 4, 210, 4,		/* time and date */
	0, 0,			/* startcluster */
	0, 0, 0, 0,		/* filesize */
};

int
msdosfs_mkdir(ndp, vap, p)
	struct nameidata *ndp;
	struct vattr *vap;
	struct proc *p;
{
	int bn;
	int error;
	u_long newcluster;
	struct denode *pdep;
	struct denode *ndep;
	struct vnode *pvp;
	struct direntry *denp;
	struct denode ndirent;
	struct msdosfsmount *pmp;
	struct buf *bp;
	u_short dDate, dTime;

	pvp = ndp->ni_dvp;
	pdep = VTODE(pvp);

	/*
	 * If this is the root directory and there is no space left we
	 * can't do anything.  This is because the root directory can not
	 * change size.
	 */
	if (pdep->de_StartCluster == MSDOSFSROOT && ndp->ni_msdosfs.msdosfs_count == 0) {
		free(ndp->ni_pnbuf, M_NAMEI);
		deput(pdep);
		return ENOSPC;
	}

	pmp = pdep->de_pmp;

	/*
	 * Allocate a cluster to hold the about to be created directory.
	 */
	if (error = clusteralloc(pmp, &newcluster, CLUST_EOFE)) {
		free(ndp->ni_pnbuf, M_NAMEI);
		deput(pdep);
		return error;
	}

	/*
	 * Now fill the cluster with the "." and ".." entries. And write
	 * the cluster to disk.  This way it is there for the parent
	 * directory to be pointing at if there were a crash.
	 */
	bn = cntobn(pmp, newcluster);
	bp = getblk(pmp->pm_devvp, bn, pmp->pm_bpcluster);	/* always succeeds */
	bzero(bp->b_un.b_addr, pmp->pm_bpcluster);
	bcopy(&dosdirtemplate, bp->b_un.b_addr, sizeof dosdirtemplate);
	denp = (struct direntry *) bp->b_un.b_addr;
	putushort(denp->deStartCluster, newcluster);
	unix2dostime(&time, &dDate, &dTime);
	putushort(denp->deDate, dDate);
	putushort(denp->deTime, dTime);
	denp++;
	putushort(denp->deStartCluster, pdep->de_StartCluster);
	putushort(denp->deDate, dDate);
	putushort(denp->deTime, dTime);
	if (error = bwrite(bp)) {
		clusterfree(pmp, newcluster, NULL);
		free(ndp->ni_pnbuf, M_NAMEI);
		deput(pdep);
		return error;
	}

	/*
	 * Now build up a directory entry pointing to the newly allocated
	 * cluster.  This will be written to an empty slot in the parent
	 * directory.
	 */
	ndep = &ndirent;
	bzero(ndep, sizeof(*ndep));
	unix2dosfn((u_char *) ndp->ni_ptr, ndep->de_Name, ndp->ni_namelen);
	unix2dostime(&time, &ndep->de_Date, &ndep->de_Time);
	ndep->de_StartCluster = newcluster;
	ndep->de_Attributes = ATTR_DIRECTORY;
	ndep->de_pmp = pmp;	/* createde() needs this	 */

	error = createde(ndep, ndp, &ndep);
	if (error) {
		clusterfree(pmp, newcluster, NULL);
	}
	else {
		ndp->ni_vp = DETOV(ndep);
	}
	free(ndp->ni_pnbuf, M_NAMEI);
#if defined(MSDOSFSDEBUG)
	printf("msdosfs_mkdir(): deput(%08x), vnode %08x\n", pdep, DETOV(pdep));
#endif				/* defined(MSDOSFSDEBUG) */
	deput(pdep);
	return error;
}

int
msdosfs_rmdir(ndp, p)
	struct nameidata *ndp;
	struct proc *p;
{
	struct denode *ddep;
	struct denode *dep;
	int error = 0;

	ddep = VTODE(ndp->ni_dvp);	/* parent dir of dir to delete	 */
	dep = VTODE(ndp->ni_vp);/* directory to delete	 */

	/*
	 * Don't let "rmdir ." go thru.
	 */
	if (ddep == dep) {
		vrele(DETOV(dep));
		deput(dep);
		return EINVAL;
	}

	/*
	 * Be sure the directory being deleted is empty.
	 */
	if (dosdirempty(dep) == 0) {
		error = ENOTEMPTY;
		goto out;
	}

	/*
	 * Delete the entry from the directory.  For dos filesystems this
	 * gets rid of the directory entry on disk, the in memory copy
	 * still exists but the de_refcnt is <= 0.  This prevents it from
	 * being found by deget().  When the deput() on dep is done we give
	 * up access and eventually msdosfs_reclaim() will be called which
	 * will remove it from the denode cache.
	 */
	if (error = removede(ndp))
		goto out;

	/*
	 * This is where we decrement the link count in the parent
	 * directory.  Since dos filesystems don't do this we just purge
	 * the name cache and let go of the parent directory denode.
	 */
	cache_purge(DETOV(ddep));
	deput(ddep);
	ndp->ni_dvp = NULL;	/* getting rid of parent dir pointer? */

	/*
	 * Truncate the directory that is being deleted.
	 */
	error = detrunc(dep, (u_long) 0, IO_SYNC);
	cache_purge(DETOV(dep));

out:	;
	if (ndp->ni_dvp)
		deput(ddep);
	deput(dep);
	return error;
}

/*
 * DOS filesystems don't know what symlinks are.
 */
int
msdosfs_symlink(ndp, vap, target, p)
	struct nameidata *ndp;
	struct vattr *vap;
	char *target;
	struct proc *p;
{
	struct denode *pdep = VTODE(ndp->ni_dvp);

	free(ndp->ni_pnbuf, M_NAMEI);
	deput(pdep);
	return EINVAL;
}

/*
 * Dummy dirents to simulate the "." and ".." entries of the root directory
 * in a dos filesystem.  Dos doesn't provide these. Note that each entry
 * must be the same size as a dos directory entry (32 bytes).
 */
struct dos_dirent {
	u_long d_fileno;
	u_short d_reclen;
	u_short d_namlen;
	u_char d_name[24];
}          rootdots[2] = {

	{
		1,		/* d_fileno			 */
		sizeof(struct direntry),	/* d_reclen			 */
		1,		/* d_namlen			 */
		"."		/* d_name			 */
	},
	{
		1,		/* d_fileno			 */
		sizeof(struct direntry),	/* d_reclen			 */
		2,		/* d_namlen			 */
		".."		/* d_name			 */
	}
};

int
msdosfs_readdir(vp, uio, cred, eofflagp, cookies, ncookies)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
	int *eofflagp;
	u_int *cookies;
	int ncookies;
{
	int error = 0;
	int diff;
	char pushout;
	long n;
	long on;
	long lost;
	long count;
	u_long cn;
	u_long fileno;
	long bias = 0;
	daddr_t bn;
	daddr_t lbn;
	struct buf *bp;
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct direntry *dentp;
	struct dirent *prev;
	struct dirent *crnt;
	u_char dirbuf[512];	/* holds converted dos directories */
	int i = 0;

#if defined(MSDOSFSDEBUG)
	printf("msdosfs_readdir(): vp %08x, uio %08x, cred %08x, eofflagp %08x\n",
	    vp, uio, cred, eofflagp);
#endif				/* defined(MSDOSFSDEBUG) */

	if (!cookies)
		ncookies = 1;
	
	/*
	 * msdosfs_readdir() won't operate properly on regular files since
	 * it does i/o only with the the filesystem vnode, and hence can
	 * retrieve the wrong block from the buffer cache for a plain file.
	 * So, fail attempts to readdir() on a plain file.
	 */
	if ((dep->de_Attributes & ATTR_DIRECTORY) == 0)
		return ENOTDIR;

	/*
	 * If the user buffer is smaller than the size of one dos directory
	 * entry or the file offset is not a multiple of the size of a
	 * directory entry, then we fail the read.
	 */
	count = uio->uio_resid & ~(sizeof(struct direntry) - 1);
	lost = uio->uio_resid - count;
	if (count < sizeof(struct direntry) ||
	    (uio->uio_offset & (sizeof(struct direntry) - 1)))
		return EINVAL;
	uio->uio_resid = count;
	uio->uio_iov->iov_len = count;

	/*
	 * If they are reading from the root directory then, we simulate
	 * the . and .. entries since these don't exist in the root
	 * directory.  We also set the offset bias to make up for having to
	 * simulate these entries. By this I mean that at file offset 64 we
	 * read the first entry in the root directory that lives on disk.
	 */
	if (dep->de_StartCluster == MSDOSFSROOT) {
		/*
		 * printf("msdosfs_readdir(): going after . or .. in root
		 * dir, offset %d\n", uio->uio_offset);
		 */
		bias = 2 * sizeof(struct direntry);
		if (uio->uio_offset < 2 * sizeof(struct direntry)) {
			if (uio->uio_offset
			    && uio->uio_offset != sizeof(struct direntry)) {
				error = EINVAL;
				goto out;
			}
			n = 1;
			if (!uio->uio_offset) {
				n = 2;
				if (cookies) {
					*cookies++ = sizeof(struct direntry);
					ncookies--;
				}
			}
			if (cookies) {
				if (ncookies-- <= 0)
					n--;
				else
					*cookies++ = 2 * sizeof(struct direntry);
			}
			
			error = uiomove((char *) rootdots + uio->uio_offset,
					n * sizeof(struct direntry), uio);
		}
	}
	while (!error && uio->uio_resid > 0 && ncookies > 0) {
		lbn = (uio->uio_offset - bias) >> pmp->pm_cnshift;
		on = (uio->uio_offset - bias) & pmp->pm_crbomask;
		n = MIN((u_long) (pmp->pm_bpcluster - on), uio->uio_resid);
		diff = dep->de_FileSize - (uio->uio_offset - bias);
		if (diff <= 0)
			return 0;
		if (diff < n)
			n = diff;
		error = pcbmap(dep, lbn, &bn, &cn);
		if (error)
			break;
		error = bread(pmp->pm_devvp, bn, pmp->pm_bpcluster, NOCRED, &bp);
		n = MIN(n, pmp->pm_bpcluster - bp->b_resid);
		if (error) {
			brelse(bp);
			return error;
		}

		/*
		 * code to convert from dos directory entries to ufs
		 * directory entries
		 */
		pushout = 0;
		dentp = (struct direntry *) (bp->b_un.b_addr + on);
		prev = 0;
		crnt = (struct dirent *) dirbuf;
		while ((char *) dentp < bp->b_un.b_addr + on + n) {
			/*
			 * printf("rd: dentp %08x prev %08x crnt %08x
			 * deName %02x attr %02x\n", dentp, prev, crnt,
			 * dentp->deName[0], dentp->deAttributes);
			 */
			/*
			 * If we have an empty entry or a slot from a
			 * deleted file, or a volume label entry just
			 * concatenate its space onto the end of the
			 * previous entry or, manufacture an empty entry if
			 * there is no previous entry.
			 */
			if (dentp->deName[0] == SLOT_EMPTY ||
			    dentp->deName[0] == SLOT_DELETED ||
			    (dentp->deAttributes & ATTR_VOLUME)) {
				if (prev) {
					prev->d_reclen += sizeof(struct direntry);
					if (cookies) {
						ncookies++;
						cookies--;
					}
				}
				else {
					prev = crnt;
					prev->d_fileno = 0;
					prev->d_reclen = sizeof(struct direntry);
					prev->d_namlen = 0;
					prev->d_name[0] = 0;
				}
			}
			else {
				/*
				 * this computation of d_fileno must match
				 * the computation of va_fileid in
				 * msdosfs_getattr
				 */
				if (dentp->deAttributes & ATTR_DIRECTORY) {
					/* if this is the root directory */
					fileno = getushort(dentp->deStartCluster);
					if (fileno == MSDOSFSROOT)
						fileno = 1;
				}
				else {
					/*
					 * if the file's dirent lives in
					 * root dir
					 */
					if ((fileno = cn) == MSDOSFSROOT)
						fileno = 1;
					fileno = (fileno << 16) |
					    ((dentp - (struct direntry *) bp->b_un.b_addr) & 0xffff);
				}
				crnt->d_fileno = fileno;
				crnt->d_reclen = sizeof(struct direntry);
				crnt->d_namlen = dos2unixfn(dentp->deName,
				    (u_char *) crnt->d_name);
				/*
				 * printf("readdir: file %s, fileno %08x,
				 * attr %02x, start %08x\n", crnt->d_name,
				 * crnt->d_fileno, dentp->deAttributes,
				 * dentp->deStartCluster);
				 */
				prev = crnt;
			}
			dentp++;
			if (cookies) {
				*cookies++ = (u_int)((char *)dentp - bp->b_un.b_addr - on)
					     + uio->uio_offset;
				ncookies--;
			}
			
			crnt = (struct dirent *) ((char *) crnt + sizeof(struct direntry));
			pushout = 1;

			/*
			 * If our intermediate buffer is full then copy its
			 * contents to user space.  I would just use the
			 * buffer the buf header points to but, I'm afraid
			 * that when we brelse() it someone else might find
			 * it in the cache and think its contents are
			 * valid.  Maybe there is a way to invalidate the
			 * buffer before brelse()'ing it.
			 */
			if ((u_char *) crnt >= &dirbuf[sizeof dirbuf]) {
				pushout = 0;
				error = uiomove(dirbuf, sizeof(dirbuf), uio);
				if (error)
					break;
				prev = 0;
				crnt = (struct dirent *) dirbuf;
			}
			if (ncookies <= 0)
				break;
		}
		if (pushout) {
			pushout = 0;
			error = uiomove(dirbuf, (char *) crnt - (char *) dirbuf,
			    uio);
		}

#if 0
		/*
		 * If we have read everything from this block or have read
		 * to end of file then we are done with this block.  Mark
		 * it to say the buffer can be reused if need be.
		 */
		if (n + on == pmp->pm_bpcluster ||
		    (uio->uio_offset - bias) == dep->de_FileSize)
			bp->b_flags |= B_AGE;
#endif /* if 0 */
		brelse(bp);
		if (n == 0)
			break;
	}
out:	;
	uio->uio_resid += lost;

	/*
	 * I don't know why we bother setting this eofflag, getdirentries()
	 * in vfs_syscalls.c doesn't bother to look at it when we return.
	 * (because NFS uses it in nfs_serv.c -- JMP)
	 */
	if (dep->de_FileSize - uio->uio_offset - bias <= 0)
		*eofflagp = 1;
	else
		*eofflagp = 0;
	return error;
}

/*
 * DOS filesystems don't know what symlinks are.
 */
int
msdosfs_readlink(vp, uio, cred)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
{
	return EINVAL;
}

int
msdosfs_abortop(ndp)
	struct nameidata *ndp;
{
	if ((ndp->ni_nameiop & (HASBUF | SAVESTART)) == HASBUF)
		FREE(ndp->ni_pnbuf, M_NAMEI);
	return 0;
}

int
msdosfs_lock(vp)
	struct vnode *vp;
{
	struct denode *dep = VTODE(vp);

	DELOCK(dep);
	return 0;
}

int
msdosfs_unlock(vp)
	struct vnode *vp;
{
	struct denode *dep = VTODE(vp);

	if (!(dep->de_flag & DELOCKED))
		panic("msdosfs_unlock: denode not locked");
	DEUNLOCK(dep);
	return 0;
}

int
msdosfs_islocked(vp)
	struct vnode *vp;
{
	return VTODE(vp)->de_flag & DELOCKED ? 1 : 0;
}

/*
 * vp - address of vnode file the file bn - which cluster we are interested
 * in mapping to a filesystem block number. vpp - returns the vnode for the
 * block special file holding the filesystem containing the file of
 * interest bnp - address of where to return the filesystem relative block
 * number
 */
int
msdosfs_bmap(vp, bn, vpp, bnp)
	struct vnode *vp;
	daddr_t bn;
	struct vnode **vpp;
	daddr_t *bnp;
{
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;

	if (vpp != NULL)
		*vpp = dep->de_devvp;
	if (bnp == NULL)
		return 0;
	return pcbmap(dep, bn << (pmp->pm_cnshift - pmp->pm_bnshift), bnp, 0);
}

int
msdosfs_strategy(bp)
	struct buf *bp;
{
	struct denode *dep = VTODE(bp->b_vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct vnode *vp;
	int error;

	if (bp->b_vp->v_type == VBLK || bp->b_vp->v_type == VCHR)
		panic("msdosfs_strategy: spec");
	/*
	 * If we don't already know the filesystem relative block number
	 * then get it using pcbmap().  If pcbmap() returns the block
	 * number as -1 then we've got a hole in the file.  DOS filesystems
	 * don't allow files with holes, so we shouldn't ever see this.
	 */
	if (bp->b_blkno == bp->b_lblkno) {
		if (error = pcbmap(dep, bp->b_lblkno, &bp->b_blkno, 0))
			return error;
		if ((long) bp->b_blkno == -1)
			clrbuf(bp);
	}
	if ((long) bp->b_blkno == -1) {
		biodone(bp);
		return 0;
	}
#ifdef DIAGNOSTIC
#endif				/* defined(DIAGNOSTIC) */
	/*
	 * Read/write the block from/to the disk that contains the desired
	 * file block.
	 */
	vp = dep->de_devvp;
	bp->b_dev = vp->v_rdev;
	(*(vp->v_op->vop_strategy)) (bp);
	return 0;
}

int
msdosfs_print(vp)
	struct vnode *vp;
{
	struct denode *dep = VTODE(vp);

	printf("tag VT_MSDOSFS, startcluster %d, dircluster %d, diroffset %d ",
	    dep->de_StartCluster, dep->de_dirclust, dep->de_diroffset);
	printf(" dev %d, %d, %s\n",
	    major(dep->de_dev), minor(dep->de_dev),
	    dep->de_flag & DELOCKED ? "(LOCKED)" : "");
	if (dep->de_spare0) {
		printf("    owner pid %d", dep->de_spare0);
		if (dep->de_spare1)
			printf(" waiting pid %d", dep->de_spare1);
		printf("\n");
	}
}

int
msdosfs_advlock(vp, id, op, fl, flags)
	struct vnode *vp;
	caddr_t id;
	int op;
	struct flock *fl;
	int flags;
{
	return EINVAL;		/* we don't do locking yet		 */
}

struct vnodeops msdosfs_vnodeops = {
	msdosfs_lookup,
	msdosfs_create,
	msdosfs_mknod,
	msdosfs_open,
	msdosfs_close,
	msdosfs_access,
	msdosfs_getattr,
	msdosfs_setattr,
	msdosfs_read,
	msdosfs_write,
	msdosfs_ioctl,
	msdosfs_select,
	msdosfs_mmap,
	msdosfs_fsync,
	msdosfs_seek,
	msdosfs_remove,
	msdosfs_link,
	msdosfs_rename,
	msdosfs_mkdir,
	msdosfs_rmdir,
	msdosfs_symlink,
	msdosfs_readdir,
	msdosfs_readlink,
	msdosfs_abortop,
	msdosfs_inactive,
	msdosfs_reclaim,
	msdosfs_lock,
	msdosfs_unlock,
	msdosfs_bmap,
	msdosfs_strategy,
	msdosfs_print,
	msdosfs_islocked,
	msdosfs_advlock,
};
