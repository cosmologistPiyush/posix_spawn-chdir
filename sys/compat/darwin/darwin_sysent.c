/* $NetBSD: darwin_sysent.c,v 1.52 2007/11/10 13:24:27 dsl Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.34 2007/11/10 13:23:18 dsl Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: darwin_sysent.c,v 1.52 2007/11/10 13:24:27 dsl Exp $");

#include "opt_ktrace.h"
#include "opt_nfsserver.h"
#include "opt_ntp.h"
#include "opt_compat_netbsd.h"
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "fs_lfs.h"
#include "fs_nfs.h"
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/common/compat_file.h>
#include <compat/sys/signal.h>
#include <compat/mach/mach_types.h>
#include <compat/mach/mach_vm.h>
#include <compat/darwin/darwin_signal.h>
#include <compat/darwin/darwin_audit.h>
#include <compat/darwin/darwin_syscallargs.h>

#define	s(type)	sizeof(type)
#define	n(type)	(sizeof(type)/sizeof (register_t))
#define	ns(type)	n(type), s(type)

struct sysent darwin_sysent[] = {
	{ ns(struct sys_syscall_args), SYCALL_INDIRECT | 0,
	    sys_syscall },			/* 0 = syscall */
	{ ns(struct sys_exit_args), 0,
	    sys_exit },				/* 1 = exit */
	{ 0, 0, 0,
	    darwin_sys_fork },			/* 2 = fork */
	{ ns(struct sys_read_args), 0,
	    sys_read },				/* 3 = read */
	{ ns(struct sys_write_args), 0,
	    sys_write },			/* 4 = write */
	{ ns(struct bsd_sys_open_args), 0,
	    bsd_sys_open },			/* 5 = open */
	{ ns(struct sys_close_args), 0,
	    sys_close },			/* 6 = close */
	{ ns(struct sys_wait4_args), 0,
	    sys_wait4 },			/* 7 = wait4 */
	{ ns(struct bsd_compat_43_sys_creat_args), 0,
	    bsd_compat_43_sys_creat },		/* 8 = ocreat */
	{ ns(struct bsd_sys_link_args), 0,
	    bsd_sys_link },			/* 9 = link */
	{ ns(struct bsd_sys_unlink_args), 0,
	    bsd_sys_unlink },			/* 10 = unlink */
	{ 0, 0, 0,
	    sys_nosys },			/* 11 = unimplemented execv */
	{ ns(struct bsd_sys_chdir_args), 0,
	    bsd_sys_chdir },			/* 12 = chdir */
	{ ns(struct sys_fchdir_args), 0,
	    sys_fchdir },			/* 13 = fchdir */
	{ ns(struct darwin_sys_mknod_args), 0,
	    darwin_sys_mknod },			/* 14 = mknod */
	{ ns(struct bsd_sys_chmod_args), 0,
	    bsd_sys_chmod },			/* 15 = chmod */
	{ ns(struct bsd_sys_chown_args), 0,
	    bsd_sys_chown },			/* 16 = chown */
	{ ns(struct sys_obreak_args), 0,
	    sys_obreak },			/* 17 = break */
	{ ns(struct darwin_sys_getfsstat_args), 0,
	    darwin_sys_getfsstat },		/* 18 = getfsstat */
	{ ns(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 19 = olseek */
	{ 0, 0, 0,
	    darwin_sys_getpid },		/* 20 = getpid */
	{ ns(struct bsd_sys_mount_args), 0,
	    bsd_sys_mount },			/* 21 = mount */
	{ ns(struct bsd_sys_unmount_args), 0,
	    bsd_sys_unmount },			/* 22 = unmount */
	{ ns(struct sys_setuid_args), 0,
	    sys_setuid },			/* 23 = setuid */
#ifdef COMPAT_43
	{ 0, 0, 0,
	    sys_getuid_with_euid },		/* 24 = getuid */
#else
	{ 0, 0, 0,
	    sys_getuid },			/* 24 = getuid */
#endif
	{ 0, 0, 0,
	    sys_geteuid },			/* 25 = geteuid */
	{ ns(struct darwin_sys_ptrace_args), 0,
	    darwin_sys_ptrace },		/* 26 = ptrace */
	{ ns(struct sys_recvmsg_args), 0,
	    sys_recvmsg },			/* 27 = recvmsg */
	{ ns(struct sys_sendmsg_args), 0,
	    sys_sendmsg },			/* 28 = sendmsg */
	{ ns(struct darwin_sys_recvfrom_args), 0,
	    darwin_sys_recvfrom },		/* 29 = recvfrom */
	{ ns(struct darwin_sys_accept_args), 0,
	    darwin_sys_accept },		/* 30 = accept */
	{ ns(struct darwin_sys_getpeername_args), 0,
	    darwin_sys_getpeername },		/* 31 = getpeername */
	{ ns(struct darwin_sys_getsockname_args), 0,
	    darwin_sys_getsockname },		/* 32 = getsockname */
	{ ns(struct bsd_sys_access_args), 0,
	    bsd_sys_access },			/* 33 = access */
	{ ns(struct bsd_sys_chflags_args), 0,
	    bsd_sys_chflags },			/* 34 = chflags */
	{ ns(struct sys_fchflags_args), 0,
	    sys_fchflags },			/* 35 = fchflags */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ ns(struct sys_kill_args), 0,
	    sys_kill },				/* 37 = kill */
	{ ns(struct bsd_compat_43_sys_stat_args), 0,
	    bsd_compat_43_sys_stat },		/* 38 = stat43 */
	{ 0, 0, 0,
	    sys_getppid },			/* 39 = getppid */
	{ ns(struct bsd_compat_43_sys_lstat_args), 0,
	    bsd_compat_43_sys_lstat },		/* 40 = lstat43 */
	{ ns(struct sys_dup_args), 0,
	    sys_dup },				/* 41 = dup */
	{ 0, 0, 0,
	    sys_pipe },				/* 42 = pipe */
	{ 0, 0, 0,
	    sys_getegid },			/* 43 = getegid */
	{ ns(struct sys_profil_args), 0,
	    sys_profil },			/* 44 = profil */
#if defined(KTRACE) || !defined(_KERNEL)
	{ ns(struct sys_ktrace_args), 0,
	    sys_ktrace },			/* 45 = ktrace */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 45 = excluded ktrace */
#endif
	{ ns(struct darwin_sys_sigaction_args), 0,
	    darwin_sys_sigaction },		/* 46 = sigaction */
#ifdef COMPAT_43
	{ 0, 0, 0,
	    sys_getgid_with_egid },		/* 47 = getgid */
#else
	{ 0, 0, 0,
	    sys_getgid },			/* 47 = getgid */
#endif
	{ ns(struct darwin_sys_sigprocmask_args), 0,
	    darwin_sys_sigprocmask },		/* 48 = sigprocmask */
	{ ns(struct sys___getlogin_args), 0,
	    sys___getlogin },			/* 49 = __getlogin */
	{ ns(struct sys___setlogin_args), 0,
	    sys___setlogin },			/* 50 = __setlogin */
	{ ns(struct bsd_sys_acct_args), 0,
	    bsd_sys_acct },			/* 51 = acct */
	{ 0, 0, 0,
	    compat_13_sys_sigpending },		/* 52 = sigpending13 */
	{ ns(struct compat_13_sys_sigaltstack_args), 0,
	    compat_13_sys_sigaltstack },	/* 53 = sigaltstack13 */
	{ ns(struct darwin_sys_ioctl_args), 0,
	    darwin_sys_ioctl },			/* 54 = ioctl */
	{ ns(struct sys_reboot_args), 0,
	    sys_reboot },			/* 55 = oreboot */
	{ ns(struct bsd_sys_revoke_args), 0,
	    bsd_sys_revoke },			/* 56 = revoke */
	{ ns(struct bsd_sys_symlink_args), 0,
	    bsd_sys_symlink },			/* 57 = symlink */
	{ ns(struct bsd_sys_readlink_args), 0,
	    bsd_sys_readlink },			/* 58 = readlink */
	{ ns(struct bsd_sys_execve_args), 0,
	    bsd_sys_execve },			/* 59 = execve */
	{ ns(struct sys_umask_args), 0,
	    sys_umask },			/* 60 = umask */
	{ ns(struct bsd_sys_chroot_args), 0,
	    bsd_sys_chroot },			/* 61 = chroot */
	{ ns(struct compat_43_sys_fstat_args), 0,
	    compat_43_sys_fstat },		/* 62 = fstat43 */
	{ 0, 0, 0,
	    sys_nosys },			/* 63 = unimplemented */
	{ 0, 0, 0,
	    compat_43_sys_getpagesize },	/* 64 = ogetpagesize */
	{ ns(struct compat_12_sys_msync_args), 0,
	    compat_12_sys_msync },		/* 65 = msync */
	{ 0, 0, 0,
	    darwin_sys_vfork },			/* 66 = vfork */
	{ 0, 0, 0,
	    sys_nosys },			/* 67 = obsolete vread */
	{ 0, 0, 0,
	    sys_nosys },			/* 68 = obsolete vwrite */
	{ ns(struct sys_sbrk_args), 0,
	    sys_sbrk },				/* 69 = sbrk */
	{ ns(struct sys_sstk_args), 0,
	    sys_sstk },				/* 70 = sstk */
	{ ns(struct sys_mmap_args), 0,
	    sys_mmap },				/* 71 = ommap */
	{ ns(struct sys_ovadvise_args), 0,
	    sys_ovadvise },			/* 72 = vadvise */
	{ ns(struct sys_munmap_args), 0,
	    sys_munmap },			/* 73 = munmap */
	{ ns(struct sys_mprotect_args), 0,
	    sys_mprotect },			/* 74 = mprotect */
	{ ns(struct sys_madvise_args), 0,
	    sys_madvise },			/* 75 = madvise */
	{ 0, 0, 0,
	    sys_nosys },			/* 76 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 77 = unimplemented */
	{ ns(struct sys_mincore_args), 0,
	    sys_mincore },			/* 78 = mincore */
	{ ns(struct sys_getgroups_args), 0,
	    sys_getgroups },			/* 79 = getgroups */
	{ ns(struct sys_setgroups_args), 0,
	    sys_setgroups },			/* 80 = setgroups */
	{ 0, 0, 0,
	    sys_getpgrp },			/* 81 = getpgrp */
	{ ns(struct sys_setpgid_args), 0,
	    sys_setpgid },			/* 82 = setpgid */
	{ ns(struct sys_setitimer_args), 0,
	    sys_setitimer },			/* 83 = setitimer */
	{ 0, 0, 0,
	    compat_43_sys_wait },		/* 84 = owait */
	{ ns(struct bsd_compat_12_sys_swapon_args), 0,
	    bsd_compat_12_sys_swapon },		/* 85 = swapon */
	{ ns(struct sys_getitimer_args), 0,
	    sys_getitimer },			/* 86 = getitimer */
	{ ns(struct compat_43_sys_gethostname_args), 0,
	    compat_43_sys_gethostname },	/* 87 = ogethostname */
	{ ns(struct compat_43_sys_sethostname_args), 0,
	    compat_43_sys_sethostname },	/* 88 = osethostname */
	{ 0, 0, 0,
	    compat_43_sys_getdtablesize },	/* 89 = ogetdtablesize */
	{ ns(struct sys_dup2_args), 0,
	    sys_dup2 },				/* 90 = dup2 */
	{ 0, 0, 0,
	    sys_nosys },			/* 91 = unimplemented */
	{ ns(struct sys_fcntl_args), 0,
	    sys_fcntl },			/* 92 = fcntl */
	{ ns(struct sys_select_args), 0,
	    sys_select },			/* 93 = select */
	{ 0, 0, 0,
	    sys_nosys },			/* 94 = unimplemented */
	{ ns(struct sys_fsync_args), 0,
	    sys_fsync },			/* 95 = fsync */
	{ ns(struct sys_setpriority_args), 0,
	    sys_setpriority },			/* 96 = setpriority */
	{ ns(struct darwin_sys_socket_args), 0,
	    darwin_sys_socket },		/* 97 = socket */
	{ ns(struct darwin_sys_connect_args), 0,
	    darwin_sys_connect },		/* 98 = connect */
	{ ns(struct compat_43_sys_accept_args), 0,
	    compat_43_sys_accept },		/* 99 = oaccept */
	{ ns(struct sys_getpriority_args), 0,
	    sys_getpriority },			/* 100 = getpriority */
	{ ns(struct compat_43_sys_send_args), 0,
	    compat_43_sys_send },		/* 101 = osend */
	{ ns(struct compat_43_sys_recv_args), 0,
	    compat_43_sys_recv },		/* 102 = orecv */
	{ ns(struct darwin_sys_sigreturn_x2_args), 0,
	    darwin_sys_sigreturn_x2 },		/* 103 = sigreturn_x2 */
	{ ns(struct darwin_sys_bind_args), 0,
	    darwin_sys_bind },			/* 104 = bind */
	{ ns(struct sys_setsockopt_args), 0,
	    sys_setsockopt },			/* 105 = setsockopt */
	{ ns(struct sys_listen_args), 0,
	    sys_listen },			/* 106 = listen */
	{ 0, 0, 0,
	    sys_nosys },			/* 107 = unimplemented */
	{ ns(struct compat_43_sys_sigvec_args), 0,
	    compat_43_sys_sigvec },		/* 108 = osigvec */
	{ ns(struct compat_43_sys_sigblock_args), 0,
	    compat_43_sys_sigblock },		/* 109 = osigblock */
	{ ns(struct compat_43_sys_sigsetmask_args), 0,
	    compat_43_sys_sigsetmask },		/* 110 = osigsetmask */
	{ ns(struct compat_13_sys_sigsuspend_args), 0,
	    compat_13_sys_sigsuspend },		/* 111 = sigsuspend13 */
	{ ns(struct compat_43_sys_sigstack_args), 0,
	    compat_43_sys_sigstack },		/* 112 = osigstack */
	{ ns(struct compat_43_sys_recvmsg_args), 0,
	    compat_43_sys_recvmsg },		/* 113 = orecvmsg */
	{ ns(struct compat_43_sys_sendmsg_args), 0,
	    compat_43_sys_sendmsg },		/* 114 = osendmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 115 = unimplemented */
	{ ns(struct sys_gettimeofday_args), 0,
	    sys_gettimeofday },			/* 116 = gettimeofday */
	{ ns(struct sys_getrusage_args), 0,
	    sys_getrusage },			/* 117 = getrusage */
	{ ns(struct sys_getsockopt_args), 0,
	    sys_getsockopt },			/* 118 = getsockopt */
	{ 0, 0, 0,
	    sys_nosys },			/* 119 = unimplemented */
	{ ns(struct sys_readv_args), 0,
	    sys_readv },			/* 120 = readv */
	{ ns(struct sys_writev_args), 0,
	    sys_writev },			/* 121 = writev */
	{ ns(struct sys_settimeofday_args), 0,
	    sys_settimeofday },			/* 122 = settimeofday */
	{ ns(struct sys_fchown_args), 0,
	    sys_fchown },			/* 123 = fchown */
	{ ns(struct sys_fchmod_args), 0,
	    sys_fchmod },			/* 124 = fchmod */
	{ ns(struct compat_43_sys_recvfrom_args), 0,
	    compat_43_sys_recvfrom },		/* 125 = orecvfrom */
	{ ns(struct sys_setreuid_args), 0,
	    sys_setreuid },			/* 126 = setreuid */
	{ ns(struct sys_setregid_args), 0,
	    sys_setregid },			/* 127 = setregid */
	{ ns(struct bsd_sys_rename_args), 0,
	    bsd_sys_rename },			/* 128 = rename */
	{ ns(struct bsd_compat_43_sys_truncate_args), 0,
	    bsd_compat_43_sys_truncate },	/* 129 = otruncate */
	{ ns(struct compat_43_sys_ftruncate_args), 0,
	    compat_43_sys_ftruncate },		/* 130 = oftruncate */
	{ ns(struct sys_flock_args), 0,
	    sys_flock },			/* 131 = flock */
	{ ns(struct bsd_sys_mkfifo_args), 0,
	    bsd_sys_mkfifo },			/* 132 = mkfifo */
	{ ns(struct darwin_sys_sendto_args), 0,
	    darwin_sys_sendto },		/* 133 = sendto */
	{ ns(struct sys_shutdown_args), 0,
	    sys_shutdown },			/* 134 = shutdown */
	{ ns(struct sys_socketpair_args), 0,
	    sys_socketpair },			/* 135 = socketpair */
	{ ns(struct bsd_sys_mkdir_args), 0,
	    bsd_sys_mkdir },			/* 136 = mkdir */
	{ ns(struct bsd_sys_rmdir_args), 0,
	    bsd_sys_rmdir },			/* 137 = rmdir */
	{ ns(struct bsd_sys_utimes_args), 0,
	    bsd_sys_utimes },			/* 138 = utimes */
	{ 0, 0, 0,
	    sys_nosys },			/* 139 = unimplemented futimes */
	{ ns(struct sys_adjtime_args), 0,
	    sys_adjtime },			/* 140 = adjtime */
	{ ns(struct compat_43_sys_getpeername_args), 0,
	    compat_43_sys_getpeername },	/* 141 = ogetpeername */
	{ 0, 0, 0,
	    compat_43_sys_gethostid },		/* 142 = ogethostid */
	{ 0, 0, 0,
	    sys_nosys },			/* 143 = unimplemented */
	{ ns(struct compat_43_sys_getrlimit_args), 0,
	    compat_43_sys_getrlimit },		/* 144 = ogetrlimit */
	{ ns(struct compat_43_sys_setrlimit_args), 0,
	    compat_43_sys_setrlimit },		/* 145 = osetrlimit */
	{ ns(struct compat_43_sys_killpg_args), 0,
	    compat_43_sys_killpg },		/* 146 = okillpg */
	{ 0, 0, 0,
	    sys_setsid },			/* 147 = setsid */
	{ 0, 0, 0,
	    sys_nosys },			/* 148 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 149 = unimplemented */
	{ ns(struct compat_43_sys_getsockname_args), 0,
	    compat_43_sys_getsockname },	/* 150 = ogetsockname */
	{ 0, 0, 0,
	    sys_nosys },			/* 151 = unimplemented getpgid */
	{ 0, 0, 0,
	    sys_nosys },			/* 152 = unimplemented setprivexec */
	{ 0, 0, 0,
	    sys_nosys },			/* 153 = unimplemented pread */
	{ 0, 0, 0,
	    sys_nosys },			/* 154 = unimplemented pwrite */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	{ ns(struct sys_nfssvc_args), 0,
	    sys_nfssvc },			/* 155 = nfssvc */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 155 = excluded nfssvc */
#endif
	{ ns(struct compat_43_sys_getdirentries_args), 0,
	    compat_43_sys_getdirentries },	/* 156 = ogetdirentries */
	{ ns(struct darwin_sys_statfs_args), 0,
	    darwin_sys_statfs },		/* 157 = statfs */
	{ ns(struct darwin_sys_fstatfs_args), 0,
	    darwin_sys_fstatfs },		/* 158 = fstatfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 159 = unimplemented unmount */
	{ 0, 0, 0,
	    sys_nosys },			/* 160 = unimplemented */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	{ ns(struct bsd_sys_getfh_args), 0,
	    bsd_sys_getfh },			/* 161 = getfh */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 161 = excluded getfh */
#endif
	{ ns(struct compat_09_sys_getdomainname_args), 0,
	    compat_09_sys_getdomainname },	/* 162 = ogetdomainname */
	{ ns(struct compat_09_sys_setdomainname_args), 0,
	    compat_09_sys_setdomainname },	/* 163 = osetdomainname */
	{ 0, 0, 0,
	    sys_nosys },			/* 164 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 165 = unimplemented quotactl */
	{ 0, 0, 0,
	    sys_nosys },			/* 166 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 167 = unimplemented mount */
	{ 0, 0, 0,
	    sys_nosys },			/* 168 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 169 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 170 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 171 = unimplemented wait3 */
	{ 0, 0, 0,
	    sys_nosys },			/* 172 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 173 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 174 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 175 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 176 = unimplemented add_profil */
	{ 0, 0, 0,
	    sys_nosys },			/* 177 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 178 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 179 = unimplemented */
	{ ns(struct darwin_sys_kdebug_trace_args), 0,
	    darwin_sys_kdebug_trace },		/* 180 = kdebug_trace */
	{ ns(struct sys_setgid_args), 0,
	    sys_setgid },			/* 181 = setgid */
	{ ns(struct sys_setegid_args), 0,
	    sys_setegid },			/* 182 = setegid */
	{ ns(struct sys_seteuid_args), 0,
	    sys_seteuid },			/* 183 = seteuid */
	{ ns(struct darwin_sys_sigreturn_args), 0,
	    darwin_sys_sigreturn },		/* 184 = sigreturn */
	{ 0, 0, 0,
	    sys_nosys },			/* 185 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 186 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 187 = unimplemented */
	{ ns(struct darwin_sys_stat_args), 0,
	    darwin_sys_stat },			/* 188 = stat12 */
	{ ns(struct darwin_sys_fstat_args), 0,
	    darwin_sys_fstat },			/* 189 = fstat */
	{ ns(struct darwin_sys_lstat_args), 0,
	    darwin_sys_lstat },			/* 190 = lstat12 */
	{ ns(struct bsd_sys_pathconf_args), 0,
	    bsd_sys_pathconf },			/* 191 = pathconf */
	{ ns(struct sys_fpathconf_args), 0,
	    sys_fpathconf },			/* 192 = fpathconf */
	{ 0, 0, 0,
	    sys_nosys },			/* 193 = unimplemented getfsstat */
	{ ns(struct sys_getrlimit_args), 0,
	    sys_getrlimit },			/* 194 = getrlimit */
	{ ns(struct sys_setrlimit_args), 0,
	    sys_setrlimit },			/* 195 = setrlimit */
	{ ns(struct compat_12_sys_getdirentries_args), 0,
	    compat_12_sys_getdirentries },	/* 196 = getdirentries */
	{ ns(struct sys_mmap_args), 0,
	    sys_mmap },				/* 197 = mmap */
	{ 0, 0, 0,
	    sys_nosys },			/* 198 = unimplemented */
	{ ns(struct darwin_sys_lseek_args), 0,
	    darwin_sys_lseek },			/* 199 = lseek */
	{ ns(struct bsd_sys_truncate_args), 0,
	    bsd_sys_truncate },			/* 200 = truncate */
	{ ns(struct sys_ftruncate_args), 0,
	    sys_ftruncate },			/* 201 = ftruncate */
	{ ns(struct darwin_sys___sysctl_args), 0,
	    darwin_sys___sysctl },		/* 202 = __sysctl */
	{ ns(struct sys_mlock_args), 0,
	    sys_mlock },			/* 203 = mlock */
	{ ns(struct sys_munlock_args), 0,
	    sys_munlock },			/* 204 = munlock */
	{ ns(struct bsd_sys_undelete_args), 0,
	    bsd_sys_undelete },			/* 205 = undelete */
	{ 0, 0, 0,
	    sys_nosys },			/* 206 = unimplemented atsocket */
	{ 0, 0, 0,
	    sys_nosys },			/* 207 = unimplemented atgetmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 208 = unimplemented atputmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 209 = unimplemented atpsndreq */
	{ 0, 0, 0,
	    sys_nosys },			/* 210 = unimplemented atpsndrsp */
	{ 0, 0, 0,
	    sys_nosys },			/* 211 = unimplemented atpgetreq */
	{ 0, 0, 0,
	    sys_nosys },			/* 212 = unimplemented atpgetrsp */
	{ 0, 0, 0,
	    sys_nosys },			/* 213 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 214 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 215 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 216 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 217 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 218 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 219 = unimplemented */
	{ ns(struct darwin_sys_getattrlist_args), 0,
	    darwin_sys_getattrlist },		/* 220 = getattrlist */
	{ 0, 0, 0,
	    sys_nosys },			/* 221 = unimplemented setattrlist */
	{ 0, 0, 0,
	    sys_nosys },			/* 222 = unimplemented getdirentriesattr */
	{ 0, 0, 0,
	    sys_nosys },			/* 223 = unimplemented exchangedata */
	{ 0, 0, 0,
	    sys_nosys },			/* 224 = unimplemented checkuseraccess */
	{ 0, 0, 0,
	    sys_nosys },			/* 225 = unimplemented searchfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 226 = unimplemented delete */
	{ 0, 0, 0,
	    sys_nosys },			/* 227 = unimplemented copyfile */
	{ 0, 0, 0,
	    sys_nosys },			/* 228 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 229 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 230 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 231 = unimplemented watchevent */
	{ 0, 0, 0,
	    sys_nosys },			/* 232 = unimplemented waitevent */
	{ 0, 0, 0,
	    sys_nosys },			/* 233 = unimplemented modwatch */
	{ 0, 0, 0,
	    sys_nosys },			/* 234 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 235 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 236 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 237 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 238 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 239 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 240 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 241 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 242 = unimplemented fsctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 243 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 244 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 245 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 246 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 247 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 248 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 249 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 250 = unimplemented minherit */
	{ 0, 0, 0,
	    sys_nosys },			/* 251 = unimplemented semsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 252 = unimplemented msgsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 253 = unimplemented semop */
	{ 0, 0, 0,
	    sys_nosys },			/* 254 = unimplemented semctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 255 = unimplemented semget */
	{ 0, 0, 0,
	    sys_nosys },			/* 256 = unimplemented semop */
	{ 0, 0, 0,
	    sys_nosys },			/* 257 = unimplemented semconfig */
	{ 0, 0, 0,
	    sys_nosys },			/* 258 = unimplemented msgctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 259 = unimplemented msgget */
	{ 0, 0, 0,
	    sys_nosys },			/* 260 = unimplemented msgsnd */
	{ 0, 0, 0,
	    sys_nosys },			/* 261 = unimplemented msgrcv */
	{ 0, 0, 0,
	    sys_nosys },			/* 262 = unimplemented shmat */
	{ 0, 0, 0,
	    sys_nosys },			/* 263 = unimplemented shmctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 264 = unimplemented shmdt */
	{ 0, 0, 0,
	    sys_nosys },			/* 265 = unimplemented shmget */
	{ 0, 0, 0,
	    sys_nosys },			/* 266 = unimplemented shm_open */
	{ 0, 0, 0,
	    sys_nosys },			/* 267 = unimplemented shm_unlink */
	{ 0, 0, 0,
	    sys_nosys },			/* 268 = unimplemented sem_open */
	{ 0, 0, 0,
	    sys_nosys },			/* 269 = unimplemented sem_close */
	{ 0, 0, 0,
	    sys_nosys },			/* 270 = unimplemented sem_unlink */
	{ 0, 0, 0,
	    sys_nosys },			/* 271 = unimplemented sem_wait */
	{ 0, 0, 0,
	    sys_nosys },			/* 272 = unimplemented sem_trywait */
	{ 0, 0, 0,
	    sys_nosys },			/* 273 = unimplemented sem_post */
	{ 0, 0, 0,
	    sys_nosys },			/* 274 = unimplemented sem_getvalue */
	{ 0, 0, 0,
	    sys_nosys },			/* 275 = unimplemented sem_init */
	{ 0, 0, 0,
	    sys_nosys },			/* 276 = unimplemented sem_destroy */
	{ 0, 0, 0,
	    sys_nosys },			/* 277 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 278 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 279 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 280 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 281 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 282 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 283 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 284 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 285 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 286 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 287 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 288 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 289 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 290 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 291 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 292 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 293 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 294 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 295 = unimplemented */
	{ ns(struct darwin_sys_load_shared_file_args), 0,
	    darwin_sys_load_shared_file },	/* 296 = load_shared_file */
	{ 0, 0, 0,
	    sys_nosys },			/* 297 = unimplemented reset_shared_file */
	{ 0, 0, 0,
	    sys_nosys },			/* 298 = unimplemented new_system_shared_regions */
	{ 0, 0, 0,
	    sys_nosys },			/* 299 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 300 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 301 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 302 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 303 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 304 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 305 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 306 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 307 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 308 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 309 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 310 = unimplemented getsid */
	{ 0, 0, 0,
	    sys_nosys },			/* 311 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 312 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 313 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 314 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 315 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 316 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 317 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 318 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 319 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 320 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 321 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 322 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 323 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 324 = unimplemented mlockall */
	{ 0, 0, 0,
	    sys_nosys },			/* 325 = unimplemented munlockall */
	{ 0, 0, 0,
	    sys_nosys },			/* 326 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 327 = unimplemented issetugid */
	{ 0, 0, 0,
	    sys_nosys },			/* 328 = unimplemented __pthread_kill */
	{ 0, 0, 0,
	    sys_nosys },			/* 329 = unimplemented pthread_sigmask */
	{ 0, 0, 0,
	    sys_nosys },			/* 330 = unimplemented sigwait */
	{ ns(struct darwin_sys_pthread_exit_args), 0,
	    darwin_sys_pthread_exit },		/* 331 = pthread_exit */
	{ 0, 0, 0,
	    sys_nosys },			/* 332 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 333 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 334 = unimplemented */
	{ ns(struct darwin_sys_utrace_args), 0,
	    darwin_sys_utrace },		/* 335 = utrace */
	{ 0, 0, 0,
	    sys_nosys },			/* 336 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 337 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 338 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 339 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 340 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 341 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 342 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 343 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 344 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 345 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 346 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 347 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 348 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 349 = unimplemented */
	{ ns(struct darwin_sys_audit_args), 0,
	    darwin_sys_audit },			/* 350 = audit */
	{ ns(struct darwin_sys_auditon_args), 0,
	    darwin_sys_auditon },		/* 351 = auditon */
	{ 0, 0, 0,
	    sys_nosys },			/* 352 = unimplemented */
	{ ns(struct darwin_sys_getauid_args), 0,
	    darwin_sys_getauid },		/* 353 = getauid */
	{ ns(struct darwin_sys_setauid_args), 0,
	    darwin_sys_setauid },		/* 354 = setauid */
	{ ns(struct darwin_sys_getauditinfo_args), 0,
	    darwin_sys_getauditinfo },		/* 355 = getauditinfo */
	{ ns(struct darwin_sys_setauditinfo_args), 0,
	    darwin_sys_setauditinfo },		/* 356 = setauditinfo */
	{ ns(struct darwin_sys_getaudit_addr_args), 0,
	    darwin_sys_getaudit_addr },		/* 357 = getaudit_addr */
	{ ns(struct darwin_sys_setaudit_addr_args), 0,
	    darwin_sys_setaudit_addr },		/* 358 = setaudit_addr */
	{ ns(struct darwin_sys_auditctl_args), 0,
	    darwin_sys_auditctl },		/* 359 = auditctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 360 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 361 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 362 = unimplemented kqueue */
	{ 0, 0, 0,
	    sys_nosys },			/* 363 = unimplemented kevent */
	{ 0, 0, 0,
	    sys_nosys },			/* 364 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 365 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 366 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 367 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 368 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 369 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 370 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 371 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 372 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 373 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 374 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 375 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 376 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 377 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 378 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 379 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 380 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 381 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 382 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 383 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 384 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 385 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 386 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 387 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 388 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 389 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 390 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 391 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 392 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 393 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 394 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 395 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 396 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 397 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 398 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 399 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 400 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 401 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 402 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 403 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 404 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 405 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 406 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 407 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 408 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 409 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 410 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 411 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 412 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 413 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 414 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 415 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 416 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 417 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 418 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 419 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 420 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 421 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 422 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 423 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 424 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 425 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 426 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 427 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 428 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 429 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 430 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 431 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 432 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 433 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 434 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 435 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 436 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 437 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 438 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 439 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 440 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 441 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 442 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 443 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 444 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 445 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 446 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 447 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 448 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 449 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 450 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 451 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 452 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 453 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 454 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 455 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 456 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 457 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 458 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 459 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 460 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 461 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 462 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 463 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 464 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 465 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 466 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 467 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 468 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 469 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 470 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 471 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 472 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 473 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 474 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 475 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 476 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 477 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 478 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 479 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 480 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 481 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 482 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 483 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 484 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 485 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 486 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 487 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 488 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 489 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 490 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 491 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 492 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 493 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 494 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 495 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 496 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 497 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 498 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 499 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 500 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 501 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 502 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 503 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 504 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 505 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 506 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 507 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 508 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 509 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 510 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 511 = filler */
};

