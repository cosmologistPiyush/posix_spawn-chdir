/*	$NetBSD: linux_syscall.h,v 1.13 1999/12/05 21:26:36 tron Exp $	*/

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.14 1999/12/05 21:25:36 tron Exp 
 */

/* syscall: "syscall" ret: "int" args: */
#define	LINUX_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	LINUX_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	LINUX_SYS_fork	2

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	LINUX_SYS_read	3

/* syscall: "write" ret: "int" args: "int" "char *" "u_int" */
#define	LINUX_SYS_write	4

/* syscall: "close" ret: "int" args: "int" */
#define	LINUX_SYS_close	6

/* syscall: "creat" ret: "int" args: "char *" "int" */
#define	LINUX_SYS_creat	8

/* syscall: "link" ret: "int" args: "char *" "char *" */
#define	LINUX_SYS_link	9

/* syscall: "unlink" ret: "int" args: "char *" */
#define	LINUX_SYS_unlink	10

/* syscall: "chdir" ret: "int" args: "char *" */
#define	LINUX_SYS_chdir	12

/* syscall: "fchdir" ret: "int" args: "int" */
#define	LINUX_SYS_fchdir	13

/* syscall: "mknod" ret: "int" args: "char *" "int" "int" */
#define	LINUX_SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "char *" "int" */
#define	LINUX_SYS_chmod	15

/* syscall: "chown" ret: "int" args: "char *" "int" "int" */
#define	LINUX_SYS_chown	16

/* syscall: "brk" ret: "int" args: "char *" */
#define	LINUX_SYS_brk	17

/* syscall: "lseek" ret: "long" args: "int" "long" "int" */
#define	LINUX_SYS_lseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	LINUX_SYS_getpid	20

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	LINUX_SYS_setuid	23

/* syscall: "getuid" ret: "uid_t" args: */
#define	LINUX_SYS_getuid	24

/* syscall: "access" ret: "int" args: "char *" "int" */
#define	LINUX_SYS_access	33

/* syscall: "sync" ret: "int" args: */
#define	LINUX_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	LINUX_SYS_kill	37

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	LINUX_SYS_setpgid	39

/* syscall: "dup" ret: "int" args: "u_int" */
#define	LINUX_SYS_dup	41

/* syscall: "pipe" ret: "int" args: */
#define	LINUX_SYS_pipe	42

/* syscall: "open" ret: "int" args: "char *" "int" "int" */
#define	LINUX_SYS_open	45

/* syscall: "getgid" ret: "gid_t" args: */
#define	LINUX_SYS_getgid	47

/* syscall: "acct" ret: "int" args: "char *" */
#define	LINUX_SYS_acct	51

/* syscall: "sigpending" ret: "int" args: "linux_old_sigset_t *" */
#define	LINUX_SYS_sigpending	52

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "caddr_t" */
#define	LINUX_SYS_ioctl	54

/* syscall: "symlink" ret: "int" args: "char *" "char *" */
#define	LINUX_SYS_symlink	57

/* syscall: "readlink" ret: "int" args: "char *" "char *" "int" */
#define	LINUX_SYS_readlink	58

/* syscall: "execve" ret: "int" args: "char *" "char **" "char **" */
#define	LINUX_SYS_execve	59

/* syscall: "umask" ret: "int" args: "int" */
#define	LINUX_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "char *" */
#define	LINUX_SYS_chroot	61

/* syscall: "getpgrp" ret: "int" args: */
#define	LINUX_SYS_getpgrp	63

/* syscall: "getpagesize" ret: "int" args: */
#define	LINUX_SYS_getpagesize	64

/* syscall: "__vfork14" ret: "int" args: */
#define	LINUX_SYS___vfork14	66

/* syscall: "stat" ret: "int" args: "char *" "struct linux_stat *" */
#define	LINUX_SYS_stat	67

/* syscall: "lstat" ret: "int" args: "char *" "struct linux_stat *" */
#define	LINUX_SYS_lstat	68

/* syscall: "mmap" ret: "int" args: "unsigned long" "size_t" "int" "int" "int" "off_t" */
#define	LINUX_SYS_mmap	71

/* syscall: "munmap" ret: "int" args: "caddr_t" "int" */
#define	LINUX_SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "caddr_t" "int" "int" */
#define	LINUX_SYS_mprotect	74

/* syscall: "getgroups" ret: "int" args: "u_int" "gid_t *" */
#define	LINUX_SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "u_int" "gid_t *" */
#define	LINUX_SYS_setgroups	80

/* syscall: "gethostname" ret: "int" args: "char *" "u_int" */
#define	LINUX_SYS_gethostname	87

/* syscall: "sethostname" ret: "int" args: "char *" "u_int" */
#define	LINUX_SYS_sethostname	88

/* syscall: "dup2" ret: "int" args: "u_int" "u_int" */
#define	LINUX_SYS_dup2	90

/* syscall: "fstat" ret: "int" args: "int" "struct linux_stat *" */
#define	LINUX_SYS_fstat	91

/* syscall: "fcntl" ret: "int" args: "int" "int" "void *" */
#define	LINUX_SYS_fcntl	92

/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	LINUX_SYS_poll	94

/* syscall: "fsync" ret: "int" args: "int" */
#define	LINUX_SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	LINUX_SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	LINUX_SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "const struct sockaddr *" "int" */
#define	LINUX_SYS_connect	98

/* syscall: "accept" ret: "int" args: "int" "caddr_t" "int *" */
#define	LINUX_SYS_accept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	LINUX_SYS_getpriority	100

/* syscall: "send" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	LINUX_SYS_send	101

/* syscall: "recv" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	LINUX_SYS_recv	102

/* syscall: "sigreturn" ret: "int" args: "struct linux_sigframe *" */
#define	LINUX_SYS_sigreturn	103

/* syscall: "bind" ret: "int" args: "int" "const struct sockaddr *" "int" */
#define	LINUX_SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "void *" "int" */
#define	LINUX_SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	LINUX_SYS_listen	106

/* syscall: "sigsuspend" ret: "int" args: "caddr_t" "int" "int" */
#define	LINUX_SYS_sigsuspend	111

/* syscall: "recvmsg" ret: "int" args: "int" "struct msghdr *" "u_int" */
#define	LINUX_SYS_recvmsg	113

/* syscall: "sendmsg" ret: "int" args: "int" "struct msghdr *" "u_int" */
#define	LINUX_SYS_sendmsg	114

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "void *" "int *" */
#define	LINUX_SYS_getsockopt	118

/* syscall: "readv" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	LINUX_SYS_readv	120

/* syscall: "writev" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	LINUX_SYS_writev	121

/* syscall: "fchown" ret: "int" args: "int" "int" "int" */
#define	LINUX_SYS_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	LINUX_SYS_fchmod	124

/* syscall: "recvfrom" ret: "int" args: "int" "void *" "int" "int" "struct sockaddr *" "int *" */
#define	LINUX_SYS_recvfrom	125

/* syscall: "setreuid" ret: "int" args: "int" "int" */
#define	LINUX_SYS_setreuid	126

/* syscall: "setregid" ret: "int" args: "int" "int" */
#define	LINUX_SYS_setregid	127

/* syscall: "rename" ret: "int" args: "char *" "char *" */
#define	LINUX_SYS_rename	128

/* syscall: "truncate" ret: "int" args: "char *" "long" */
#define	LINUX_SYS_truncate	129

/* syscall: "ftruncate" ret: "int" args: "int" "long" */
#define	LINUX_SYS_ftruncate	130

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	LINUX_SYS_flock	131

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	LINUX_SYS_setgid	132

/* syscall: "sendto" ret: "int" args: "int" "void *" "int" "int" "struct sockaddr *" "int" */
#define	LINUX_SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	LINUX_SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	LINUX_SYS_socketpair	135

/* syscall: "mkdir" ret: "int" args: "char *" "int" */
#define	LINUX_SYS_mkdir	136

/* syscall: "rmdir" ret: "int" args: "char *" */
#define	LINUX_SYS_rmdir	137

/* syscall: "getpeername" ret: "int" args: "int" "caddr_t" "int *" */
#define	LINUX_SYS_getpeername	141

/* syscall: "getrlimit" ret: "int" args: "u_int" "struct orlimit *" */
#define	LINUX_SYS_getrlimit	144

/* syscall: "setrlimit" ret: "int" args: "u_int" "struct orlimit *" */
#define	LINUX_SYS_setrlimit	145

/* syscall: "setsid" ret: "int" args: */
#define	LINUX_SYS_setsid	147

/* syscall: "getsockname" ret: "int" args: "int" "caddr_t" "int *" */
#define	LINUX_SYS_getsockname	150

/* syscall: "sigaction" ret: "int" args: "int" "const struct linux_old_sigaction *" "struct linux_old_sigaction *" */
#define	LINUX_SYS_sigaction	156

/* syscall: "msgctl" ret: "int" args: "int" "int" "struct linux_msqid_ds *" */
#define	LINUX_SYS_msgctl	200

/* syscall: "msgget" ret: "int" args: "key_t" "int" */
#define	LINUX_SYS_msgget	201

/* syscall: "msgrcv" ret: "int" args: "int" "void *" "size_t" "int" */
#define	LINUX_SYS_msgrcv	202

/* syscall: "msgsnd" ret: "int" args: "int" "void *" "size_t" "int" */
#define	LINUX_SYS_msgsnd	203

/* syscall: "semctl" ret: "int" args: "int" "int" "int" "union linux_semun" */
#define	LINUX_SYS_semctl	204

/* syscall: "semget" ret: "int" args: "key_t" "int" "int" */
#define	LINUX_SYS_semget	205

/* syscall: "semop" ret: "int" args: "int" "struct sembuf *" "unsigned u_int" */
#define	LINUX_SYS_semop	206

/* syscall: "olduname" ret: "int" args: "struct linux_old_utsname *" */
#define	LINUX_SYS_olduname	207

/* syscall: "lchown" ret: "int" args: "char *" "int" "int" */
#define	LINUX_SYS_lchown	208

/* syscall: "shmat" ret: "int" args: "int" "void *" "int" "u_long *" */
#define	LINUX_SYS_shmat	209

/* syscall: "shmctl" ret: "int" args: "int" "int" "struct linux_shmid_ds *" */
#define	LINUX_SYS_shmctl	210

/* syscall: "shmdt" ret: "int" args: "void *" */
#define	LINUX_SYS_shmdt	211

/* syscall: "shmget" ret: "int" args: "key_t" "int" "int" */
#define	LINUX_SYS_shmget	212

/* syscall: "msync" ret: "int" args: "caddr_t" "int" "int" */
#define	LINUX_SYS_msync	217

/* syscall: "getpgid" ret: "int" args: "int" */
#define	LINUX_SYS_getpgid	233

/* syscall: "getsid" ret: "pid_t" args: "pid_t" */
#define	LINUX_SYS_getsid	234

/* syscall: "fdatasync" ret: "int" args: "int" */
#define	LINUX_SYS_fdatasync	261

/* syscall: "getdents" ret: "int" args: "int" "struct linux_dirent *" "unsigned int" */
#define	LINUX_SYS_getdents	305

/* syscall: "reboot" ret: "int" args: "int" */
#define	LINUX_SYS_reboot	311

/* syscall: "uselib" ret: "int" args: "char *" */
#define	LINUX_SYS_uselib	313

/* syscall: "mlock" ret: "int" args: "const void *" "size_t" */
#define	LINUX_SYS_mlock	314

/* syscall: "munlock" ret: "int" args: "const void *" "size_t" */
#define	LINUX_SYS_munlock	315

/* syscall: "__sysctl" ret: "int" args: "struct linux___sysctl *" */
#define	LINUX_SYS___sysctl	319

/* syscall: "swapon" ret: "int" args: "char *" */
#define	LINUX_SYS_swapon	322

/* syscall: "times" ret: "int" args: "struct times *" */
#define	LINUX_SYS_times	323

/* syscall: "personality" ret: "int" args: "int" */
#define	LINUX_SYS_personality	324

/* syscall: "statfs" ret: "int" args: "char *" "struct linux_statfs *" */
#define	LINUX_SYS_statfs	328

/* syscall: "fstatfs" ret: "int" args: "int" "struct linux_statfs *" */
#define	LINUX_SYS_fstatfs	329

/* syscall: "sched_setparam" ret: "int" args: "pid_t" "const struct linux_sched_param *" */
#define	LINUX_SYS_sched_setparam	330

/* syscall: "sched_getparam" ret: "int" args: "pid_t" "struct linux_sched_param *" */
#define	LINUX_SYS_sched_getparam	331

/* syscall: "sched_setscheduler" ret: "int" args: "pid_t" "int" "const struct linux_sched_param *" */
#define	LINUX_SYS_sched_setscheduler	332

/* syscall: "sched_getscheduler" ret: "int" args: "pid_t" */
#define	LINUX_SYS_sched_getscheduler	333

/* syscall: "sched_yield" ret: "int" args: */
#define	LINUX_SYS_sched_yield	334

/* syscall: "sched_get_priority_max" ret: "int" args: "int" */
#define	LINUX_SYS_sched_get_priority_max	335

/* syscall: "sched_get_priority_min" ret: "int" args: "int" */
#define	LINUX_SYS_sched_get_priority_min	336

/* syscall: "uname" ret: "int" args: "struct linux_utsname *" */
#define	LINUX_SYS_uname	339

/* syscall: "nanosleep" ret: "int" args: "const struct timespec *" "struct timespec *" */
#define	LINUX_SYS_nanosleep	340

/* syscall: "mremap" ret: "void *" args: "void *" "size_t" "size_t" "u_long" */
#define	LINUX_SYS_mremap	341

/* syscall: "setresuid" ret: "int" args: "uid_t" "uid_t" "uid_t" */
#define	LINUX_SYS_setresuid	343

/* syscall: "getresuid" ret: "int" args: "uid_t *" "uid_t *" "uid_t *" */
#define	LINUX_SYS_getresuid	344

/* syscall: "pread" ret: "int" args: "int" "char *" "size_t" "linux_off_t" */
#define	LINUX_SYS_pread	349

/* syscall: "pwrite" ret: "int" args: "int" "char *" "size_t" "linux_off_t" */
#define	LINUX_SYS_pwrite	350

/* syscall: "rt_sigreturn" ret: "int" args: "struct linux_rt_sigframe *" */
#define	LINUX_SYS_rt_sigreturn	351

/* syscall: "rt_sigaction" ret: "int" args: "int" "const struct linux_sigaction *" "struct linux_sigaction *" "size_t" */
#define	LINUX_SYS_rt_sigaction	352

/* syscall: "rt_sigprocmask" ret: "int" args: "int" "const linux_sigset_t *" "linux_sigset_t *" "size_t" */
#define	LINUX_SYS_rt_sigprocmask	353

/* syscall: "rt_sigpending" ret: "int" args: "linux_sigset_t *" "size_t" */
#define	LINUX_SYS_rt_sigpending	354

/* syscall: "rt_queueinfo" ret: "int" args: "int" "int" "linux_siginfo_t *" */
#define	LINUX_SYS_rt_queueinfo	356

/* syscall: "rt_sigsuspend" ret: "int" args: "linux_sigset_t *" "size_t" */
#define	LINUX_SYS_rt_sigsuspend	357

/* syscall: "select" ret: "int" args: "int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	LINUX_SYS_select	358

/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	LINUX_SYS_gettimeofday	359

/* syscall: "settimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	LINUX_SYS_settimeofday	360

/* syscall: "getitimer" ret: "int" args: "u_int" "struct itimerval *" */
#define	LINUX_SYS_getitimer	361

/* syscall: "setitimer" ret: "int" args: "u_int" "struct itimerval *" "struct itimerval *" */
#define	LINUX_SYS_setitimer	362

/* syscall: "utimes" ret: "int" args: "char *" "struct timecal *" */
#define	LINUX_SYS_utimes	363

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage *" */
#define	LINUX_SYS_getrusage	364

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage *" */
#define	LINUX_SYS_wait4	365

/* syscall: "__getcwd" ret: "int" args: "char *" "size_t" */
#define	LINUX_SYS___getcwd	367

#define	LINUX_SYS_MAXSYSCALL	371
