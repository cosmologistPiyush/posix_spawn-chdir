/*	$NetBSD: linux_sysent.c,v 1.14 1999/12/05 21:26:36 tron Exp $	*/

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.14 1999/12/05 21:25:36 tron Exp 
 */

#include "opt_sysv.h"
#include "opt_compat_43.h"
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
	{ 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args),
	    sys_exit },				/* 1 = exit */
	{ 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct sys_read_args),
	    sys_read },				/* 3 = read */
	{ 3, s(struct sys_write_args),
	    sys_write },			/* 4 = write */
	{ 0, 0,
	    sys_nosys },			/* 5 = unimplemented */
	{ 1, s(struct sys_close_args),
	    sys_close },			/* 6 = close */
	{ 0, 0,
	    sys_nosys },			/* 7 = unimplemented osf_wait4 */
	{ 2, s(struct linux_sys_creat_args),
	    linux_sys_creat },			/* 8 = creat */
	{ 2, s(struct sys_link_args),
	    sys_link },				/* 9 = link */
	{ 1, s(struct linux_sys_unlink_args),
	    linux_sys_unlink },			/* 10 = unlink */
	{ 0, 0,
	    sys_nosys },			/* 11 = unimplemented */
	{ 1, s(struct linux_sys_chdir_args),
	    linux_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct sys_fchdir_args),
	    sys_fchdir },			/* 13 = fchdir */
	{ 3, s(struct linux_sys_mknod_args),
	    linux_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct linux_sys_chmod_args),
	    linux_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct linux_sys_chown_args),
	    linux_sys_chown },			/* 16 = chown */
	{ 1, s(struct linux_sys_brk_args),
	    linux_sys_brk },			/* 17 = brk */
	{ 0, 0,
	    sys_nosys },			/* 18 = unimplemented */
	{ 3, s(struct compat_43_sys_lseek_args),
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0,
	    sys_getpid },			/* 20 = getpid */
	{ 0, 0,
	    sys_nosys },			/* 21 = unimplemented osf_mount */
	{ 0, 0,
	    sys_nosys },			/* 22 = unimplemented osf_umount */
	{ 1, s(struct sys_setuid_args),
	    sys_setuid },			/* 23 = setuid */
	{ 0, 0,
	    sys_getuid },			/* 24 = getuid */
	{ 0, 0,
	    sys_nosys },			/* 25 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 26 = unimplemented ptrace */
	{ 0, 0,
	    sys_nosys },			/* 27 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 28 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 29 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 30 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 31 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 32 = unimplemented */
	{ 2, s(struct linux_sys_access_args),
	    linux_sys_access },			/* 33 = access */
	{ 0, 0,
	    sys_nosys },			/* 34 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 35 = unimplemented */
	{ 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct linux_sys_kill_args),
	    linux_sys_kill },			/* 37 = kill */
	{ 0, 0,
	    sys_nosys },			/* 38 = unimplemented */
	{ 2, s(struct sys_setpgid_args),
	    sys_setpgid },			/* 39 = setpgid */
	{ 0, 0,
	    sys_nosys },			/* 40 = unimplemented */
	{ 1, s(struct sys_dup_args),
	    sys_dup },				/* 41 = dup */
	{ 0, 0,
	    linux_sys_pipe },			/* 42 = pipe */
	{ 0, 0,
	    sys_nosys },			/* 43 = unimplemented osf_set_program_attributes */
	{ 0, 0,
	    sys_nosys },			/* 44 = unimplemented */
	{ 3, s(struct linux_sys_open_args),
	    linux_sys_open },			/* 45 = open */
	{ 0, 0,
	    sys_nosys },			/* 46 = unimplemented */
	{ 0, 0,
	    sys_getgid },			/* 47 = getgid */
	{ 0, 0,
	    sys_nosys },			/* 48 = unimplemented osf_sigprocmask */
	{ 0, 0,
	    sys_nosys },			/* 49 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 50 = unimplemented */
	{ 1, s(struct sys_acct_args),
	    sys_acct },				/* 51 = acct */
	{ 1, s(struct linux_sys_sigpending_args),
	    linux_sys_sigpending },		/* 52 = sigpending */
	{ 0, 0,
	    sys_nosys },			/* 53 = unimplemented */
	{ 3, s(struct linux_sys_ioctl_args),
	    linux_sys_ioctl },			/* 54 = ioctl */
	{ 0, 0,
	    sys_nosys },			/* 55 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 56 = unimplemented */
	{ 2, s(struct linux_sys_symlink_args),
	    linux_sys_symlink },		/* 57 = symlink */
	{ 3, s(struct linux_sys_readlink_args),
	    linux_sys_readlink },		/* 58 = readlink */
	{ 3, s(struct linux_sys_execve_args),
	    linux_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args),
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args),
	    sys_chroot },			/* 61 = chroot */
	{ 0, 0,
	    sys_nosys },			/* 62 = unimplemented */
	{ 0, 0,
	    sys_getpgrp },			/* 63 = getpgrp */
	{ 0, 0,
	    compat_43_sys_getpagesize },	/* 64 = getpagesize */
	{ 0, 0,
	    sys_nosys },			/* 65 = unimplemented */
	{ 0, 0,
	    sys___vfork14 },			/* 66 = __vfork14 */
	{ 2, s(struct linux_sys_stat_args),
	    linux_sys_stat },			/* 67 = stat */
	{ 2, s(struct linux_sys_lstat_args),
	    linux_sys_lstat },			/* 68 = lstat */
	{ 0, 0,
	    sys_nosys },			/* 69 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 70 = unimplemented */
	{ 6, s(struct linux_sys_mmap_args),
	    linux_sys_mmap },			/* 71 = mmap */
	{ 0, 0,
	    sys_nosys },			/* 72 = unimplemented */
	{ 2, s(struct sys_munmap_args),
	    sys_munmap },			/* 73 = munmap */
	{ 3, s(struct sys_mprotect_args),
	    sys_mprotect },			/* 74 = mprotect */
	{ 0, 0,
	    sys_nosys },			/* 75 = unimplemented madvise */
	{ 0, 0,
	    sys_nosys },			/* 76 = unimplemented vhangup */
	{ 0, 0,
	    sys_nosys },			/* 77 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 78 = unimplemented */
	{ 2, s(struct sys_getgroups_args),
	    sys_getgroups },			/* 79 = getgroups */
	{ 2, s(struct sys_setgroups_args),
	    sys_setgroups },			/* 80 = setgroups */
	{ 0, 0,
	    sys_nosys },			/* 81 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 82 = unimplemented setpgrp */
	{ 0, 0,
	    sys_nosys },			/* 83 = unimplemented osf_setitimer */
	{ 0, 0,
	    sys_nosys },			/* 84 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 85 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 86 = unimplemented osf_getitimer */
	{ 2, s(struct compat_43_sys_gethostname_args),
	    compat_43_sys_gethostname },	/* 87 = gethostname */
	{ 2, s(struct compat_43_sys_sethostname_args),
	    compat_43_sys_sethostname },	/* 88 = sethostname */
	{ 0, 0,
	    sys_nosys },			/* 89 = unimplemented getdtablesize */
	{ 2, s(struct sys_dup2_args),
	    sys_dup2 },				/* 90 = dup2 */
	{ 2, s(struct linux_sys_fstat_args),
	    linux_sys_fstat },			/* 91 = fstat */
	{ 3, s(struct linux_sys_fcntl_args),
	    linux_sys_fcntl },			/* 92 = fcntl */
	{ 0, 0,
	    sys_nosys },			/* 93 = unimplemented osf_select */
	{ 3, s(struct sys_poll_args),
	    sys_poll },				/* 94 = poll */
	{ 1, s(struct sys_fsync_args),
	    sys_fsync },			/* 95 = fsync */
	{ 3, s(struct sys_setpriority_args),
	    sys_setpriority },			/* 96 = setpriority */
	{ 3, s(struct linux_sys_socket_args),
	    linux_sys_socket },			/* 97 = socket */
	{ 3, s(struct sys_connect_args),
	    sys_connect },			/* 98 = connect */
	{ 3, s(struct compat_43_sys_accept_args),
	    compat_43_sys_accept },		/* 99 = accept */
	{ 2, s(struct sys_getpriority_args),
	    sys_getpriority },			/* 100 = getpriority */
	{ 4, s(struct compat_43_sys_send_args),
	    compat_43_sys_send },		/* 101 = send */
	{ 4, s(struct compat_43_sys_recv_args),
	    compat_43_sys_recv },		/* 102 = recv */
	{ 1, s(struct linux_sys_sigreturn_args),
	    linux_sys_sigreturn },		/* 103 = sigreturn */
	{ 3, s(struct sys_bind_args),
	    sys_bind },				/* 104 = bind */
	{ 5, s(struct linux_sys_setsockopt_args),
	    linux_sys_setsockopt },		/* 105 = setsockopt */
	{ 2, s(struct sys_listen_args),
	    sys_listen },			/* 106 = listen */
	{ 0, 0,
	    sys_nosys },			/* 107 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 108 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 109 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 110 = unimplemented */
	{ 3, s(struct linux_sys_sigsuspend_args),
	    linux_sys_sigsuspend },		/* 111 = sigsuspend */
	{ 0, 0,
	    sys_nosys },			/* 112 = unimplemented */
	{ 3, s(struct sys_recvmsg_args),
	    sys_recvmsg },			/* 113 = recvmsg */
	{ 3, s(struct sys_sendmsg_args),
	    sys_sendmsg },			/* 114 = sendmsg */
	{ 0, 0,
	    sys_nosys },			/* 115 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 116 = unimplemented osf_gettimeofday */
	{ 0, 0,
	    sys_nosys },			/* 117 = unimplemented osf_getrusage */
	{ 5, s(struct linux_sys_getsockopt_args),
	    linux_sys_getsockopt },		/* 118 = getsockopt */
	{ 0, 0,
	    sys_nosys },			/* 119 = unimplemented */
	{ 3, s(struct sys_readv_args),
	    sys_readv },			/* 120 = readv */
	{ 3, s(struct sys_writev_args),
	    sys_writev },			/* 121 = writev */
	{ 0, 0,
	    sys_nosys },			/* 122 = unimplemented osf_settimeofday */
	{ 3, s(struct linux_sys_fchown_args),
	    linux_sys_fchown },			/* 123 = fchown */
	{ 2, s(struct sys_fchmod_args),
	    sys_fchmod },			/* 124 = fchmod */
	{ 6, s(struct linux_sys_recvfrom_args),
	    linux_sys_recvfrom },		/* 125 = recvfrom */
	{ 2, s(struct linux_sys_setreuid_args),
	    linux_sys_setreuid },		/* 126 = setreuid */
	{ 2, s(struct linux_sys_setregid_args),
	    linux_sys_setregid },		/* 127 = setregid */
	{ 2, s(struct linux_sys_rename_args),
	    linux_sys_rename },			/* 128 = rename */
	{ 2, s(struct linux_sys_truncate_args),
	    linux_sys_truncate },		/* 129 = truncate */
	{ 2, s(struct compat_43_sys_ftruncate_args),
	    compat_43_sys_ftruncate },		/* 130 = ftruncate */
	{ 2, s(struct sys_flock_args),
	    sys_flock },			/* 131 = flock */
	{ 1, s(struct sys_setgid_args),
	    sys_setgid },			/* 132 = setgid */
	{ 6, s(struct linux_sys_sendto_args),
	    linux_sys_sendto },			/* 133 = sendto */
	{ 2, s(struct sys_shutdown_args),
	    sys_shutdown },			/* 134 = shutdown */
	{ 4, s(struct linux_sys_socketpair_args),
	    linux_sys_socketpair },		/* 135 = socketpair */
	{ 2, s(struct linux_sys_mkdir_args),
	    linux_sys_mkdir },			/* 136 = mkdir */
	{ 1, s(struct linux_sys_rmdir_args),
	    linux_sys_rmdir },			/* 137 = rmdir */
	{ 0, 0,
	    sys_nosys },			/* 138 = unimplemented osf_utimes */
	{ 0, 0,
	    sys_nosys },			/* 139 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 140 = unimplemented */
	{ 3, s(struct compat_43_sys_getpeername_args),
	    compat_43_sys_getpeername },	/* 141 = getpeername */
	{ 0, 0,
	    sys_nosys },			/* 142 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 143 = unimplemented */
	{ 2, s(struct compat_43_sys_getrlimit_args),
	    compat_43_sys_getrlimit },		/* 144 = getrlimit */
	{ 2, s(struct compat_43_sys_setrlimit_args),
	    compat_43_sys_setrlimit },		/* 145 = setrlimit */
	{ 0, 0,
	    sys_nosys },			/* 146 = unimplemented */
	{ 0, 0,
	    sys_setsid },			/* 147 = setsid */
	{ 0, 0,
	    sys_nosys },			/* 148 = unimplemented quotactl */
	{ 0, 0,
	    sys_nosys },			/* 149 = unimplemented */
	{ 3, s(struct compat_43_sys_getsockname_args),
	    compat_43_sys_getsockname },	/* 150 = getsockname */
	{ 0, 0,
	    sys_nosys },			/* 151 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 152 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 153 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 154 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 155 = unimplemented */
	{ 3, s(struct linux_sys_sigaction_args),
	    linux_sys_sigaction },		/* 156 = sigaction */
	{ 0, 0,
	    sys_nosys },			/* 157 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 158 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 159 = unimplemented osf_getdirentries */
	{ 0, 0,
	    sys_nosys },			/* 160 = unimplemented osf_statfs */
	{ 0, 0,
	    sys_nosys },			/* 161 = unimplemented osf_fstatfs */
	{ 0, 0,
	    sys_nosys },			/* 162 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 163 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 164 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 165 = unimplemented osf_getdomainname */
	{ 0, 0,
	    sys_nosys },			/* 166 = unimplemented setdomainname */
	{ 0, 0,
	    sys_nosys },			/* 167 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 168 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 169 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 170 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 171 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 172 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 173 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 174 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 175 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 176 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 177 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 178 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 179 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 180 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 181 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 182 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 183 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 184 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 185 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 186 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 187 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 188 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 189 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 190 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 191 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 192 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 193 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 194 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 195 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 196 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 197 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 198 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 199 = unimplemented osf_swapon */
#ifdef SYSVMSG
	{ 3, s(struct linux_sys_msgctl_args),
	    linux_sys_msgctl },			/* 200 = msgctl */
	{ 2, s(struct sys_msgget_args),
	    sys_msgget },			/* 201 = msgget */
	{ 4, s(struct sys_msgrcv_args),
	    sys_msgrcv },			/* 202 = msgrcv */
	{ 4, s(struct sys_msgsnd_args),
	    sys_msgsnd },			/* 203 = msgsnd */
#else
	{ 0, 0,
	    sys_nosys },			/* 200 = unimplemented msgctl */
	{ 0, 0,
	    sys_nosys },			/* 201 = unimplemented msgget */
	{ 0, 0,
	    sys_nosys },			/* 202 = unimplemented msgrcv */
	{ 0, 0,
	    sys_nosys },			/* 203 = unimplemented msgsnd */
#endif
#ifdef SYSVSEM
	{ 4, s(struct linux_sys_semctl_args),
	    linux_sys_semctl },			/* 204 = semctl */
	{ 3, s(struct sys_semget_args),
	    sys_semget },			/* 205 = semget */
	{ 3, s(struct sys_semop_args),
	    sys_semop },			/* 206 = semop */
#else
	{ 0, 0,
	    sys_nosys },			/* 204 = unimplemented semctl */
	{ 0, 0,
	    sys_nosys },			/* 205 = unimplemented semget */
	{ 0, 0,
	    sys_nosys },			/* 206 = unimplemented semop */
#endif
	{ 1, s(struct linux_sys_olduname_args),
	    linux_sys_olduname },		/* 207 = olduname */
	{ 3, s(struct linux_sys_lchown_args),
	    linux_sys_lchown },			/* 208 = lchown */
#ifdef SYSVSHM
	{ 4, s(struct linux_sys_shmat_args),
	    linux_sys_shmat },			/* 209 = shmat */
	{ 3, s(struct linux_sys_shmctl_args),
	    linux_sys_shmctl },			/* 210 = shmctl */
	{ 1, s(struct sys_shmdt_args),
	    sys_shmdt },			/* 211 = shmdt */
	{ 3, s(struct sys_shmget_args),
	    sys_shmget },			/* 212 = shmget */
#else
	{ 0, 0,
	    sys_nosys },			/* 209 = unimplemented shmat */
	{ 0, 0,
	    sys_nosys },			/* 210 = unimplemented shmctl */
	{ 0, 0,
	    sys_nosys },			/* 211 = unimplemented shmdt */
	{ 0, 0,
	    sys_nosys },			/* 212 = unimplemented shmget */
#endif
	{ 0, 0,
	    sys_nosys },			/* 213 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 214 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 215 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 216 = unimplemented */
	{ 3, s(struct linux_sys_msync_args),
	    linux_sys_msync },			/* 217 = msync */
	{ 0, 0,
	    sys_nosys },			/* 218 = unimplemented osf_signal */
	{ 0, 0,
	    sys_nosys },			/* 219 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 220 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 221 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 222 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 223 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 224 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 225 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 226 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 227 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 228 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 229 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 230 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 231 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 232 = unimplemented */
	{ 1, s(struct linux_sys_getpgid_args),
	    linux_sys_getpgid },		/* 233 = getpgid */
	{ 1, s(struct sys_getsid_args),
	    sys_getsid },			/* 234 = getsid */
	{ 0, 0,
	    sys_nosys },			/* 235 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 236 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 237 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 238 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 239 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 240 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 241 = unimplemented osf_sysinfo */
	{ 0, 0,
	    sys_nosys },			/* 242 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 243 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 244 = unimplemented osf_proplist_syscall */
	{ 0, 0,
	    sys_nosys },			/* 245 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 246 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 247 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 248 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 249 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 250 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 251 = unimplemented osf_usleep_thread */
	{ 0, 0,
	    sys_nosys },			/* 252 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 253 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 254 = unimplemented sysfs */
	{ 0, 0,
	    sys_nosys },			/* 255 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 256 = unimplemented osf_getsysinfo */
	{ 0, 0,
	    sys_nosys },			/* 257 = unimplemented osf_setsysinfo */
	{ 0, 0,
	    sys_nosys },			/* 258 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 259 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 260 = unimplemented */
	{ 1, s(struct linux_sys_fdatasync_args),
	    linux_sys_fdatasync },		/* 261 = fdatasync */
	{ 0, 0,
	    sys_nosys },			/* 262 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 263 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 264 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 265 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 266 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 267 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 268 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 269 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 270 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 271 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 272 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 273 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 274 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 275 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 276 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 277 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 278 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 279 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 280 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 281 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 282 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 283 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 284 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 285 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 286 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 287 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 288 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 289 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 290 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 291 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 292 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 293 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 294 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 295 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 296 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 297 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 298 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 299 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 300 = unimplemented bdflush */
	{ 0, 0,
	    sys_nosys },			/* 301 = unimplemented sethae */
	{ 0, 0,
	    sys_nosys },			/* 302 = unimplemented mount */
	{ 0, 0,
	    sys_nosys },			/* 303 = unimplemented old_adjtimex */
	{ 0, 0,
	    sys_nosys },			/* 304 = unimplemented swapoff */
	{ 3, s(struct linux_sys_getdents_args),
	    linux_sys_getdents },		/* 305 = getdents */
	{ 0, 0,
	    sys_nosys },			/* 306 = unimplemented create_module */
	{ 0, 0,
	    sys_nosys },			/* 307 = unimplemented init_module */
	{ 0, 0,
	    sys_nosys },			/* 308 = unimplemented delete_module */
	{ 0, 0,
	    sys_nosys },			/* 309 = unimplemented get_kernel_syms */
	{ 0, 0,
	    sys_nosys },			/* 310 = unimplemented syslog */
	{ 1, s(struct sys_reboot_args),
	    sys_reboot },			/* 311 = reboot */
	{ 0, 0,
	    sys_nosys },			/* 312 = unimplemented { int linux_sys_clone ( int flags , void * stack ) ; } */
#ifdef EXEC_AOUT
	{ 1, s(struct linux_sys_uselib_args),
	    linux_sys_uselib },			/* 313 = uselib */
#else
	{ 0, 0,
	    sys_nosys },			/* 313 = unimplemented sys_uselib */
#endif
	{ 2, s(struct sys_mlock_args),
	    sys_mlock },			/* 314 = mlock */
	{ 2, s(struct sys_munlock_args),
	    sys_munlock },			/* 315 = munlock */
	{ 0, 0,
	    sys_nosys },			/* 316 = unimplemented mlockall */
	{ 0, 0,
	    sys_nosys },			/* 317 = unimplemented munlockall */
	{ 0, 0,
	    sys_nosys },			/* 318 = unimplemented sysinfo */
	{ 1, s(struct linux_sys___sysctl_args),
	    linux_sys___sysctl },		/* 319 = __sysctl */
	{ 0, 0,
	    sys_nosys },			/* 320 = unimplemented idle */
	{ 0, 0,
	    sys_nosys },			/* 321 = unimplemented umount */
	{ 1, s(struct compat_12_sys_swapon_args),
	    compat_12_sys_swapon },		/* 322 = swapon */
	{ 1, s(struct linux_sys_times_args),
	    linux_sys_times },			/* 323 = times */
	{ 1, s(struct linux_sys_personality_args),
	    linux_sys_personality },		/* 324 = personality */
	{ 0, 0,
	    sys_nosys },			/* 325 = unimplemented setfsuid */
	{ 0, 0,
	    sys_nosys },			/* 326 = unimplemented setfsgid */
	{ 0, 0,
	    sys_nosys },			/* 327 = unimplemented ustat */
	{ 2, s(struct linux_sys_statfs_args),
	    linux_sys_statfs },			/* 328 = statfs */
	{ 2, s(struct linux_sys_fstatfs_args),
	    linux_sys_fstatfs },		/* 329 = fstatfs */
	{ 2, s(struct linux_sys_sched_setparam_args),
	    linux_sys_sched_setparam },		/* 330 = sched_setparam */
	{ 2, s(struct linux_sys_sched_getparam_args),
	    linux_sys_sched_getparam },		/* 331 = sched_getparam */
	{ 3, s(struct linux_sys_sched_setscheduler_args),
	    linux_sys_sched_setscheduler },	/* 332 = sched_setscheduler */
	{ 1, s(struct linux_sys_sched_getscheduler_args),
	    linux_sys_sched_getscheduler },	/* 333 = sched_getscheduler */
	{ 0, 0,
	    linux_sys_sched_yield },		/* 334 = sched_yield */
	{ 1, s(struct linux_sys_sched_get_priority_max_args),
	    linux_sys_sched_get_priority_max },	/* 335 = sched_get_priority_max */
	{ 1, s(struct linux_sys_sched_get_priority_min_args),
	    linux_sys_sched_get_priority_min },	/* 336 = sched_get_priority_min */
	{ 0, 0,
	    sys_nosys },			/* 337 = unimplemented sched_rr_get_interval */
	{ 0, 0,
	    sys_nosys },			/* 338 = unimplemented afs_syscall */
	{ 1, s(struct linux_sys_uname_args),
	    linux_sys_uname },			/* 339 = uname */
	{ 2, s(struct sys_nanosleep_args),
	    sys_nanosleep },			/* 340 = nanosleep */
	{ 4, s(struct linux_sys_mremap_args),
	    linux_sys_mremap },			/* 341 = mremap */
	{ 0, 0,
	    sys_nosys },			/* 342 = unimplemented nfsservctl */
	{ 3, s(struct linux_sys_setresuid_args),
	    linux_sys_setresuid },		/* 343 = setresuid */
	{ 3, s(struct linux_sys_getresuid_args),
	    linux_sys_getresuid },		/* 344 = getresuid */
	{ 0, 0,
	    sys_nosys },			/* 345 = unimplemented pciconfig_read */
	{ 0, 0,
	    sys_nosys },			/* 346 = unimplemented pciconfig_write */
	{ 0, 0,
	    sys_nosys },			/* 347 = unimplemented query_module */
	{ 0, 0,
	    sys_nosys },			/* 348 = unimplemented prctl */
	{ 4, s(struct linux_sys_pread_args),
	    linux_sys_pread },			/* 349 = pread */
	{ 4, s(struct linux_sys_pwrite_args),
	    linux_sys_pwrite },			/* 350 = pwrite */
	{ 1, s(struct linux_sys_rt_sigreturn_args),
	    linux_sys_rt_sigreturn },		/* 351 = rt_sigreturn */
	{ 4, s(struct linux_sys_rt_sigaction_args),
	    linux_sys_rt_sigaction },		/* 352 = rt_sigaction */
	{ 4, s(struct linux_sys_rt_sigprocmask_args),
	    linux_sys_rt_sigprocmask },		/* 353 = rt_sigprocmask */
	{ 2, s(struct linux_sys_rt_sigpending_args),
	    linux_sys_rt_sigpending },		/* 354 = rt_sigpending */
	{ 0, 0,
	    sys_nosys },			/* 355 = unimplemented rt_sigtimedwait */
	{ 3, s(struct linux_sys_rt_queueinfo_args),
	    linux_sys_rt_queueinfo },		/* 356 = rt_queueinfo */
	{ 2, s(struct linux_sys_rt_sigsuspend_args),
	    linux_sys_rt_sigsuspend },		/* 357 = rt_sigsuspend */
	{ 5, s(struct linux_sys_select_args),
	    linux_sys_select },			/* 358 = select */
	{ 2, s(struct sys_gettimeofday_args),
	    sys_gettimeofday },			/* 359 = gettimeofday */
	{ 2, s(struct sys_settimeofday_args),
	    sys_settimeofday },			/* 360 = settimeofday */
	{ 2, s(struct sys_getitimer_args),
	    sys_getitimer },			/* 361 = getitimer */
	{ 3, s(struct sys_setitimer_args),
	    sys_setitimer },			/* 362 = setitimer */
	{ 2, s(struct sys_utimes_args),
	    sys_utimes },			/* 363 = utimes */
	{ 2, s(struct sys_getrusage_args),
	    sys_getrusage },			/* 364 = getrusage */
	{ 4, s(struct linux_sys_wait4_args),
	    linux_sys_wait4 },			/* 365 = wait4 */
	{ 0, 0,
	    sys_nosys },			/* 366 = unimplemented adjtimex */
	{ 2, s(struct sys___getcwd_args),
	    sys___getcwd },			/* 367 = __getcwd */
	{ 0, 0,
	    sys_nosys },			/* 368 = unimplemented capget */
	{ 0, 0,
	    sys_nosys },			/* 369 = unimplemented capset */
	{ 0, 0,
	    sys_nosys },			/* 370 = unimplemented sendfile */
};

