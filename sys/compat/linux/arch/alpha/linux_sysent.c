/* $NetBSD: linux_sysent.c,v 1.43 2002/02/18 16:36:21 christos Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.38 2002/02/18 16:35:57 christos Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_sysent.c,v 1.43 2002/02/18 16:36:21 christos Exp $");

#if defined(_KERNEL_OPT)
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "opt_compat_osf1.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_ipc.h>
#include <compat/linux/common/linux_msg.h>
#include <compat/linux/common/linux_sem.h>
#include <compat/linux/common/linux_shm.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/linux_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent linux_sysent[] = {
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args), 0,
	    sys_exit },				/* 1 = exit */
	{ 0, 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct sys_read_args), 0,
	    sys_read },				/* 3 = read */
	{ 3, s(struct sys_write_args), 0,
	    sys_write },			/* 4 = write */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 5 = unimplemented */
	{ 1, s(struct sys_close_args), 0,
	    sys_close },			/* 6 = close */
	{ 4, s(struct osf1_sys_wait4_args), 0,
	    osf1_sys_wait4 },			/* 7 = wait4 */
	{ 2, s(struct linux_sys_creat_args), 0,
	    linux_sys_creat },			/* 8 = creat */
	{ 2, s(struct linux_sys_link_args), 0,
	    linux_sys_link },			/* 9 = link */
	{ 1, s(struct linux_sys_unlink_args), 0,
	    linux_sys_unlink },			/* 10 = unlink */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 11 = unimplemented */
	{ 1, s(struct linux_sys_chdir_args), 0,
	    linux_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct sys_fchdir_args), 0,
	    sys_fchdir },			/* 13 = fchdir */
	{ 3, s(struct linux_sys_mknod_args), 0,
	    linux_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct linux_sys_chmod_args), 0,
	    linux_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct sys___posix_chown_args), 0,
	    sys___posix_chown },		/* 16 = __posix_chown */
	{ 1, s(struct linux_sys_brk_args), 0,
	    linux_sys_brk },			/* 17 = brk */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 18 = unimplemented */
	{ 3, s(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0, 0,
	    sys_getpid_with_ppid },		/* 20 = getpid_with_ppid */
	{ 4, s(struct osf1_sys_mount_args), 0,
	    osf1_sys_mount },			/* 21 = mount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 22 = unimplemented umount */
	{ 1, s(struct sys_setuid_args), 0,
	    sys_setuid },			/* 23 = setuid */
	{ 0, 0, 0,
	    sys_getuid_with_euid },		/* 24 = getuid_with_euid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 25 = unimplemented */
	{ 4, s(struct linux_sys_ptrace_args), 0,
	    linux_sys_ptrace },			/* 26 = ptrace */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 27 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 28 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 29 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 30 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 31 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 32 = unimplemented */
	{ 2, s(struct linux_sys_access_args), 0,
	    linux_sys_access },			/* 33 = access */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 34 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 35 = unimplemented */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct linux_sys_kill_args), 0,
	    linux_sys_kill },			/* 37 = kill */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 38 = unimplemented */
	{ 2, s(struct sys_setpgid_args), 0,
	    sys_setpgid },			/* 39 = setpgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 40 = unimplemented */
	{ 1, s(struct sys_dup_args), 0,
	    sys_dup },				/* 41 = dup */
	{ 0, 0, 0,
	    linux_sys_pipe },			/* 42 = pipe */
	{ 4, s(struct osf1_sys_set_program_attributes_args), 0,
	    osf1_sys_set_program_attributes },	/* 43 = set_program_attributes */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 44 = unimplemented */
	{ 3, s(struct linux_sys_open_args), 0,
	    linux_sys_open },			/* 45 = open */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 46 = unimplemented */
	{ 0, 0, 0,
	    sys_getgid_with_egid },		/* 47 = getgid_with_egid */
	{ 2, s(struct compat_13_sys_sigprocmask_args), 0,
	    compat_13_sys_sigprocmask },	/* 48 = sigprocmask */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 49 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 50 = unimplemented */
	{ 1, s(struct sys_acct_args), 0,
	    sys_acct },				/* 51 = acct */
	{ 1, s(struct linux_sys_sigpending_args), 0,
	    linux_sys_sigpending },		/* 52 = sigpending */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 53 = unimplemented */
	{ 3, s(struct linux_sys_ioctl_args), 0,
	    linux_sys_ioctl },			/* 54 = ioctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 55 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 56 = unimplemented */
	{ 2, s(struct linux_sys_symlink_args), 0,
	    linux_sys_symlink },		/* 57 = symlink */
	{ 3, s(struct linux_sys_readlink_args), 0,
	    linux_sys_readlink },		/* 58 = readlink */
	{ 3, s(struct linux_sys_execve_args), 0,
	    linux_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args), 0,
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args), 0,
	    sys_chroot },			/* 61 = chroot */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 62 = unimplemented */
	{ 0, 0, 0,
	    sys_getpgrp },			/* 63 = getpgrp */
	{ 0, 0, 0,
	    compat_43_sys_getpagesize },	/* 64 = getpagesize */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 65 = unimplemented */
	{ 0, 0, 0,
	    sys___vfork14 },			/* 66 = __vfork14 */
	{ 2, s(struct linux_sys_stat_args), 0,
	    linux_sys_stat },			/* 67 = stat */
	{ 2, s(struct linux_sys_lstat_args), 0,
	    linux_sys_lstat },			/* 68 = lstat */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 69 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 70 = unimplemented */
	{ 6, s(struct linux_sys_mmap_args), 0,
	    linux_sys_mmap },			/* 71 = mmap */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 72 = unimplemented */
	{ 2, s(struct sys_munmap_args), 0,
	    sys_munmap },			/* 73 = munmap */
	{ 3, s(struct sys_mprotect_args), 0,
	    sys_mprotect },			/* 74 = mprotect */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 75 = unimplemented madvise */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 76 = unimplemented vhangup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 77 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 78 = unimplemented */
	{ 2, s(struct sys_getgroups_args), 0,
	    sys_getgroups },			/* 79 = getgroups */
	{ 2, s(struct sys_setgroups_args), 0,
	    sys_setgroups },			/* 80 = setgroups */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 81 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 82 = unimplemented setpgrp */
	{ 3, s(struct osf1_sys_setitimer_args), 0,
	    osf1_sys_setitimer },		/* 83 = setitimer */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 84 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 85 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 86 = unimplemented osf1_sys_getitimer */
	{ 2, s(struct compat_43_sys_gethostname_args), 0,
	    compat_43_sys_gethostname },	/* 87 = gethostname */
	{ 2, s(struct compat_43_sys_sethostname_args), 0,
	    compat_43_sys_sethostname },	/* 88 = sethostname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 89 = unimplemented getdtablesize */
	{ 2, s(struct sys_dup2_args), 0,
	    sys_dup2 },				/* 90 = dup2 */
	{ 2, s(struct linux_sys_fstat_args), 0,
	    linux_sys_fstat },			/* 91 = fstat */
	{ 3, s(struct linux_sys_fcntl_args), 0,
	    linux_sys_fcntl },			/* 92 = fcntl */
	{ 5, s(struct osf1_sys_select_args), 0,
	    osf1_sys_select },			/* 93 = select */
	{ 3, s(struct sys_poll_args), 0,
	    sys_poll },				/* 94 = poll */
	{ 1, s(struct sys_fsync_args), 0,
	    sys_fsync },			/* 95 = fsync */
	{ 3, s(struct sys_setpriority_args), 0,
	    sys_setpriority },			/* 96 = setpriority */
	{ 3, s(struct linux_sys_socket_args), 0,
	    linux_sys_socket },			/* 97 = socket */
	{ 3, s(struct linux_sys_connect_args), 0,
	    linux_sys_connect },		/* 98 = connect */
	{ 3, s(struct linux_sys_accept_args), 0,
	    linux_sys_accept },			/* 99 = accept */
	{ 2, s(struct sys_getpriority_args), 0,
	    sys_getpriority },			/* 100 = getpriority */
	{ 4, s(struct linux_sys_send_args), 0,
	    linux_sys_send },			/* 101 = send */
	{ 4, s(struct linux_sys_recv_args), 0,
	    linux_sys_recv },			/* 102 = recv */
	{ 1, s(struct linux_sys_sigreturn_args), 0,
	    linux_sys_sigreturn },		/* 103 = sigreturn */
	{ 3, s(struct linux_sys_bind_args), 0,
	    linux_sys_bind },			/* 104 = bind */
	{ 5, s(struct linux_sys_setsockopt_args), 0,
	    linux_sys_setsockopt },		/* 105 = setsockopt */
	{ 2, s(struct sys_listen_args), 0,
	    sys_listen },			/* 106 = listen */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 107 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 108 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 109 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 110 = unimplemented */
	{ 3, s(struct linux_sys_sigsuspend_args), 0,
	    linux_sys_sigsuspend },		/* 111 = sigsuspend */
	{ 2, s(struct compat_43_sys_sigstack_args), 0,
	    compat_43_sys_sigstack },		/* 112 = sigstack */
	{ 3, s(struct linux_sys_recvmsg_args), 0,
	    linux_sys_recvmsg },		/* 113 = recvmsg */
	{ 3, s(struct linux_sys_sendmsg_args), 0,
	    linux_sys_sendmsg },		/* 114 = sendmsg */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 115 = unimplemented */
	{ 2, s(struct osf1_sys_gettimeofday_args), 0,
	    osf1_sys_gettimeofday },		/* 116 = gettimeofday */
	{ 2, s(struct osf1_sys_getrusage_args), 0,
	    osf1_sys_getrusage },		/* 117 = getrusage */
	{ 5, s(struct linux_sys_getsockopt_args), 0,
	    linux_sys_getsockopt },		/* 118 = getsockopt */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 119 = unimplemented */
	{ 3, s(struct sys_readv_args), 0,
	    sys_readv },			/* 120 = readv */
	{ 3, s(struct sys_writev_args), 0,
	    sys_writev },			/* 121 = writev */
	{ 2, s(struct osf1_sys_settimeofday_args), 0,
	    osf1_sys_settimeofday },		/* 122 = settimeofday */
	{ 3, s(struct sys___posix_fchown_args), 0,
	    sys___posix_fchown },		/* 123 = __posix_fchown */
	{ 2, s(struct sys_fchmod_args), 0,
	    sys_fchmod },			/* 124 = fchmod */
	{ 6, s(struct linux_sys_recvfrom_args), 0,
	    linux_sys_recvfrom },		/* 125 = recvfrom */
	{ 2, s(struct sys_setreuid_args), 0,
	    sys_setreuid },			/* 126 = setreuid */
	{ 2, s(struct sys_setregid_args), 0,
	    sys_setregid },			/* 127 = setregid */
	{ 2, s(struct linux_sys_rename_args), 0,
	    linux_sys_rename },			/* 128 = rename */
	{ 2, s(struct linux_sys_truncate_args), 0,
	    linux_sys_truncate },		/* 129 = truncate */
	{ 2, s(struct compat_43_sys_ftruncate_args), 0,
	    compat_43_sys_ftruncate },		/* 130 = ftruncate */
	{ 2, s(struct sys_flock_args), 0,
	    sys_flock },			/* 131 = flock */
	{ 1, s(struct sys_setgid_args), 0,
	    sys_setgid },			/* 132 = setgid */
	{ 6, s(struct linux_sys_sendto_args), 0,
	    linux_sys_sendto },			/* 133 = sendto */
	{ 2, s(struct sys_shutdown_args), 0,
	    sys_shutdown },			/* 134 = shutdown */
	{ 4, s(struct linux_sys_socketpair_args), 0,
	    linux_sys_socketpair },		/* 135 = socketpair */
	{ 2, s(struct linux_sys_mkdir_args), 0,
	    linux_sys_mkdir },			/* 136 = mkdir */
	{ 1, s(struct linux_sys_rmdir_args), 0,
	    linux_sys_rmdir },			/* 137 = rmdir */
	{ 2, s(struct osf1_sys_utimes_args), 0,
	    osf1_sys_utimes },			/* 138 = utimes */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 139 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 140 = unimplemented */
	{ 3, s(struct linux_sys_getpeername_args), 0,
	    linux_sys_getpeername },		/* 141 = getpeername */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 142 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 143 = unimplemented */
	{ 2, s(struct compat_43_sys_getrlimit_args), 0,
	    compat_43_sys_getrlimit },		/* 144 = getrlimit */
	{ 2, s(struct compat_43_sys_setrlimit_args), 0,
	    compat_43_sys_setrlimit },		/* 145 = setrlimit */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 146 = unimplemented */
	{ 0, 0, 0,
	    sys_setsid },			/* 147 = setsid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 148 = unimplemented quotactl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 149 = unimplemented */
	{ 3, s(struct linux_sys_getsockname_args), 0,
	    linux_sys_getsockname },		/* 150 = getsockname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 151 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 152 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 153 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 154 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 155 = unimplemented */
	{ 3, s(struct linux_sys_sigaction_args), 0,
	    linux_sys_sigaction },		/* 156 = sigaction */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 157 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 158 = unimplemented */
	{ 4, s(struct compat_43_sys_getdirentries_args), 0,
	    compat_43_sys_getdirentries },	/* 159 = getdirentries */
	{ 3, s(struct osf1_sys_statfs_args), 0,
	    osf1_sys_statfs },			/* 160 = statfs */
	{ 3, s(struct osf1_sys_fstatfs_args), 0,
	    osf1_sys_fstatfs },			/* 161 = fstatfs */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 162 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 163 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 164 = unimplemented */
	{ 2, s(struct compat_09_sys_getdomainname_args), 0,
	    compat_09_sys_getdomainname },	/* 165 = getdomainname */
	{ 2, s(struct linux_sys_setdomainname_args), 0,
	    linux_sys_setdomainname },		/* 166 = setdomainname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 167 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 168 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 169 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 170 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 171 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 172 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 173 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 174 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 175 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 176 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 177 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 178 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 179 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 180 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 181 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 182 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 183 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 184 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 185 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 186 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 187 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 188 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 189 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 190 = unimplemented */
	{ 2, s(struct linux_sys_ugetrlimit_args), 0,
	    linux_sys_ugetrlimit },		/* 191 = ugetrlimit */
	{ 6, s(struct linux_sys_mmap2_args), 0,
	    linux_sys_mmap2 },			/* 192 = mmap2 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 193 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 194 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 195 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 196 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 197 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 198 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 199 = unimplemented osf1_sys_swapon */
#ifdef SYSVMSG
	{ 3, s(struct linux_sys_msgctl_args), 0,
	    linux_sys_msgctl },			/* 200 = msgctl */
	{ 2, s(struct sys_msgget_args), 0,
	    sys_msgget },			/* 201 = msgget */
	{ 5, s(struct sys_msgrcv_args), 0,
	    sys_msgrcv },			/* 202 = msgrcv */
	{ 4, s(struct sys_msgsnd_args), 0,
	    sys_msgsnd },			/* 203 = msgsnd */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 200 = unimplemented msgctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 201 = unimplemented msgget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 202 = unimplemented msgrcv */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 203 = unimplemented msgsnd */
#endif
#ifdef SYSVSEM
	{ 4, s(struct linux_sys_semctl_args), 0,
	    linux_sys_semctl },			/* 204 = semctl */
	{ 3, s(struct sys_semget_args), 0,
	    sys_semget },			/* 205 = semget */
	{ 3, s(struct sys_semop_args), 0,
	    sys_semop },			/* 206 = semop */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 204 = unimplemented semctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 205 = unimplemented semget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 206 = unimplemented semop */
#endif
	{ 1, s(struct linux_sys_olduname_args), 0,
	    linux_sys_olduname },		/* 207 = olduname */
	{ 3, s(struct sys___posix_lchown_args), 0,
	    sys___posix_lchown },		/* 208 = __posix_lchown */
#ifdef SYSVSHM
	{ 4, s(struct linux_sys_shmat_args), 0,
	    linux_sys_shmat },			/* 209 = shmat */
	{ 3, s(struct linux_sys_shmctl_args), 0,
	    linux_sys_shmctl },			/* 210 = shmctl */
	{ 1, s(struct sys_shmdt_args), 0,
	    sys_shmdt },			/* 211 = shmdt */
	{ 3, s(struct sys_shmget_args), 0,
	    sys_shmget },			/* 212 = shmget */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 209 = unimplemented shmat */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 210 = unimplemented shmctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 211 = unimplemented shmdt */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 212 = unimplemented shmget */
#endif
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 213 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 214 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 215 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 216 = unimplemented */
	{ 3, s(struct linux_sys_msync_args), 0,
	    linux_sys_msync },			/* 217 = msync */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 218 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 219 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 220 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 221 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 222 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 223 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 224 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 225 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 226 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 227 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 228 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 229 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 230 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 231 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 232 = unimplemented */
	{ 1, s(struct linux_sys_getpgid_args), 0,
	    linux_sys_getpgid },		/* 233 = getpgid */
	{ 1, s(struct sys_getsid_args), 0,
	    sys_getsid },			/* 234 = getsid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 235 = unimplemented sigaltstack */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 236 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 237 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 238 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 239 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 240 = unimplemented */
	{ 3, s(struct osf1_sys_sysinfo_args), 0,
	    osf1_sys_sysinfo },			/* 241 = sysinfo */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 242 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 243 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 244 = unimplemented osf1_sys_proplist_syscall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 245 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 246 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 247 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 248 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 249 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 250 = unimplemented */
	{ 2, s(struct osf1_sys_usleep_thread_args), 0,
	    osf1_sys_usleep_thread },		/* 251 = usleep_thread */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 252 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 253 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 254 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 255 = unimplemented sysfs */
	{ 5, s(struct osf1_sys_getsysinfo_args), 0,
	    osf1_sys_getsysinfo },		/* 256 = getsysinfo */
	{ 5, s(struct osf1_sys_setsysinfo_args), 0,
	    osf1_sys_setsysinfo },		/* 257 = setsysinfo */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 258 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 259 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 260 = unimplemented */
	{ 1, s(struct linux_sys_fdatasync_args), 0,
	    linux_sys_fdatasync },		/* 261 = fdatasync */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 262 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 263 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 264 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 265 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 266 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 267 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 268 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 269 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 270 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 271 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 272 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 273 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 274 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 275 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 276 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 277 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 278 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 279 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 280 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 281 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 282 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 283 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 284 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 285 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 286 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 287 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 288 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 289 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 290 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 291 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 292 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 293 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 294 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 295 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 296 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 297 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 298 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 299 = unimplemented */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 300 = unimplemented bdflush */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 301 = unimplemented sethae */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 302 = unimplemented mount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 303 = unimplemented old_adjtimex */
	{ 1, s(struct linux_sys_swapoff_args), 0,
	    linux_sys_swapoff },		/* 304 = swapoff */
	{ 3, s(struct linux_sys_getdents_args), 0,
	    linux_sys_getdents },		/* 305 = getdents */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 306 = unimplemented alpha_create_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 307 = unimplemented init_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 308 = unimplemented delete_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 309 = unimplemented get_kernel_syms */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 310 = unimplemented syslog */
	{ 4, s(struct linux_sys_reboot_args), 0,
	    linux_sys_reboot },			/* 311 = reboot */
	{ 2, s(struct linux_sys_clone_args), 0,
	    linux_sys_clone },			/* 312 = clone */
#ifdef EXEC_AOUT
	{ 1, s(struct linux_sys_uselib_args), 0,
	    linux_sys_uselib },			/* 313 = uselib */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 313 = unimplemented sys_uselib */
#endif
	{ 2, s(struct sys_mlock_args), 0,
	    sys_mlock },			/* 314 = mlock */
	{ 2, s(struct sys_munlock_args), 0,
	    sys_munlock },			/* 315 = munlock */
	{ 1, s(struct sys_mlockall_args), 0,
	    sys_mlockall },			/* 316 = mlockall */
	{ 0, 0, 0,
	    sys_munlockall },			/* 317 = munlockall */
	{ 1, s(struct linux_sys_sysinfo_args), 0,
	    linux_sys_sysinfo },		/* 318 = sysinfo */
	{ 1, s(struct linux_sys___sysctl_args), 0,
	    linux_sys___sysctl },		/* 319 = __sysctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 320 = unimplemented idle */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 321 = unimplemented oldumount */
	{ 1, s(struct linux_sys_swapon_args), 0,
	    linux_sys_swapon },			/* 322 = swapon */
	{ 1, s(struct linux_sys_times_args), 0,
	    linux_sys_times },			/* 323 = times */
	{ 1, s(struct linux_sys_personality_args), 0,
	    linux_sys_personality },		/* 324 = personality */
	{ 1, s(struct linux_sys_setfsuid_args), 0,
	    linux_sys_setfsuid },		/* 325 = setfsuid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 326 = unimplemented setfsgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 327 = unimplemented ustat */
	{ 2, s(struct linux_sys_statfs_args), 0,
	    linux_sys_statfs },			/* 328 = statfs */
	{ 2, s(struct linux_sys_fstatfs_args), 0,
	    linux_sys_fstatfs },		/* 329 = fstatfs */
	{ 2, s(struct linux_sys_sched_setparam_args), 0,
	    linux_sys_sched_setparam },		/* 330 = sched_setparam */
	{ 2, s(struct linux_sys_sched_getparam_args), 0,
	    linux_sys_sched_getparam },		/* 331 = sched_getparam */
	{ 3, s(struct linux_sys_sched_setscheduler_args), 0,
	    linux_sys_sched_setscheduler },	/* 332 = sched_setscheduler */
	{ 1, s(struct linux_sys_sched_getscheduler_args), 0,
	    linux_sys_sched_getscheduler },	/* 333 = sched_getscheduler */
	{ 0, 0, 0,
	    linux_sys_sched_yield },		/* 334 = sched_yield */
	{ 1, s(struct linux_sys_sched_get_priority_max_args), 0,
	    linux_sys_sched_get_priority_max },	/* 335 = sched_get_priority_max */
	{ 1, s(struct linux_sys_sched_get_priority_min_args), 0,
	    linux_sys_sched_get_priority_min },	/* 336 = sched_get_priority_min */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 337 = unimplemented sched_rr_get_interval */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 338 = unimplemented afs_syscall */
	{ 1, s(struct linux_sys_uname_args), 0,
	    linux_sys_uname },			/* 339 = uname */
	{ 2, s(struct sys_nanosleep_args), 0,
	    sys_nanosleep },			/* 340 = nanosleep */
	{ 4, s(struct linux_sys_mremap_args), 0,
	    linux_sys_mremap },			/* 341 = mremap */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 342 = unimplemented nfsservctl */
	{ 3, s(struct linux_sys_setresuid_args), 0,
	    linux_sys_setresuid },		/* 343 = setresuid */
	{ 3, s(struct linux_sys_getresuid_args), 0,
	    linux_sys_getresuid },		/* 344 = getresuid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 345 = unimplemented pciconfig_read */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 346 = unimplemented pciconfig_write */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 347 = unimplemented query_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 348 = unimplemented prctl */
	{ 4, s(struct linux_sys_pread_args), 0,
	    linux_sys_pread },			/* 349 = pread */
	{ 4, s(struct linux_sys_pwrite_args), 0,
	    linux_sys_pwrite },			/* 350 = pwrite */
	{ 1, s(struct linux_sys_rt_sigreturn_args), 0,
	    linux_sys_rt_sigreturn },		/* 351 = rt_sigreturn */
	{ 4, s(struct linux_sys_rt_sigaction_args), 0,
	    linux_sys_rt_sigaction },		/* 352 = rt_sigaction */
	{ 4, s(struct linux_sys_rt_sigprocmask_args), 0,
	    linux_sys_rt_sigprocmask },		/* 353 = rt_sigprocmask */
	{ 2, s(struct linux_sys_rt_sigpending_args), 0,
	    linux_sys_rt_sigpending },		/* 354 = rt_sigpending */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 355 = unimplemented rt_sigtimedwait */
	{ 3, s(struct linux_sys_rt_queueinfo_args), 0,
	    linux_sys_rt_queueinfo },		/* 356 = rt_queueinfo */
	{ 2, s(struct linux_sys_rt_sigsuspend_args), 0,
	    linux_sys_rt_sigsuspend },		/* 357 = rt_sigsuspend */
	{ 5, s(struct linux_sys_select_args), 0,
	    linux_sys_select },			/* 358 = select */
	{ 2, s(struct sys_gettimeofday_args), 0,
	    sys_gettimeofday },			/* 359 = gettimeofday */
	{ 2, s(struct sys_settimeofday_args), 0,
	    sys_settimeofday },			/* 360 = settimeofday */
	{ 2, s(struct sys_getitimer_args), 0,
	    sys_getitimer },			/* 361 = getitimer */
	{ 3, s(struct sys_setitimer_args), 0,
	    sys_setitimer },			/* 362 = setitimer */
	{ 2, s(struct sys_utimes_args), 0,
	    sys_utimes },			/* 363 = utimes */
	{ 2, s(struct sys_getrusage_args), 0,
	    sys_getrusage },			/* 364 = getrusage */
	{ 4, s(struct linux_sys_wait4_args), 0,
	    linux_sys_wait4 },			/* 365 = wait4 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 366 = unimplemented adjtimex */
	{ 2, s(struct sys___getcwd_args), 0,
	    sys___getcwd },			/* 367 = __getcwd */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 368 = unimplemented capget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 369 = unimplemented capset */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 370 = unimplemented sendfile */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 371 = unimplemented setresgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 372 = unimplemented getresgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 373 = unimplemented sys_dipc */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 374 = unimplemented pivot_root */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 375 = unimplemented mincore */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 376 = unimplemented pciconfig_iobase */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 377 = unimplemented getdents64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 378 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 379 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 380 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 381 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 382 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 383 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 384 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 385 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 386 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 387 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 388 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 389 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 390 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 391 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 392 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 393 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 394 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 395 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 396 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 397 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 398 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 399 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 400 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 401 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 402 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 403 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 404 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 405 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 406 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 407 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 408 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 409 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 410 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 411 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 412 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 413 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 414 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 415 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 416 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 417 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 418 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 419 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 420 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 421 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 422 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 423 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 424 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 425 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 426 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 427 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 428 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 429 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 430 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 431 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 432 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 433 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 434 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 435 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 436 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 437 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 438 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 439 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 440 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 441 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 442 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 443 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 444 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 445 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 446 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 447 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 448 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 449 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 450 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 451 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 452 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 453 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 454 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 455 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 456 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 457 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 458 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 459 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 460 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 461 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 462 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 463 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 464 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 465 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 466 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 467 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 468 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 469 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 470 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 471 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 472 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 473 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 474 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 475 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 476 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 477 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 478 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 479 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 480 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 481 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 482 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 483 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 484 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 485 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 486 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 487 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 488 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 489 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 490 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 491 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 492 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 493 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 494 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 495 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 496 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 497 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 498 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 499 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 500 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 501 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 502 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 503 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 504 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 505 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 506 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 507 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 508 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 509 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 510 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 511 = filler */
};

