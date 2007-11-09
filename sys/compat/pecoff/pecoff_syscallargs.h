/* $NetBSD: pecoff_syscallargs.h,v 1.31 2007/11/09 15:10:33 dsl Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.30 2007/11/09 15:05:34 dsl Exp
 */

#ifndef _PECOFF_SYS_SYSCALLARGS_H_
#define	_PECOFF_SYS_SYSCALLARGS_H_

#define	PECOFF_SYS_MAXSYSARGS	8

#undef	syscallarg
#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

#undef check_syscall_args
#define check_syscall_args(call) \
	typedef char call##_check_args[sizeof (struct call##_args) \
		<= PECOFF_SYS_MAXSYSARGS * sizeof (register_t) ? 1 : -1];

struct sys_syscall_args {
	syscallarg(int) code;
	syscallarg(register_t) args[PECOFF_SYS_MAXSYSARGS];
};
#ifdef COMPAT_20
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_43
#else
#endif
#if defined(KTRACE) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_43
#else
#endif
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_20
#else
#endif
#ifdef COMPAT_30
#else
#endif
#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(_LP64)
#else
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(_LP64)
#else
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(_LP64)
#else
#endif
#if defined(NTP) || !defined(_KERNEL)
#else
#endif
#if defined(LFS) || !defined(_KERNEL)
#else
#endif

struct sys___syscall_args {
	syscallarg(quad_t) code;
	syscallarg(register_t) args[PECOFF_SYS_MAXSYSARGS];
};
#if defined(LKM) || !defined(_KERNEL)
#else	/* !LKM */
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
#else
#endif
#if defined(P1003_1B_SEMAPHORE) || (!defined(_KERNEL) && defined(_LIBC))
#else
#endif
#ifdef COMPAT_30
#endif
#ifdef COMPAT_30
#endif
#if defined(KTRACE) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_16
#else
#endif
#ifdef COMPAT_16
#else
#endif
#ifdef COMPAT_30
#else
#endif
#ifdef COMPAT_20
#else
#endif
#if defined(SYSVSEM) || !defined(_KERNEL)
#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_30
#else
#endif
#ifdef COMPAT_30
#else
#endif

/*
 * System call prototypes.
 */

int	sys_syscall(struct lwp *, void *, register_t *);

int	sys_exit(struct lwp *, void *, register_t *);

int	sys_fork(struct lwp *, void *, register_t *);

int	sys_read(struct lwp *, void *, register_t *);

int	sys_write(struct lwp *, void *, register_t *);

int	sys_open(struct lwp *, void *, register_t *);

int	sys_close(struct lwp *, void *, register_t *);

int	sys_wait4(struct lwp *, void *, register_t *);

int	sys_link(struct lwp *, void *, register_t *);

int	sys_unlink(struct lwp *, void *, register_t *);

int	sys_chdir(struct lwp *, void *, register_t *);

int	sys_fchdir(struct lwp *, void *, register_t *);

int	sys_mknod(struct lwp *, void *, register_t *);

int	sys_chmod(struct lwp *, void *, register_t *);

int	sys_chown(struct lwp *, void *, register_t *);

int	sys_obreak(struct lwp *, void *, register_t *);

#ifdef COMPAT_20
int	compat_20_sys_getfsstat(struct lwp *, void *, register_t *);

#else
#endif
#ifdef COMPAT_43
int	sys_getpid_with_ppid(struct lwp *, void *, register_t *);

#else
int	sys_getpid(struct lwp *, void *, register_t *);

#endif
int	compat_40_sys_mount(struct lwp *, void *, register_t *);

int	sys_unmount(struct lwp *, void *, register_t *);

int	sys_setuid(struct lwp *, void *, register_t *);

#ifdef COMPAT_43
int	sys_getuid_with_euid(struct lwp *, void *, register_t *);

#else
int	sys_getuid(struct lwp *, void *, register_t *);

#endif
int	sys_geteuid(struct lwp *, void *, register_t *);

int	sys_ptrace(struct lwp *, void *, register_t *);

int	sys_recvmsg(struct lwp *, void *, register_t *);

int	sys_sendmsg(struct lwp *, void *, register_t *);

int	sys_recvfrom(struct lwp *, void *, register_t *);

int	sys_accept(struct lwp *, void *, register_t *);

int	sys_getpeername(struct lwp *, void *, register_t *);

int	sys_getsockname(struct lwp *, void *, register_t *);

int	sys_access(struct lwp *, void *, register_t *);

int	sys_chflags(struct lwp *, void *, register_t *);

int	sys_fchflags(struct lwp *, void *, register_t *);

int	sys_sync(struct lwp *, void *, register_t *);

int	sys_kill(struct lwp *, void *, register_t *);

int	sys_getppid(struct lwp *, void *, register_t *);

int	sys_dup(struct lwp *, void *, register_t *);

int	sys_pipe(struct lwp *, void *, register_t *);

int	sys_getegid(struct lwp *, void *, register_t *);

int	sys_profil(struct lwp *, void *, register_t *);

#if defined(KTRACE) || !defined(_KERNEL)
int	sys_ktrace(struct lwp *, void *, register_t *);

#else
#endif
#ifdef COMPAT_43
int	sys_getgid_with_egid(struct lwp *, void *, register_t *);

#else
int	sys_getgid(struct lwp *, void *, register_t *);

#endif
int	sys___getlogin(struct lwp *, void *, register_t *);

int	sys___setlogin(struct lwp *, void *, register_t *);

int	sys_acct(struct lwp *, void *, register_t *);

int	sys_ioctl(struct lwp *, void *, register_t *);

int	sys_revoke(struct lwp *, void *, register_t *);

int	sys_symlink(struct lwp *, void *, register_t *);

int	sys_readlink(struct lwp *, void *, register_t *);

int	sys_execve(struct lwp *, void *, register_t *);

int	sys_umask(struct lwp *, void *, register_t *);

int	sys_chroot(struct lwp *, void *, register_t *);

int	sys_vfork(struct lwp *, void *, register_t *);

int	sys_sbrk(struct lwp *, void *, register_t *);

int	sys_sstk(struct lwp *, void *, register_t *);

int	sys_ovadvise(struct lwp *, void *, register_t *);

int	sys_munmap(struct lwp *, void *, register_t *);

int	sys_mprotect(struct lwp *, void *, register_t *);

int	sys_madvise(struct lwp *, void *, register_t *);

int	sys_mincore(struct lwp *, void *, register_t *);

int	sys_getgroups(struct lwp *, void *, register_t *);

int	sys_setgroups(struct lwp *, void *, register_t *);

int	sys_getpgrp(struct lwp *, void *, register_t *);

int	sys_setpgid(struct lwp *, void *, register_t *);

int	sys_setitimer(struct lwp *, void *, register_t *);

int	sys_getitimer(struct lwp *, void *, register_t *);

int	sys_dup2(struct lwp *, void *, register_t *);

int	sys_fcntl(struct lwp *, void *, register_t *);

int	sys_select(struct lwp *, void *, register_t *);

int	sys_fsync(struct lwp *, void *, register_t *);

int	sys_setpriority(struct lwp *, void *, register_t *);

int	compat_30_sys_socket(struct lwp *, void *, register_t *);

int	sys_connect(struct lwp *, void *, register_t *);

int	sys_getpriority(struct lwp *, void *, register_t *);

int	sys_bind(struct lwp *, void *, register_t *);

int	sys_setsockopt(struct lwp *, void *, register_t *);

int	sys_listen(struct lwp *, void *, register_t *);

int	sys_gettimeofday(struct lwp *, void *, register_t *);

int	sys_getrusage(struct lwp *, void *, register_t *);

int	sys_getsockopt(struct lwp *, void *, register_t *);

int	sys_readv(struct lwp *, void *, register_t *);

int	sys_writev(struct lwp *, void *, register_t *);

int	sys_settimeofday(struct lwp *, void *, register_t *);

int	sys_fchown(struct lwp *, void *, register_t *);

int	sys_fchmod(struct lwp *, void *, register_t *);

int	sys_setreuid(struct lwp *, void *, register_t *);

int	sys_setregid(struct lwp *, void *, register_t *);

int	sys_rename(struct lwp *, void *, register_t *);

int	sys_flock(struct lwp *, void *, register_t *);

int	sys_mkfifo(struct lwp *, void *, register_t *);

int	sys_sendto(struct lwp *, void *, register_t *);

int	sys_shutdown(struct lwp *, void *, register_t *);

int	sys_socketpair(struct lwp *, void *, register_t *);

int	sys_mkdir(struct lwp *, void *, register_t *);

int	sys_rmdir(struct lwp *, void *, register_t *);

int	sys_utimes(struct lwp *, void *, register_t *);

int	sys_adjtime(struct lwp *, void *, register_t *);

int	sys_setsid(struct lwp *, void *, register_t *);

int	sys_quotactl(struct lwp *, void *, register_t *);

#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
int	sys_nfssvc(struct lwp *, void *, register_t *);

#else
#endif
int	compat_20_sys_statfs(struct lwp *, void *, register_t *);

#ifdef COMPAT_20
int	compat_20_sys_fstatfs(struct lwp *, void *, register_t *);

#else
#endif
#ifdef COMPAT_30
int	compat_30_sys_getfh(struct lwp *, void *, register_t *);

#else
#endif
int	sys_sysarch(struct lwp *, void *, register_t *);

#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(_LP64)
#else
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(_LP64)
#else
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(_LP64)
#else
#endif
int	sys_pread(struct lwp *, void *, register_t *);

int	sys_pwrite(struct lwp *, void *, register_t *);

#if defined(NTP) || !defined(_KERNEL)
int	sys_ntp_adjtime(struct lwp *, void *, register_t *);

#else
#endif
int	sys_setgid(struct lwp *, void *, register_t *);

int	sys_setegid(struct lwp *, void *, register_t *);

int	sys_seteuid(struct lwp *, void *, register_t *);

#if defined(LFS) || !defined(_KERNEL)
int	sys_lfs_bmapv(struct lwp *, void *, register_t *);

int	sys_lfs_markv(struct lwp *, void *, register_t *);

int	sys_lfs_segclean(struct lwp *, void *, register_t *);

int	sys_lfs_segwait(struct lwp *, void *, register_t *);

#else
#endif
int	sys_pathconf(struct lwp *, void *, register_t *);

int	sys_fpathconf(struct lwp *, void *, register_t *);

int	sys_getrlimit(struct lwp *, void *, register_t *);

int	sys_setrlimit(struct lwp *, void *, register_t *);

int	sys_mmap(struct lwp *, void *, register_t *);

int	sys___syscall(struct lwp *, void *, register_t *);

int	sys_lseek(struct lwp *, void *, register_t *);

int	sys_truncate(struct lwp *, void *, register_t *);

int	sys_ftruncate(struct lwp *, void *, register_t *);

int	sys___sysctl(struct lwp *, void *, register_t *);

int	sys_mlock(struct lwp *, void *, register_t *);

int	sys_munlock(struct lwp *, void *, register_t *);

int	sys_undelete(struct lwp *, void *, register_t *);

int	sys_futimes(struct lwp *, void *, register_t *);

int	sys_getpgid(struct lwp *, void *, register_t *);

int	sys_reboot(struct lwp *, void *, register_t *);

int	sys_poll(struct lwp *, void *, register_t *);

#if defined(LKM) || !defined(_KERNEL)
int	sys_lkmnosys(struct lwp *, void *, register_t *);

#else	/* !LKM */
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
int	sys_semget(struct lwp *, void *, register_t *);

int	sys_semop(struct lwp *, void *, register_t *);

int	sys_semconfig(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
int	sys_msgget(struct lwp *, void *, register_t *);

int	sys_msgsnd(struct lwp *, void *, register_t *);

int	sys_msgrcv(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
int	sys_shmat(struct lwp *, void *, register_t *);

int	sys_shmdt(struct lwp *, void *, register_t *);

int	sys_shmget(struct lwp *, void *, register_t *);

#else
#endif
int	sys_clock_gettime(struct lwp *, void *, register_t *);

int	sys_clock_settime(struct lwp *, void *, register_t *);

int	sys_clock_getres(struct lwp *, void *, register_t *);

int	sys_timer_create(struct lwp *, void *, register_t *);

int	sys_timer_delete(struct lwp *, void *, register_t *);

int	sys_timer_settime(struct lwp *, void *, register_t *);

int	sys_timer_gettime(struct lwp *, void *, register_t *);

int	sys_timer_getoverrun(struct lwp *, void *, register_t *);

int	sys_nanosleep(struct lwp *, void *, register_t *);

int	sys_fdatasync(struct lwp *, void *, register_t *);

int	sys_mlockall(struct lwp *, void *, register_t *);

int	sys_munlockall(struct lwp *, void *, register_t *);

int	sys___sigtimedwait(struct lwp *, void *, register_t *);

#if defined(P1003_1B_SEMAPHORE) || (!defined(_KERNEL) && defined(_LIBC))
int	sys__ksem_init(struct lwp *, void *, register_t *);

int	sys__ksem_open(struct lwp *, void *, register_t *);

int	sys__ksem_unlink(struct lwp *, void *, register_t *);

int	sys__ksem_close(struct lwp *, void *, register_t *);

int	sys__ksem_post(struct lwp *, void *, register_t *);

int	sys__ksem_wait(struct lwp *, void *, register_t *);

int	sys__ksem_trywait(struct lwp *, void *, register_t *);

int	sys__ksem_getvalue(struct lwp *, void *, register_t *);

int	sys__ksem_destroy(struct lwp *, void *, register_t *);

#else
#endif
int	sys___posix_rename(struct lwp *, void *, register_t *);

int	sys_swapctl(struct lwp *, void *, register_t *);

#ifdef COMPAT_30
int	compat_30_sys_getdents(struct lwp *, void *, register_t *);

#endif
int	sys_minherit(struct lwp *, void *, register_t *);

int	sys_lchmod(struct lwp *, void *, register_t *);

int	sys_lchown(struct lwp *, void *, register_t *);

int	sys_lutimes(struct lwp *, void *, register_t *);

int	sys___msync13(struct lwp *, void *, register_t *);

#ifdef COMPAT_30
int	compat_30_sys___stat13(struct lwp *, void *, register_t *);

int	compat_30_sys___fstat13(struct lwp *, void *, register_t *);

int	compat_30_sys___lstat13(struct lwp *, void *, register_t *);

#endif
int	sys___sigaltstack14(struct lwp *, void *, register_t *);

int	sys___vfork14(struct lwp *, void *, register_t *);

int	sys___posix_chown(struct lwp *, void *, register_t *);

int	sys___posix_fchown(struct lwp *, void *, register_t *);

int	sys___posix_lchown(struct lwp *, void *, register_t *);

int	sys_getsid(struct lwp *, void *, register_t *);

int	sys___clone(struct lwp *, void *, register_t *);

#if defined(KTRACE) || !defined(_KERNEL)
int	sys_fktrace(struct lwp *, void *, register_t *);

#else
#endif
int	sys_preadv(struct lwp *, void *, register_t *);

int	sys_pwritev(struct lwp *, void *, register_t *);

#ifdef COMPAT_16
int	compat_16_sys___sigaction14(struct lwp *, void *, register_t *);

#else
#endif
int	sys___sigpending14(struct lwp *, void *, register_t *);

int	sys___sigprocmask14(struct lwp *, void *, register_t *);

int	sys___sigsuspend14(struct lwp *, void *, register_t *);

#ifdef COMPAT_16
int	compat_16_sys___sigreturn14(struct lwp *, void *, register_t *);

#else
#endif
int	sys___getcwd(struct lwp *, void *, register_t *);

int	sys_fchroot(struct lwp *, void *, register_t *);

#ifdef COMPAT_30
int	compat_30_sys_fhopen(struct lwp *, void *, register_t *);

int	compat_30_sys_fhstat(struct lwp *, void *, register_t *);

#else
#endif
#ifdef COMPAT_20
int	compat_20_sys_fhstatfs(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVSEM) || !defined(_KERNEL)
int	sys_____semctl13(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
int	sys___msgctl13(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
int	sys___shmctl13(struct lwp *, void *, register_t *);

#else
#endif
int	sys_lchflags(struct lwp *, void *, register_t *);

int	sys_issetugid(struct lwp *, void *, register_t *);

int	sys_utrace(struct lwp *, void *, register_t *);

int	sys_getcontext(struct lwp *, void *, register_t *);

int	sys_setcontext(struct lwp *, void *, register_t *);

int	sys__lwp_create(struct lwp *, void *, register_t *);

int	sys__lwp_exit(struct lwp *, void *, register_t *);

int	sys__lwp_self(struct lwp *, void *, register_t *);

int	sys__lwp_wait(struct lwp *, void *, register_t *);

int	sys__lwp_suspend(struct lwp *, void *, register_t *);

int	sys__lwp_continue(struct lwp *, void *, register_t *);

int	sys__lwp_wakeup(struct lwp *, void *, register_t *);

int	sys__lwp_getprivate(struct lwp *, void *, register_t *);

int	sys__lwp_setprivate(struct lwp *, void *, register_t *);

int	sys___sigaction_sigtramp(struct lwp *, void *, register_t *);

int	sys_pmc_get_info(struct lwp *, void *, register_t *);

int	sys_pmc_control(struct lwp *, void *, register_t *);

int	sys_rasctl(struct lwp *, void *, register_t *);

int	sys_kqueue(struct lwp *, void *, register_t *);

int	sys_kevent(struct lwp *, void *, register_t *);

int	sys_fsync_range(struct lwp *, void *, register_t *);

int	sys_uuidgen(struct lwp *, void *, register_t *);

int	sys_getvfsstat(struct lwp *, void *, register_t *);

int	sys_statvfs1(struct lwp *, void *, register_t *);

int	sys_fstatvfs1(struct lwp *, void *, register_t *);

#ifdef COMPAT_30
int	compat_30_sys_fhstatvfs1(struct lwp *, void *, register_t *);

#else
#endif
int	sys_extattrctl(struct lwp *, void *, register_t *);

int	sys_extattr_set_file(struct lwp *, void *, register_t *);

int	sys_extattr_get_file(struct lwp *, void *, register_t *);

int	sys_extattr_delete_file(struct lwp *, void *, register_t *);

int	sys_extattr_set_fd(struct lwp *, void *, register_t *);

int	sys_extattr_get_fd(struct lwp *, void *, register_t *);

int	sys_extattr_delete_fd(struct lwp *, void *, register_t *);

int	sys_extattr_set_link(struct lwp *, void *, register_t *);

int	sys_extattr_get_link(struct lwp *, void *, register_t *);

int	sys_extattr_delete_link(struct lwp *, void *, register_t *);

int	sys_extattr_list_fd(struct lwp *, void *, register_t *);

int	sys_extattr_list_file(struct lwp *, void *, register_t *);

int	sys_extattr_list_link(struct lwp *, void *, register_t *);

int	sys_pselect(struct lwp *, void *, register_t *);

int	sys_pollts(struct lwp *, void *, register_t *);

int	sys_setxattr(struct lwp *, void *, register_t *);

int	sys_lsetxattr(struct lwp *, void *, register_t *);

int	sys_fsetxattr(struct lwp *, void *, register_t *);

int	sys_getxattr(struct lwp *, void *, register_t *);

int	sys_lgetxattr(struct lwp *, void *, register_t *);

int	sys_fgetxattr(struct lwp *, void *, register_t *);

int	sys_listxattr(struct lwp *, void *, register_t *);

int	sys_llistxattr(struct lwp *, void *, register_t *);

int	sys_flistxattr(struct lwp *, void *, register_t *);

int	sys_removexattr(struct lwp *, void *, register_t *);

int	sys_lremovexattr(struct lwp *, void *, register_t *);

int	sys_fremovexattr(struct lwp *, void *, register_t *);

int	sys___stat30(struct lwp *, void *, register_t *);

int	sys___fstat30(struct lwp *, void *, register_t *);

int	sys___lstat30(struct lwp *, void *, register_t *);

int	sys___getdents30(struct lwp *, void *, register_t *);

int	sys_posix_fadvise(struct lwp *, void *, register_t *);

#ifdef COMPAT_30
int	compat_30_sys___fhstat30(struct lwp *, void *, register_t *);

#else
#endif
int	sys___ntp_gettime30(struct lwp *, void *, register_t *);

int	sys___socket30(struct lwp *, void *, register_t *);

int	sys___getfh30(struct lwp *, void *, register_t *);

int	sys___fhopen40(struct lwp *, void *, register_t *);

int	sys___fhstatvfs140(struct lwp *, void *, register_t *);

#endif /* _PECOFF_SYS_SYSCALLARGS_H_ */
