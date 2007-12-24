/* $NetBSD: linux_syscall.h,v 1.22 2007/12/24 14:21:41 njoly Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.22 2007/12/24 14:17:17 njoly Exp
 */

#ifndef _LINUX_SYS_SYSCALL_H_
#define	_LINUX_SYS_SYSCALL_H_

#define	LINUX_SYS_MAXSYSARGS	8

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	LINUX_SYS_read	0

/* syscall: "write" ret: "int" args: "int" "char *" "u_int" */
#define	LINUX_SYS_write	1

/* syscall: "open" ret: "int" args: "const char *" "int" "int" */
#define	LINUX_SYS_open	2

/* syscall: "close" ret: "int" args: "int" */
#define	LINUX_SYS_close	3

/* syscall: "stat64" ret: "int" args: "const char *" "struct linux_stat *" */
#define	LINUX_SYS_stat64	4

/* syscall: "fstat64" ret: "int" args: "int" "struct linux_stat *" */
#define	LINUX_SYS_fstat64	5

/* syscall: "lstat64" ret: "int" args: "const char *" "struct linux_stat *" */
#define	LINUX_SYS_lstat64	6

/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	LINUX_SYS_poll	7

/* syscall: "lseek" ret: "long" args: "int" "long" "int" */
#define	LINUX_SYS_lseek	8

/* syscall: "mmap" ret: "linux_off_t" args: "unsigned long" "size_t" "int" "int" "int" "linux_off_t" */
#define	LINUX_SYS_mmap	9

/* syscall: "mprotect" ret: "int" args: "const void *" "unsigned long" "int" */
#define	LINUX_SYS_mprotect	10

/* syscall: "munmap" ret: "int" args: "void *" "int" */
#define	LINUX_SYS_munmap	11

/* syscall: "brk" ret: "int" args: "char *" */
#define	LINUX_SYS_brk	12

/* syscall: "rt_sigaction" ret: "int" args: "int" "const struct linux_sigaction *" "struct linux_sigaction *" "size_t" */
#define	LINUX_SYS_rt_sigaction	13

/* syscall: "rt_sigprocmask" ret: "int" args: "int" "const linux_sigset_t *" "linux_sigset_t *" "size_t" */
#define	LINUX_SYS_rt_sigprocmask	14

/* syscall: "rt_sigreturn" ret: "int" args: */
#define	LINUX_SYS_rt_sigreturn	15

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "void *" */
#define	LINUX_SYS_ioctl	16

/* syscall: "pread" ret: "int" args: "int" "char *" "size_t" "linux_off_t" */
#define	LINUX_SYS_pread	17

/* syscall: "pwrite" ret: "int" args: "int" "char *" "size_t" "linux_off_t" */
#define	LINUX_SYS_pwrite	18

/* syscall: "readv" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	LINUX_SYS_readv	19

/* syscall: "writev" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	LINUX_SYS_writev	20

/* syscall: "access" ret: "int" args: "const char *" "int" */
#define	LINUX_SYS_access	21

/* syscall: "pipe" ret: "int" args: "int *" */
#define	LINUX_SYS_pipe	22

/* syscall: "select" ret: "int" args: "int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	LINUX_SYS_select	23

/* syscall: "sched_yield" ret: "int" args: */
#define	LINUX_SYS_sched_yield	24

/* syscall: "mremap" ret: "void *" args: "void *" "size_t" "size_t" "u_long" */
#define	LINUX_SYS_mremap	25

/* syscall: "msync" ret: "int" args: "void *" "int" "int" */
#define	LINUX_SYS_msync	26

/* syscall: "mincore" ret: "int" args: "void *" "size_t" "char *" */
#define	LINUX_SYS_mincore	27

/* syscall: "madvise" ret: "int" args: "void *" "size_t" "int" */
#define	LINUX_SYS_madvise	28

#ifdef SYSVSHM
/* syscall: "shmget" ret: "int" args: "key_t" "size_t" "int" */
#define	LINUX_SYS_shmget	29

/* syscall: "shmat" ret: "int" args: "int" "void *" "int" */
#define	LINUX_SYS_shmat	30

/* syscall: "shmctl" ret: "int" args: "int" "int" "struct linux_shmid_ds *" */
#define	LINUX_SYS_shmctl	31

#else
#endif
/* syscall: "dup" ret: "int" args: "u_int" */
#define	LINUX_SYS_dup	32

/* syscall: "dup2" ret: "int" args: "u_int" "u_int" */
#define	LINUX_SYS_dup2	33

/* syscall: "pause" ret: "int" args: */
#define	LINUX_SYS_pause	34

/* syscall: "nanosleep" ret: "int" args: "const struct timespec *" "struct timespec *" */
#define	LINUX_SYS_nanosleep	35

/* syscall: "getitimer" ret: "int" args: "u_int" "struct itimerval *" */
#define	LINUX_SYS_getitimer	36

/* syscall: "alarm" ret: "int" args: "unsigned int" */
#define	LINUX_SYS_alarm	37

/* syscall: "setitimer" ret: "int" args: "u_int" "struct itimerval *" "struct itimerval *" */
#define	LINUX_SYS_setitimer	38

/* syscall: "getpid" ret: "pid_t" args: */
#define	LINUX_SYS_getpid	39

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	LINUX_SYS_socket	41

/* syscall: "connect" ret: "int" args: "int" "const struct osockaddr *" "unsigned int" */
#define	LINUX_SYS_connect	42

/* syscall: "oaccept" ret: "int" args: "int" "struct osockaddr *" "int *" */
#define	LINUX_SYS_oaccept	43

/* syscall: "sendto" ret: "ssize_t" args: "int" "void *" "int" "int" "struct osockaddr *" "int" */
#define	LINUX_SYS_sendto	44

/* syscall: "recvfrom" ret: "ssize_t" args: "int" "void *" "size_t" "int" "struct osockaddr *" "unsigned int *" */
#define	LINUX_SYS_recvfrom	45

/* syscall: "sendmsg" ret: "int" args: "int" "const struct msghdr *" "int" */
#define	LINUX_SYS_sendmsg	46

/* syscall: "recvmsg" ret: "ssize_t" args: "int" "struct msghdr *" "int" */
#define	LINUX_SYS_recvmsg	47

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	LINUX_SYS_shutdown	48

/* syscall: "bind" ret: "int" args: "int" "const struct osockaddr *" "unsigned int" */
#define	LINUX_SYS_bind	49

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	LINUX_SYS_listen	50

/* syscall: "getsockname" ret: "int" args: "int" "void *" "int *" */
#define	LINUX_SYS_getsockname	51

/* syscall: "getpeername" ret: "int" args: "int" "struct sockaddr *" "unsigned int *" */
#define	LINUX_SYS_getpeername	52

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	LINUX_SYS_socketpair	53

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "void *" "int" */
#define	LINUX_SYS_setsockopt	54

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "void *" "int *" */
#define	LINUX_SYS_getsockopt	55

/* syscall: "clone" ret: "int" args: "int" "void *" "void *" "void *" */
#define	LINUX_SYS_clone	56

/* syscall: "fork" ret: "int" args: */
#define	LINUX_SYS_fork	57

/* syscall: "__vfork14" ret: "int" args: */
#define	LINUX_SYS___vfork14	58

/* syscall: "execve" ret: "int" args: "const char *" "char **" "char **" */
#define	LINUX_SYS_execve	59

/* syscall: "exit" ret: "int" args: "int" */
#define	LINUX_SYS_exit	60

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage *" */
#define	LINUX_SYS_wait4	61

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	LINUX_SYS_kill	62

/* syscall: "uname" ret: "int" args: "struct linux_utsname *" */
#define	LINUX_SYS_uname	63

#ifdef SYSVSEM
/* syscall: "semget" ret: "int" args: "key_t" "int" "int" */
#define	LINUX_SYS_semget	64

/* syscall: "semop" ret: "int" args: "int" "struct sembuf *" "size_t" */
#define	LINUX_SYS_semop	65

/* syscall: "semctl" ret: "int" args: "int" "int" "int" "union linux_semun" */
#define	LINUX_SYS_semctl	66

#else
#endif
#ifdef SYSVSHM
/* syscall: "shmdt" ret: "int" args: "const void *" */
#define	LINUX_SYS_shmdt	67

#else
#endif
#ifdef SYSVMSG
/* syscall: "msgget" ret: "int" args: "key_t" "int" */
#define	LINUX_SYS_msgget	68

/* syscall: "msgsnd" ret: "int" args: "int" "void *" "size_t" "int" */
#define	LINUX_SYS_msgsnd	69

/* syscall: "msgrcv" ret: "ssize_t" args: "int" "void *" "size_t" "long" "int" */
#define	LINUX_SYS_msgrcv	70

/* syscall: "msgctl" ret: "int" args: "int" "int" "struct linux_msqid_ds *" */
#define	LINUX_SYS_msgctl	71

#else
#endif
/* syscall: "fcntl" ret: "int" args: "int" "int" "void *" */
#define	LINUX_SYS_fcntl	72

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	LINUX_SYS_flock	73

/* syscall: "fsync" ret: "int" args: "int" */
#define	LINUX_SYS_fsync	74

/* syscall: "fdatasync" ret: "int" args: "int" */
#define	LINUX_SYS_fdatasync	75

/* syscall: "truncate64" ret: "int" args: "const char *" "off_t" */
#define	LINUX_SYS_truncate64	76

/* syscall: "ftruncate64" ret: "int" args: "unsigned int" "off_t" */
#define	LINUX_SYS_ftruncate64	77

/* syscall: "getdents" ret: "int" args: "int" "struct linux_dirent *" "unsigned int" */
#define	LINUX_SYS_getdents	78

/* syscall: "__getcwd" ret: "int" args: "char *" "size_t" */
#define	LINUX_SYS___getcwd	79

/* syscall: "chdir" ret: "int" args: "const char *" */
#define	LINUX_SYS_chdir	80

/* syscall: "fchdir" ret: "int" args: "int" */
#define	LINUX_SYS_fchdir	81

/* syscall: "__posix_rename" ret: "int" args: "const char *" "const char *" */
#define	LINUX_SYS___posix_rename	82

/* syscall: "mkdir" ret: "int" args: "const char *" "int" */
#define	LINUX_SYS_mkdir	83

/* syscall: "rmdir" ret: "int" args: "const char *" */
#define	LINUX_SYS_rmdir	84

/* syscall: "creat" ret: "int" args: "const char *" "int" */
#define	LINUX_SYS_creat	85

/* syscall: "link" ret: "int" args: "const char *" "const char *" */
#define	LINUX_SYS_link	86

/* syscall: "unlink" ret: "int" args: "const char *" */
#define	LINUX_SYS_unlink	87

/* syscall: "symlink" ret: "int" args: "const char *" "const char *" */
#define	LINUX_SYS_symlink	88

/* syscall: "readlink" ret: "int" args: "const char *" "char *" "int" */
#define	LINUX_SYS_readlink	89

/* syscall: "chmod" ret: "int" args: "const char *" "int" */
#define	LINUX_SYS_chmod	90

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	LINUX_SYS_fchmod	91

/* syscall: "__posix_chown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	LINUX_SYS___posix_chown	92

/* syscall: "__posix_fchown" ret: "int" args: "int" "uid_t" "gid_t" */
#define	LINUX_SYS___posix_fchown	93

/* syscall: "__posix_lchown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	LINUX_SYS___posix_lchown	94

/* syscall: "umask" ret: "int" args: "int" */
#define	LINUX_SYS_umask	95

/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	LINUX_SYS_gettimeofday	96

/* syscall: "getrlimit" ret: "int" args: "int" "struct rlimit *" */
#define	LINUX_SYS_getrlimit	97

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage *" */
#define	LINUX_SYS_getrusage	98

/* syscall: "sysinfo" ret: "int" args: "struct linux_sysinfo *" */
#define	LINUX_SYS_sysinfo	99

/* syscall: "times" ret: "int" args: "struct times *" */
#define	LINUX_SYS_times	100

/* syscall: "ptrace" ret: "int" args: "long" "long" "long" "long" */
#define	LINUX_SYS_ptrace	101

/* syscall: "getuid" ret: "uid_t" args: */
#define	LINUX_SYS_getuid	102

/* syscall: "getgid" ret: "gid_t" args: */
#define	LINUX_SYS_getgid	104

/* syscall: "setuid" ret: "void" args: "uid_t" */
#define	LINUX_SYS_setuid	105

/* syscall: "setgid" ret: "void" args: "gid_t" */
#define	LINUX_SYS_setgid	106

/* syscall: "geteuid" ret: "uid_t" args: */
#define	LINUX_SYS_geteuid	107

/* syscall: "getegid" ret: "gid_t" args: */
#define	LINUX_SYS_getegid	108

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	LINUX_SYS_setpgid	109

/* syscall: "getppid" ret: "pid_t" args: */
#define	LINUX_SYS_getppid	110

/* syscall: "getpgrp" ret: "int" args: */
#define	LINUX_SYS_getpgrp	111

/* syscall: "setsid" ret: "int" args: */
#define	LINUX_SYS_setsid	112

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	LINUX_SYS_setreuid	113

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	LINUX_SYS_setregid	114

/* syscall: "getgroups" ret: "int" args: "u_int" "gid_t *" */
#define	LINUX_SYS_getgroups	115

/* syscall: "setgroups" ret: "int" args: "u_int" "gid_t *" */
#define	LINUX_SYS_setgroups	116

/* syscall: "setresuid" ret: "int" args: "uid_t" "uid_t" "uid_t" */
#define	LINUX_SYS_setresuid	117

/* syscall: "getresuid" ret: "int" args: "uid_t *" "uid_t *" "uid_t *" */
#define	LINUX_SYS_getresuid	118

/* syscall: "setresgid" ret: "int" args: "gid_t" "gid_t" "gid_t" */
#define	LINUX_SYS_setresgid	119

/* syscall: "getresgid" ret: "int" args: "gid_t *" "gid_t *" "gid_t *" */
#define	LINUX_SYS_getresgid	120

/* syscall: "getpgid" ret: "pid_t" args: "pid_t" */
#define	LINUX_SYS_getpgid	121

/* syscall: "setfsuid" ret: "int" args: "uid_t" */
#define	LINUX_SYS_setfsuid	122

/* syscall: "setfsgid" ret: "int" args: "gid_t" */
#define	LINUX_SYS_setfsgid	123

/* syscall: "getsid" ret: "pid_t" args: "pid_t" */
#define	LINUX_SYS_getsid	124

/* syscall: "rt_sigpending" ret: "int" args: "linux_sigset_t *" "size_t" */
#define	LINUX_SYS_rt_sigpending	127

/* syscall: "rt_queueinfo" ret: "int" args: "int" "int" "siginfo_t *" */
#define	LINUX_SYS_rt_queueinfo	129

/* syscall: "rt_sigsuspend" ret: "int" args: "linux_sigset_t *" "size_t" */
#define	LINUX_SYS_rt_sigsuspend	130

/* syscall: "sigaltstack" ret: "int" args: "const struct linux_sigaltstack *" "struct linux_sigaltstack *" */
#define	LINUX_SYS_sigaltstack	131

/* syscall: "utime" ret: "int" args: "const char *" "struct linux_utimbuf *" */
#define	LINUX_SYS_utime	132

/* syscall: "mknod" ret: "int" args: "const char *" "int" "int" */
#define	LINUX_SYS_mknod	133

#ifdef EXEC_AOUT
/* syscall: "uselib" ret: "int" args: "const char *" */
#define	LINUX_SYS_uselib	134

#else
#endif
/* syscall: "personality" ret: "int" args: "int" */
#define	LINUX_SYS_personality	135

/* syscall: "statfs" ret: "int" args: "const char *" "struct linux_statfs *" */
#define	LINUX_SYS_statfs	137

/* syscall: "fstatfs" ret: "int" args: "int" "struct linux_statfs *" */
#define	LINUX_SYS_fstatfs	138

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	LINUX_SYS_getpriority	140

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	LINUX_SYS_setpriority	141

/* syscall: "sched_setparam" ret: "int" args: "pid_t" "const struct linux_sched_param *" */
#define	LINUX_SYS_sched_setparam	142

/* syscall: "sched_getparam" ret: "int" args: "pid_t" "struct linux_sched_param *" */
#define	LINUX_SYS_sched_getparam	143

/* syscall: "sched_setscheduler" ret: "int" args: "pid_t" "int" "const struct linux_sched_param *" */
#define	LINUX_SYS_sched_setscheduler	144

/* syscall: "sched_getscheduler" ret: "int" args: "pid_t" */
#define	LINUX_SYS_sched_getscheduler	145

/* syscall: "sched_get_priority_max" ret: "int" args: "int" */
#define	LINUX_SYS_sched_get_priority_max	146

/* syscall: "sched_get_priority_min" ret: "int" args: "int" */
#define	LINUX_SYS_sched_get_priority_min	147

/* syscall: "mlock" ret: "int" args: "void *" "size_t" */
#define	LINUX_SYS_mlock	149

/* syscall: "munlock" ret: "int" args: "void *" "size_t" */
#define	LINUX_SYS_munlock	150

/* syscall: "mlockall" ret: "int" args: "int" */
#define	LINUX_SYS_mlockall	151

/* syscall: "munlockall" ret: "int" args: */
#define	LINUX_SYS_munlockall	152

/* syscall: "modify_ldt" ret: "int" args: "int" "void *" "size_t" */
#define	LINUX_SYS_modify_ldt	154

/* syscall: "__sysctl" ret: "int" args: "struct linux___sysctl *" */
#define	LINUX_SYS___sysctl	156

/* syscall: "arch_prctl" ret: "int" args: "int" "unsigned long" */
#define	LINUX_SYS_arch_prctl	158

/* syscall: "setrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	LINUX_SYS_setrlimit	160

/* syscall: "chroot" ret: "int" args: "char *" */
#define	LINUX_SYS_chroot	161

/* syscall: "sync" ret: "int" args: */
#define	LINUX_SYS_sync	162

/* syscall: "acct" ret: "int" args: "char *" */
#define	LINUX_SYS_acct	163

/* syscall: "settimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	LINUX_SYS_settimeofday	164

/* syscall: "swapon" ret: "int" args: "char *" */
#define	LINUX_SYS_swapon	167

/* syscall: "swapoff" ret: "int" args: "const char *" */
#define	LINUX_SYS_swapoff	168

/* syscall: "reboot" ret: "int" args: "int" "int" "int" "void *" */
#define	LINUX_SYS_reboot	169

/* syscall: "sethostname" ret: "int" args: "char *" "u_int" */
#define	LINUX_SYS_sethostname	170

/* syscall: "setdomainname" ret: "int" args: "char *" "int" */
#define	LINUX_SYS_setdomainname	171

/* syscall: "iopl" ret: "int" args: "int" */
#define	LINUX_SYS_iopl	172

/* syscall: "ioperm" ret: "int" args: "unsigned int" "unsigned int" "int" */
#define	LINUX_SYS_ioperm	173

/* syscall: "gettid" ret: "pid_t" args: */
#define	LINUX_SYS_gettid	186

/* syscall: "setxattr" ret: "int" args: "char *" "char *" "void *" "size_t" "int" */
#define	LINUX_SYS_setxattr	188

/* syscall: "lsetxattr" ret: "int" args: "char *" "char *" "void *" "size_t" "int" */
#define	LINUX_SYS_lsetxattr	189

/* syscall: "fsetxattr" ret: "int" args: "int" "char *" "void *" "size_t" "int" */
#define	LINUX_SYS_fsetxattr	190

/* syscall: "getxattr" ret: "ssize_t" args: "char *" "char *" "void *" "size_t" */
#define	LINUX_SYS_getxattr	191

/* syscall: "lgetxattr" ret: "ssize_t" args: "char *" "char *" "void *" "size_t" */
#define	LINUX_SYS_lgetxattr	192

/* syscall: "fgetxattr" ret: "ssize_t" args: "int" "char *" "void *" "size_t" */
#define	LINUX_SYS_fgetxattr	193

/* syscall: "listxattr" ret: "ssize_t" args: "char *" "char *" "size_t" */
#define	LINUX_SYS_listxattr	194

/* syscall: "llistxattr" ret: "ssize_t" args: "char *" "char *" "size_t" */
#define	LINUX_SYS_llistxattr	195

/* syscall: "flistxattr" ret: "ssize_t" args: "int" "char *" "size_t" */
#define	LINUX_SYS_flistxattr	196

/* syscall: "removexattr" ret: "int" args: "char *" "char *" */
#define	LINUX_SYS_removexattr	197

/* syscall: "lremovexattr" ret: "int" args: "char *" "char *" */
#define	LINUX_SYS_lremovexattr	198

/* syscall: "fremovexattr" ret: "int" args: "int" "char *" */
#define	LINUX_SYS_fremovexattr	199

/* syscall: "tkill" ret: "int" args: "int" "int" */
#define	LINUX_SYS_tkill	200

/* syscall: "time" ret: "int" args: "linux_time_t *" */
#define	LINUX_SYS_time	201

/* syscall: "futex" ret: "int" args: "int *" "int" "int" "const struct timespec *" "int *" "int" */
#define	LINUX_SYS_futex	202

/* syscall: "sched_setaffinity" ret: "int" args: "pid_t" "unsigned int" "unsigned long *" */
#define	LINUX_SYS_sched_setaffinity	203

/* syscall: "sched_getaffinity" ret: "int" args: "pid_t" "unsigned int" "unsigned long *" */
#define	LINUX_SYS_sched_getaffinity	204

/* syscall: "getdents64" ret: "int" args: "int" "struct linux_dirent64 *" "unsigned int" */
#define	LINUX_SYS_getdents64	217

/* syscall: "set_tid_address" ret: "int" args: "int *" */
#define	LINUX_SYS_set_tid_address	218

/* syscall: "clock_settime" ret: "int" args: "clockid_t" "struct linux_timespec *" */
#define	LINUX_SYS_clock_settime	227

/* syscall: "clock_gettime" ret: "int" args: "clockid_t" "struct linux_timespec *" */
#define	LINUX_SYS_clock_gettime	228

/* syscall: "clock_getres" ret: "int" args: "clockid_t" "struct linux_timespec *" */
#define	LINUX_SYS_clock_getres	229

/* syscall: "clock_nanosleep" ret: "int" args: "clockid_t" "int" "struct linux_timespec *" "struct linux_timespec *" */
#define	LINUX_SYS_clock_nanosleep	230

/* syscall: "exit_group" ret: "int" args: "int" */
#define	LINUX_SYS_exit_group	231

/* syscall: "tgkill" ret: "int" args: "int" "int" "int" */
#define	LINUX_SYS_tgkill	234

/* syscall: "nosys" ret: "int" args: */
#define	LINUX_SYS_nosys	237

#define	LINUX_SYS_MAXSYSCALL	238
#define	LINUX_SYS_NSYSENT	512
#endif /* _LINUX_SYS_SYSCALL_H_ */
