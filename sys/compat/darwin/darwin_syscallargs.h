/* $NetBSD: darwin_syscallargs.h,v 1.53 2007/12/20 23:10:42 dsl Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.35 2007/12/12 21:24:36 dsl Exp
 */

#ifndef _DARWIN_SYS_SYSCALLARGS_H_
#define	_DARWIN_SYS_SYSCALLARGS_H_

#define	DARWIN_SYS_MAXSYSARGS	8

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
		<= DARWIN_SYS_MAXSYSARGS * sizeof (register_t) ? 1 : -1];

struct sys_syscall_args;

struct sys_exit_args;

struct sys_read_args;

struct sys_write_args;

struct sys_open_args;

struct sys_close_args;

struct sys_wait4_args;

struct compat_43_sys_creat_args;

struct sys_link_args;

struct sys_unlink_args;

struct sys_chdir_args;

struct sys_fchdir_args;

struct darwin_sys_mknod_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
	syscallarg(dev_t) dev;
};
check_syscall_args(darwin_sys_mknod)

struct sys_chmod_args;

struct sys_chown_args;

struct sys_obreak_args;

struct darwin_sys_getfsstat_args {
	syscallarg(struct darwin_statfs *) buf;
	syscallarg(long) bufsize;
	syscallarg(int) flags;
};
check_syscall_args(darwin_sys_getfsstat)

struct compat_43_sys_lseek_args;

struct compat_40_sys_mount_args;

struct sys_unmount_args;

struct sys_setuid_args;
#ifdef COMPAT_43
#else
#endif

struct darwin_sys_ptrace_args {
	syscallarg(int) req;
	syscallarg(pid_t) pid;
	syscallarg(void *) addr;
	syscallarg(int) data;
};
check_syscall_args(darwin_sys_ptrace)

struct sys_recvmsg_args;

struct sys_sendmsg_args;

struct darwin_sys_recvfrom_args {
	syscallarg(int) s;
	syscallarg(void *) buf;
	syscallarg(size_t) len;
	syscallarg(int) flags;
	syscallarg(struct sockaddr *) from;
	syscallarg(unsigned int *) fromlenaddr;
};
check_syscall_args(darwin_sys_recvfrom)

struct darwin_sys_accept_args {
	syscallarg(int) s;
	syscallarg(struct sockaddr *) name;
	syscallarg(unsigned int *) anamelen;
};
check_syscall_args(darwin_sys_accept)

struct darwin_sys_getpeername_args {
	syscallarg(int) fdes;
	syscallarg(struct sockaddr *) asa;
	syscallarg(unsigned int *) alen;
};
check_syscall_args(darwin_sys_getpeername)

struct darwin_sys_getsockname_args {
	syscallarg(int) fdes;
	syscallarg(struct sockaddr *) asa;
	syscallarg(unsigned int *) alen;
};
check_syscall_args(darwin_sys_getsockname)

struct sys_access_args;

struct sys_chflags_args;

struct sys_fchflags_args;

struct sys_kill_args;

struct compat_43_sys_stat_args;

struct compat_43_sys_lstat_args;

struct sys_dup_args;

struct sys_profil_args;
#if defined(KTRACE) || !defined(_KERNEL)

struct sys_ktrace_args;
#else
#endif

struct darwin_sys_sigaction_args {
	syscallarg(int) signum;
	syscallarg(struct darwin___sigaction *) nsa;
	syscallarg(struct sigaction13 *) osa;
};
check_syscall_args(darwin_sys_sigaction)
#ifdef COMPAT_43
#else
#endif

struct darwin_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(sigset13_t *) set;
	syscallarg(sigset13_t *) oset;
};
check_syscall_args(darwin_sys_sigprocmask)

struct sys___getlogin_args;

struct sys___setlogin_args;

struct sys_acct_args;

struct compat_13_sys_sigaltstack_args;

struct darwin_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(void *) data;
};
check_syscall_args(darwin_sys_ioctl)

struct sys_reboot_args;

struct sys_revoke_args;

struct sys_symlink_args;

struct sys_readlink_args;

struct sys_execve_args;

struct sys_umask_args;

struct sys_chroot_args;

struct compat_43_sys_fstat_args;

struct compat_12_sys_msync_args;

struct sys_sbrk_args;

struct sys_sstk_args;

struct sys_mmap_args;

struct sys_ovadvise_args;

struct sys_munmap_args;

struct sys_mprotect_args;

struct sys_madvise_args;

struct sys_mincore_args;

struct sys_getgroups_args;

struct sys_setgroups_args;

struct sys_setpgid_args;

struct sys_setitimer_args;

struct compat_12_sys_swapon_args;

struct sys_getitimer_args;

struct compat_43_sys_gethostname_args;

struct compat_43_sys_sethostname_args;

struct sys_dup2_args;

struct sys_fcntl_args;

struct sys_select_args;

struct sys_fsync_args;

struct sys_setpriority_args;

struct darwin_sys_socket_args {
	syscallarg(int) domain;
	syscallarg(int) type;
	syscallarg(int) protocol;
};
check_syscall_args(darwin_sys_socket)

struct darwin_sys_connect_args {
	syscallarg(int) s;
	syscallarg(const struct sockaddr *) name;
	syscallarg(unsigned int) namelen;
};
check_syscall_args(darwin_sys_connect)

struct compat_43_sys_accept_args;

struct sys_getpriority_args;

struct compat_43_sys_send_args;

struct compat_43_sys_recv_args;

struct darwin_sys_sigreturn_x2_args {
	syscallarg(struct darwin_ucontext *) uctx;
};
check_syscall_args(darwin_sys_sigreturn_x2)

struct darwin_sys_bind_args {
	syscallarg(int) s;
	syscallarg(const struct sockaddr *) name;
	syscallarg(unsigned int) namelen;
};
check_syscall_args(darwin_sys_bind)

struct sys_setsockopt_args;

struct sys_listen_args;

struct compat_43_sys_sigvec_args;

struct compat_43_sys_sigblock_args;

struct compat_43_sys_sigsetmask_args;

struct compat_13_sys_sigsuspend_args;

struct compat_43_sys_sigstack_args;

struct compat_43_sys_recvmsg_args;

struct compat_43_sys_sendmsg_args;

struct sys_gettimeofday_args;

struct sys_getrusage_args;

struct sys_getsockopt_args;

struct sys_readv_args;

struct sys_writev_args;

struct sys_settimeofday_args;

struct sys_fchown_args;

struct sys_fchmod_args;

struct compat_43_sys_recvfrom_args;

struct sys_setreuid_args;

struct sys_setregid_args;

struct sys_rename_args;

struct compat_43_sys_truncate_args;

struct compat_43_sys_ftruncate_args;

struct sys_flock_args;

struct sys_mkfifo_args;

struct darwin_sys_sendto_args {
	syscallarg(int) s;
	syscallarg(const void *) buf;
	syscallarg(size_t) len;
	syscallarg(int) flags;
	syscallarg(const struct sockaddr *) to;
	syscallarg(unsigned int) tolen;
};
check_syscall_args(darwin_sys_sendto)

struct sys_shutdown_args;

struct sys_socketpair_args;

struct sys_mkdir_args;

struct sys_rmdir_args;

struct sys_utimes_args;

struct sys_adjtime_args;

struct compat_43_sys_getpeername_args;

struct compat_43_sys_getrlimit_args;

struct compat_43_sys_setrlimit_args;

struct compat_43_sys_killpg_args;

struct compat_43_sys_getsockname_args;
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)

struct sys_nfssvc_args;
#else
#endif

struct compat_43_sys_getdirentries_args;

struct darwin_sys_statfs_args {
	syscallarg(const char *) path;
	syscallarg(struct darwin_statfs *) buf;
};
check_syscall_args(darwin_sys_statfs)

struct darwin_sys_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(struct darwin_statfs *) buf;
};
check_syscall_args(darwin_sys_fstatfs)
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)

struct compat_30_sys_getfh_args;
#else
#endif

struct compat_09_sys_getdomainname_args;

struct compat_09_sys_setdomainname_args;

struct darwin_sys_kdebug_trace_args {
	syscallarg(int) debugid;
	syscallarg(int) arg1;
	syscallarg(int) arg2;
	syscallarg(int) arg3;
	syscallarg(int) arg4;
	syscallarg(int) arg5;
};
check_syscall_args(darwin_sys_kdebug_trace)

struct sys_setgid_args;

struct sys_setegid_args;

struct sys_seteuid_args;

struct darwin_sys_sigreturn_args {
	syscallarg(struct darwin_ucontext *) uctx;
	syscallarg(int) ucvers;
};
check_syscall_args(darwin_sys_sigreturn)

struct darwin_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat12 *) ub;
};
check_syscall_args(darwin_sys_stat)

struct darwin_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct stat12 *) sb;
};
check_syscall_args(darwin_sys_fstat)

struct darwin_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat12 *) ub;
};
check_syscall_args(darwin_sys_lstat)

struct sys_pathconf_args;

struct sys_fpathconf_args;

struct sys_getrlimit_args;

struct sys_setrlimit_args;

struct compat_12_sys_getdirentries_args;

struct sys_mmap_args;

struct darwin_sys_lseek_args {
	syscallarg(int) fd;
	syscallarg(long) off1;
	syscallarg(long) off2;
	syscallarg(int) whence;
};
check_syscall_args(darwin_sys_lseek)

struct sys_truncate_args;

struct sys_ftruncate_args;

struct darwin_sys___sysctl_args {
	syscallarg(int *) name;
	syscallarg(u_int) namelen;
	syscallarg(void *) oldp;
	syscallarg(size_t *) oldlenp;
	syscallarg(void *) newp;
	syscallarg(size_t) newlen;
};
check_syscall_args(darwin_sys___sysctl)

struct sys_mlock_args;

struct sys_munlock_args;

struct sys_undelete_args;

struct darwin_sys_getattrlist_args {
	syscallarg(const char *) path;
	syscallarg(struct darwin_attrlist *) alist;
	syscallarg(void *) attributes;
	syscallarg(size_t) buflen;
	syscallarg(unsigned long) options;
};
check_syscall_args(darwin_sys_getattrlist)

struct darwin_sys_load_shared_file_args {
	syscallarg(char *) filename;
	syscallarg(void *) addr;
	syscallarg(u_long) len;
	syscallarg(void **) base;
	syscallarg(int) count;
	syscallarg(mach_sf_mapping_t *) mappings;
	syscallarg(int *) flags;
};
check_syscall_args(darwin_sys_load_shared_file)

struct darwin_sys_pthread_exit_args {
	syscallarg(void *) value_ptr;
};
check_syscall_args(darwin_sys_pthread_exit)

struct darwin_sys_utrace_args {
	syscallarg(void *) addr;
	syscallarg(size_t) len;
};
check_syscall_args(darwin_sys_utrace)

struct darwin_sys_audit_args {
	syscallarg(void *) record;
	syscallarg(int) len;
};
check_syscall_args(darwin_sys_audit)

struct darwin_sys_auditon_args {
	syscallarg(int) cmd;
	syscallarg(void *) data;
	syscallarg(int) len;
};
check_syscall_args(darwin_sys_auditon)

struct darwin_sys_getauid_args {
	syscallarg(darwin_au_id_t *) auid;
};
check_syscall_args(darwin_sys_getauid)

struct darwin_sys_setauid_args {
	syscallarg(darwin_au_id_t *) auid;
};
check_syscall_args(darwin_sys_setauid)

struct darwin_sys_getauditinfo_args {
	syscallarg(struct darwin_auditinfo *) auditinfo;
};
check_syscall_args(darwin_sys_getauditinfo)

struct darwin_sys_setauditinfo_args {
	syscallarg(struct darwin_auditinfo *) auditinfo;
};
check_syscall_args(darwin_sys_setauditinfo)

struct darwin_sys_getaudit_addr_args {
	syscallarg(struct darwin_auditinfo_addr *) auditinfo_addr;
	syscallarg(int) len;
};
check_syscall_args(darwin_sys_getaudit_addr)

struct darwin_sys_setaudit_addr_args {
	syscallarg(struct darwin_auditinfo_addr *) auditinfo_addr;
	syscallarg(int) len;
};
check_syscall_args(darwin_sys_setaudit_addr)

struct darwin_sys_auditctl_args {
	syscallarg(char *) path;
};
check_syscall_args(darwin_sys_auditctl)

/*
 * System call prototypes.
 */

int	sys_syscall(struct lwp *, const struct sys_syscall_args *, register_t *);

int	sys_exit(struct lwp *, const struct sys_exit_args *, register_t *);

int	darwin_sys_fork(struct lwp *, const void *, register_t *);

int	sys_read(struct lwp *, const struct sys_read_args *, register_t *);

int	sys_write(struct lwp *, const struct sys_write_args *, register_t *);

int	sys_open(struct lwp *, const struct sys_open_args *, register_t *);

int	sys_close(struct lwp *, const struct sys_close_args *, register_t *);

int	sys_wait4(struct lwp *, const struct sys_wait4_args *, register_t *);

int	compat_43_sys_creat(struct lwp *, const struct compat_43_sys_creat_args *, register_t *);

int	sys_link(struct lwp *, const struct sys_link_args *, register_t *);

int	sys_unlink(struct lwp *, const struct sys_unlink_args *, register_t *);

int	sys_chdir(struct lwp *, const struct sys_chdir_args *, register_t *);

int	sys_fchdir(struct lwp *, const struct sys_fchdir_args *, register_t *);

int	darwin_sys_mknod(struct lwp *, const struct darwin_sys_mknod_args *, register_t *);

int	sys_chmod(struct lwp *, const struct sys_chmod_args *, register_t *);

int	sys_chown(struct lwp *, const struct sys_chown_args *, register_t *);

int	sys_obreak(struct lwp *, const struct sys_obreak_args *, register_t *);

int	darwin_sys_getfsstat(struct lwp *, const struct darwin_sys_getfsstat_args *, register_t *);

int	compat_43_sys_lseek(struct lwp *, const struct compat_43_sys_lseek_args *, register_t *);

int	darwin_sys_getpid(struct lwp *, const void *, register_t *);

int	compat_40_sys_mount(struct lwp *, const struct compat_40_sys_mount_args *, register_t *);

int	sys_unmount(struct lwp *, const struct sys_unmount_args *, register_t *);

int	sys_setuid(struct lwp *, const struct sys_setuid_args *, register_t *);

#ifdef COMPAT_43
int	sys_getuid_with_euid(struct lwp *, const void *, register_t *);

#else
int	sys_getuid(struct lwp *, const void *, register_t *);

#endif
int	sys_geteuid(struct lwp *, const void *, register_t *);

int	darwin_sys_ptrace(struct lwp *, const struct darwin_sys_ptrace_args *, register_t *);

int	sys_recvmsg(struct lwp *, const struct sys_recvmsg_args *, register_t *);

int	sys_sendmsg(struct lwp *, const struct sys_sendmsg_args *, register_t *);

int	darwin_sys_recvfrom(struct lwp *, const struct darwin_sys_recvfrom_args *, register_t *);

int	darwin_sys_accept(struct lwp *, const struct darwin_sys_accept_args *, register_t *);

int	darwin_sys_getpeername(struct lwp *, const struct darwin_sys_getpeername_args *, register_t *);

int	darwin_sys_getsockname(struct lwp *, const struct darwin_sys_getsockname_args *, register_t *);

int	sys_access(struct lwp *, const struct sys_access_args *, register_t *);

int	sys_chflags(struct lwp *, const struct sys_chflags_args *, register_t *);

int	sys_fchflags(struct lwp *, const struct sys_fchflags_args *, register_t *);

int	sys_sync(struct lwp *, const void *, register_t *);

int	sys_kill(struct lwp *, const struct sys_kill_args *, register_t *);

int	compat_43_sys_stat(struct lwp *, const struct compat_43_sys_stat_args *, register_t *);

int	sys_getppid(struct lwp *, const void *, register_t *);

int	compat_43_sys_lstat(struct lwp *, const struct compat_43_sys_lstat_args *, register_t *);

int	sys_dup(struct lwp *, const struct sys_dup_args *, register_t *);

int	sys_pipe(struct lwp *, const void *, register_t *);

int	sys_getegid(struct lwp *, const void *, register_t *);

int	sys_profil(struct lwp *, const struct sys_profil_args *, register_t *);

#if defined(KTRACE) || !defined(_KERNEL)
int	sys_ktrace(struct lwp *, const struct sys_ktrace_args *, register_t *);

#else
#endif
int	darwin_sys_sigaction(struct lwp *, const struct darwin_sys_sigaction_args *, register_t *);

#ifdef COMPAT_43
int	sys_getgid_with_egid(struct lwp *, const void *, register_t *);

#else
int	sys_getgid(struct lwp *, const void *, register_t *);

#endif
int	darwin_sys_sigprocmask(struct lwp *, const struct darwin_sys_sigprocmask_args *, register_t *);

int	sys___getlogin(struct lwp *, const struct sys___getlogin_args *, register_t *);

int	sys___setlogin(struct lwp *, const struct sys___setlogin_args *, register_t *);

int	sys_acct(struct lwp *, const struct sys_acct_args *, register_t *);

int	compat_13_sys_sigpending(struct lwp *, const void *, register_t *);

int	compat_13_sys_sigaltstack(struct lwp *, const struct compat_13_sys_sigaltstack_args *, register_t *);

int	darwin_sys_ioctl(struct lwp *, const struct darwin_sys_ioctl_args *, register_t *);

int	sys_reboot(struct lwp *, const struct sys_reboot_args *, register_t *);

int	sys_revoke(struct lwp *, const struct sys_revoke_args *, register_t *);

int	sys_symlink(struct lwp *, const struct sys_symlink_args *, register_t *);

int	sys_readlink(struct lwp *, const struct sys_readlink_args *, register_t *);

int	sys_execve(struct lwp *, const struct sys_execve_args *, register_t *);

int	sys_umask(struct lwp *, const struct sys_umask_args *, register_t *);

int	sys_chroot(struct lwp *, const struct sys_chroot_args *, register_t *);

int	compat_43_sys_fstat(struct lwp *, const struct compat_43_sys_fstat_args *, register_t *);

int	compat_43_sys_getpagesize(struct lwp *, const void *, register_t *);

int	compat_12_sys_msync(struct lwp *, const struct compat_12_sys_msync_args *, register_t *);

int	darwin_sys_vfork(struct lwp *, const void *, register_t *);

int	sys_sbrk(struct lwp *, const struct sys_sbrk_args *, register_t *);

int	sys_sstk(struct lwp *, const struct sys_sstk_args *, register_t *);

int	sys_mmap(struct lwp *, const struct sys_mmap_args *, register_t *);

int	sys_ovadvise(struct lwp *, const struct sys_ovadvise_args *, register_t *);

int	sys_munmap(struct lwp *, const struct sys_munmap_args *, register_t *);

int	sys_mprotect(struct lwp *, const struct sys_mprotect_args *, register_t *);

int	sys_madvise(struct lwp *, const struct sys_madvise_args *, register_t *);

int	sys_mincore(struct lwp *, const struct sys_mincore_args *, register_t *);

int	sys_getgroups(struct lwp *, const struct sys_getgroups_args *, register_t *);

int	sys_setgroups(struct lwp *, const struct sys_setgroups_args *, register_t *);

int	sys_getpgrp(struct lwp *, const void *, register_t *);

int	sys_setpgid(struct lwp *, const struct sys_setpgid_args *, register_t *);

int	sys_setitimer(struct lwp *, const struct sys_setitimer_args *, register_t *);

int	compat_43_sys_wait(struct lwp *, const void *, register_t *);

int	compat_12_sys_swapon(struct lwp *, const struct compat_12_sys_swapon_args *, register_t *);

int	sys_getitimer(struct lwp *, const struct sys_getitimer_args *, register_t *);

int	compat_43_sys_gethostname(struct lwp *, const struct compat_43_sys_gethostname_args *, register_t *);

int	compat_43_sys_sethostname(struct lwp *, const struct compat_43_sys_sethostname_args *, register_t *);

int	compat_43_sys_getdtablesize(struct lwp *, const void *, register_t *);

int	sys_dup2(struct lwp *, const struct sys_dup2_args *, register_t *);

int	sys_fcntl(struct lwp *, const struct sys_fcntl_args *, register_t *);

int	sys_select(struct lwp *, const struct sys_select_args *, register_t *);

int	sys_fsync(struct lwp *, const struct sys_fsync_args *, register_t *);

int	sys_setpriority(struct lwp *, const struct sys_setpriority_args *, register_t *);

int	darwin_sys_socket(struct lwp *, const struct darwin_sys_socket_args *, register_t *);

int	darwin_sys_connect(struct lwp *, const struct darwin_sys_connect_args *, register_t *);

int	compat_43_sys_accept(struct lwp *, const struct compat_43_sys_accept_args *, register_t *);

int	sys_getpriority(struct lwp *, const struct sys_getpriority_args *, register_t *);

int	compat_43_sys_send(struct lwp *, const struct compat_43_sys_send_args *, register_t *);

int	compat_43_sys_recv(struct lwp *, const struct compat_43_sys_recv_args *, register_t *);

int	darwin_sys_sigreturn_x2(struct lwp *, const struct darwin_sys_sigreturn_x2_args *, register_t *);

int	darwin_sys_bind(struct lwp *, const struct darwin_sys_bind_args *, register_t *);

int	sys_setsockopt(struct lwp *, const struct sys_setsockopt_args *, register_t *);

int	sys_listen(struct lwp *, const struct sys_listen_args *, register_t *);

int	compat_43_sys_sigvec(struct lwp *, const struct compat_43_sys_sigvec_args *, register_t *);

int	compat_43_sys_sigblock(struct lwp *, const struct compat_43_sys_sigblock_args *, register_t *);

int	compat_43_sys_sigsetmask(struct lwp *, const struct compat_43_sys_sigsetmask_args *, register_t *);

int	compat_13_sys_sigsuspend(struct lwp *, const struct compat_13_sys_sigsuspend_args *, register_t *);

int	compat_43_sys_sigstack(struct lwp *, const struct compat_43_sys_sigstack_args *, register_t *);

int	compat_43_sys_recvmsg(struct lwp *, const struct compat_43_sys_recvmsg_args *, register_t *);

int	compat_43_sys_sendmsg(struct lwp *, const struct compat_43_sys_sendmsg_args *, register_t *);

int	sys_gettimeofday(struct lwp *, const struct sys_gettimeofday_args *, register_t *);

int	sys_getrusage(struct lwp *, const struct sys_getrusage_args *, register_t *);

int	sys_getsockopt(struct lwp *, const struct sys_getsockopt_args *, register_t *);

int	sys_readv(struct lwp *, const struct sys_readv_args *, register_t *);

int	sys_writev(struct lwp *, const struct sys_writev_args *, register_t *);

int	sys_settimeofday(struct lwp *, const struct sys_settimeofday_args *, register_t *);

int	sys_fchown(struct lwp *, const struct sys_fchown_args *, register_t *);

int	sys_fchmod(struct lwp *, const struct sys_fchmod_args *, register_t *);

int	compat_43_sys_recvfrom(struct lwp *, const struct compat_43_sys_recvfrom_args *, register_t *);

int	sys_setreuid(struct lwp *, const struct sys_setreuid_args *, register_t *);

int	sys_setregid(struct lwp *, const struct sys_setregid_args *, register_t *);

int	sys_rename(struct lwp *, const struct sys_rename_args *, register_t *);

int	compat_43_sys_truncate(struct lwp *, const struct compat_43_sys_truncate_args *, register_t *);

int	compat_43_sys_ftruncate(struct lwp *, const struct compat_43_sys_ftruncate_args *, register_t *);

int	sys_flock(struct lwp *, const struct sys_flock_args *, register_t *);

int	sys_mkfifo(struct lwp *, const struct sys_mkfifo_args *, register_t *);

int	darwin_sys_sendto(struct lwp *, const struct darwin_sys_sendto_args *, register_t *);

int	sys_shutdown(struct lwp *, const struct sys_shutdown_args *, register_t *);

int	sys_socketpair(struct lwp *, const struct sys_socketpair_args *, register_t *);

int	sys_mkdir(struct lwp *, const struct sys_mkdir_args *, register_t *);

int	sys_rmdir(struct lwp *, const struct sys_rmdir_args *, register_t *);

int	sys_utimes(struct lwp *, const struct sys_utimes_args *, register_t *);

int	sys_adjtime(struct lwp *, const struct sys_adjtime_args *, register_t *);

int	compat_43_sys_getpeername(struct lwp *, const struct compat_43_sys_getpeername_args *, register_t *);

int	compat_43_sys_gethostid(struct lwp *, const void *, register_t *);

int	compat_43_sys_getrlimit(struct lwp *, const struct compat_43_sys_getrlimit_args *, register_t *);

int	compat_43_sys_setrlimit(struct lwp *, const struct compat_43_sys_setrlimit_args *, register_t *);

int	compat_43_sys_killpg(struct lwp *, const struct compat_43_sys_killpg_args *, register_t *);

int	sys_setsid(struct lwp *, const void *, register_t *);

int	compat_43_sys_getsockname(struct lwp *, const struct compat_43_sys_getsockname_args *, register_t *);

#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
int	sys_nfssvc(struct lwp *, const struct sys_nfssvc_args *, register_t *);

#else
#endif
int	compat_43_sys_getdirentries(struct lwp *, const struct compat_43_sys_getdirentries_args *, register_t *);

int	darwin_sys_statfs(struct lwp *, const struct darwin_sys_statfs_args *, register_t *);

int	darwin_sys_fstatfs(struct lwp *, const struct darwin_sys_fstatfs_args *, register_t *);

#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
int	compat_30_sys_getfh(struct lwp *, const struct compat_30_sys_getfh_args *, register_t *);

#else
#endif
int	compat_09_sys_getdomainname(struct lwp *, const struct compat_09_sys_getdomainname_args *, register_t *);

int	compat_09_sys_setdomainname(struct lwp *, const struct compat_09_sys_setdomainname_args *, register_t *);

int	darwin_sys_kdebug_trace(struct lwp *, const struct darwin_sys_kdebug_trace_args *, register_t *);

int	sys_setgid(struct lwp *, const struct sys_setgid_args *, register_t *);

int	sys_setegid(struct lwp *, const struct sys_setegid_args *, register_t *);

int	sys_seteuid(struct lwp *, const struct sys_seteuid_args *, register_t *);

int	darwin_sys_sigreturn(struct lwp *, const struct darwin_sys_sigreturn_args *, register_t *);

int	darwin_sys_stat(struct lwp *, const struct darwin_sys_stat_args *, register_t *);

int	darwin_sys_fstat(struct lwp *, const struct darwin_sys_fstat_args *, register_t *);

int	darwin_sys_lstat(struct lwp *, const struct darwin_sys_lstat_args *, register_t *);

int	sys_pathconf(struct lwp *, const struct sys_pathconf_args *, register_t *);

int	sys_fpathconf(struct lwp *, const struct sys_fpathconf_args *, register_t *);

int	sys_getrlimit(struct lwp *, const struct sys_getrlimit_args *, register_t *);

int	sys_setrlimit(struct lwp *, const struct sys_setrlimit_args *, register_t *);

int	compat_12_sys_getdirentries(struct lwp *, const struct compat_12_sys_getdirentries_args *, register_t *);

int	darwin_sys_lseek(struct lwp *, const struct darwin_sys_lseek_args *, register_t *);

int	sys_truncate(struct lwp *, const struct sys_truncate_args *, register_t *);

int	sys_ftruncate(struct lwp *, const struct sys_ftruncate_args *, register_t *);

int	darwin_sys___sysctl(struct lwp *, const struct darwin_sys___sysctl_args *, register_t *);

int	sys_mlock(struct lwp *, const struct sys_mlock_args *, register_t *);

int	sys_munlock(struct lwp *, const struct sys_munlock_args *, register_t *);

int	sys_undelete(struct lwp *, const struct sys_undelete_args *, register_t *);

int	darwin_sys_getattrlist(struct lwp *, const struct darwin_sys_getattrlist_args *, register_t *);

int	darwin_sys_load_shared_file(struct lwp *, const struct darwin_sys_load_shared_file_args *, register_t *);

int	darwin_sys_pthread_exit(struct lwp *, const struct darwin_sys_pthread_exit_args *, register_t *);

int	darwin_sys_utrace(struct lwp *, const struct darwin_sys_utrace_args *, register_t *);

int	darwin_sys_audit(struct lwp *, const struct darwin_sys_audit_args *, register_t *);

int	darwin_sys_auditon(struct lwp *, const struct darwin_sys_auditon_args *, register_t *);

int	darwin_sys_getauid(struct lwp *, const struct darwin_sys_getauid_args *, register_t *);

int	darwin_sys_setauid(struct lwp *, const struct darwin_sys_setauid_args *, register_t *);

int	darwin_sys_getauditinfo(struct lwp *, const struct darwin_sys_getauditinfo_args *, register_t *);

int	darwin_sys_setauditinfo(struct lwp *, const struct darwin_sys_setauditinfo_args *, register_t *);

int	darwin_sys_getaudit_addr(struct lwp *, const struct darwin_sys_getaudit_addr_args *, register_t *);

int	darwin_sys_setaudit_addr(struct lwp *, const struct darwin_sys_setaudit_addr_args *, register_t *);

int	darwin_sys_auditctl(struct lwp *, const struct darwin_sys_auditctl_args *, register_t *);

#endif /* _DARWIN_SYS_SYSCALLARGS_H_ */
