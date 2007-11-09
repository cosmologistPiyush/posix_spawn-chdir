/* $NetBSD: pecoff_syscalls.c,v 1.31 2007/11/09 15:10:33 dsl Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.30 2007/11/09 15:05:34 dsl Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: pecoff_syscalls.c,v 1.31 2007/11/09 15:10:33 dsl Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_ktrace.h"
#include "opt_nfsserver.h"
#include "opt_ntp.h"
#include "opt_compat_netbsd.h"
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "opt_posix.h"
#include "fs_lfs.h"
#include "fs_nfs.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/shm.h>
#include <sys/syscallargs.h>
#include <compat/pecoff/pecoff_syscallargs.h>
#include <compat/sys/shm.h>
#endif /* _KERNEL_OPT */

const char *const pecoff_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"wait4",
	/*   8 */	"#8 (excluded { int sys_creat ( const char * path , mode_t mode ) ; } ocreat)",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"#11 (obsolete execv)",
	/*  12 */	"chdir",
	/*  13 */	"fchdir",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"chown",
	/*  17 */	"break",
#ifdef COMPAT_20
	/*  18 */	"getfsstat",
#else
	/*  18 */	"#18 (excluded compat_20_sys_getfsstat)",
#endif
	/*  19 */	"#19 (excluded { long sys_lseek ( int fd , long offset , int whence ) ; } olseek)",
#ifdef COMPAT_43
	/*  20 */	"getpid",
#else
	/*  20 */	"getpid",
#endif
	/*  21 */	"mount",
	/*  22 */	"unmount",
	/*  23 */	"setuid",
#ifdef COMPAT_43
	/*  24 */	"getuid",
#else
	/*  24 */	"getuid",
#endif
	/*  25 */	"geteuid",
	/*  26 */	"ptrace",
	/*  27 */	"recvmsg",
	/*  28 */	"sendmsg",
	/*  29 */	"recvfrom",
	/*  30 */	"accept",
	/*  31 */	"getpeername",
	/*  32 */	"getsockname",
	/*  33 */	"access",
	/*  34 */	"chflags",
	/*  35 */	"fchflags",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"#38 (excluded { int compat_43_sys_stat ( const char * path , struct stat43 * ub ) ; } stat43)",
	/*  39 */	"getppid",
	/*  40 */	"#40 (excluded { int compat_43_sys_lstat ( const char * path , struct stat43 * ub ) ; } lstat43)",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"getegid",
	/*  44 */	"profil",
#if defined(KTRACE) || !defined(_KERNEL)
	/*  45 */	"ktrace",
#else
	/*  45 */	"#45 (excluded ktrace)",
#endif
	/*  46 */	"#46 (excluded { int sys_sigaction ( int signum , const struct sigaction13 * nsa , struct sigaction13 * osa ) ; } sigaction13)",
#ifdef COMPAT_43
	/*  47 */	"getgid",
#else
	/*  47 */	"getgid",
#endif
	/*  48 */	"#48 (excluded { int sys_sigprocmask ( int how , int mask ) ; } sigprocmask13)",
	/*  49 */	"__getlogin",
	/*  50 */	"__setlogin",
	/*  51 */	"acct",
	/*  52 */	"#52 (excluded { int sys_sigpending ( void ) ; } sigpending13)",
	/*  53 */	"#53 (excluded { int sys_sigaltstack ( const struct sigaltstack13 * nss , struct sigaltstack13 * oss ) ; } sigaltstack13)",
	/*  54 */	"ioctl",
	/*  55 */	"#55 (excluded { int sys_reboot ( int opt ) ; } oreboot)",
	/*  56 */	"revoke",
	/*  57 */	"symlink",
	/*  58 */	"readlink",
	/*  59 */	"execve",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"#62 (excluded { int sys_fstat ( int fd , struct stat43 * sb ) ; } fstat43)",
	/*  63 */	"#63 (excluded { int sys_getkerninfo ( int op , char * where , int * size , int arg ) ; } ogetkerninfo)",
	/*  64 */	"#64 (excluded { int sys_getpagesize ( void ) ; } ogetpagesize)",
	/*  65 */	"#65 (excluded { int sys_msync ( void * addr , size_t len ) ; })",
	/*  66 */	"vfork",
	/*  67 */	"#67 (obsolete vread)",
	/*  68 */	"#68 (obsolete vwrite)",
	/*  69 */	"sbrk",
	/*  70 */	"sstk",
	/*  71 */	"#71 (excluded { int sys_mmap ( void * addr , size_t len , int prot , int flags , int fd , long pos ) ; } ommap)",
	/*  72 */	"vadvise",
	/*  73 */	"munmap",
	/*  74 */	"mprotect",
	/*  75 */	"madvise",
	/*  76 */	"#76 (obsolete vhangup)",
	/*  77 */	"#77 (obsolete vlimit)",
	/*  78 */	"mincore",
	/*  79 */	"getgroups",
	/*  80 */	"setgroups",
	/*  81 */	"getpgrp",
	/*  82 */	"setpgid",
	/*  83 */	"setitimer",
	/*  84 */	"#84 (excluded { int sys_wait ( void ) ; } owait)",
	/*  85 */	"#85 (excluded { int sys_swapon ( const char * name ) ; } oswapon)",
	/*  86 */	"getitimer",
	/*  87 */	"#87 (excluded { int sys_gethostname ( char * hostname , u_int len ) ; } ogethostname)",
	/*  88 */	"#88 (excluded { int sys_sethostname ( char * hostname , u_int len ) ; } osethostname)",
	/*  89 */	"#89 (excluded { int sys_getdtablesize ( void ) ; } ogetdtablesize)",
	/*  90 */	"dup2",
	/*  91 */	"#91 (unimplemented getdopt)",
	/*  92 */	"fcntl",
	/*  93 */	"select",
	/*  94 */	"#94 (unimplemented setdopt)",
	/*  95 */	"fsync",
	/*  96 */	"setpriority",
	/*  97 */	"socket",
	/*  98 */	"connect",
	/*  99 */	"#99 (excluded { int sys_accept ( int s , void * name , int * anamelen ) ; } oaccept)",
	/* 100 */	"getpriority",
	/* 101 */	"#101 (excluded { int sys_send ( int s , void * buf , int len , int flags ) ; } osend)",
	/* 102 */	"#102 (excluded { int sys_recv ( int s , void * buf , int len , int flags ) ; } orecv)",
	/* 103 */	"#103 (excluded { int sys_sigreturn ( struct sigcontext13 * sigcntxp ) ; } sigreturn13)",
	/* 104 */	"bind",
	/* 105 */	"setsockopt",
	/* 106 */	"listen",
	/* 107 */	"#107 (obsolete vtimes)",
	/* 108 */	"#108 (excluded { int sys_sigvec ( int signum , struct sigvec * nsv , struct sigvec * osv ) ; } osigvec)",
	/* 109 */	"#109 (excluded { int sys_sigblock ( int mask ) ; } osigblock)",
	/* 110 */	"#110 (excluded { int sys_sigsetmask ( int mask ) ; } osigsetmask)",
	/* 111 */	"#111 (excluded { int sys_sigsuspend ( int mask ) ; } sigsuspend13)",
	/* 112 */	"#112 (excluded { int sys_sigstack ( struct sigstack * nss , struct sigstack * oss ) ; } osigstack)",
	/* 113 */	"#113 (excluded { int sys_recvmsg ( int s , struct omsghdr * msg , int flags ) ; } orecvmsg)",
	/* 114 */	"#114 (excluded { int sys_sendmsg ( int s , void * msg , int flags ) ; } osendmsg)",
	/* 115 */	"#115 (obsolete vtrace)",
	/* 116 */	"gettimeofday",
	/* 117 */	"getrusage",
	/* 118 */	"getsockopt",
	/* 119 */	"#119 (obsolete resuba)",
	/* 120 */	"readv",
	/* 121 */	"writev",
	/* 122 */	"settimeofday",
	/* 123 */	"fchown",
	/* 124 */	"fchmod",
	/* 125 */	"#125 (excluded { int sys_recvfrom ( int s , void * buf , size_t len , int flags , void * from , int * fromlenaddr ) ; } orecvfrom)",
	/* 126 */	"setreuid",
	/* 127 */	"setregid",
	/* 128 */	"rename",
	/* 129 */	"#129 (excluded { int compat_43_sys_truncate ( const char * path , long length ) ; } otruncate)",
	/* 130 */	"#130 (excluded { int sys_ftruncate ( int fd , long length ) ; } oftruncate)",
	/* 131 */	"flock",
	/* 132 */	"mkfifo",
	/* 133 */	"sendto",
	/* 134 */	"shutdown",
	/* 135 */	"socketpair",
	/* 136 */	"mkdir",
	/* 137 */	"rmdir",
	/* 138 */	"utimes",
	/* 139 */	"#139 (obsolete 4.2 sigreturn)",
	/* 140 */	"adjtime",
	/* 141 */	"#141 (excluded { int sys_getpeername ( int fdes , void * asa , int * alen ) ; } ogetpeername)",
	/* 142 */	"#142 (excluded { int32_t sys_gethostid ( void ) ; } ogethostid)",
	/* 143 */	"#143 (excluded { int sys_sethostid ( int32_t hostid ) ; } osethostid)",
	/* 144 */	"#144 (excluded { int sys_getrlimit ( int which , struct orlimit * rlp ) ; } ogetrlimit)",
	/* 145 */	"#145 (excluded { int sys_setrlimit ( int which , const struct orlimit * rlp ) ; } osetrlimit)",
	/* 146 */	"#146 (excluded { int sys_killpg ( int pgid , int signum ) ; } okillpg)",
	/* 147 */	"setsid",
	/* 148 */	"quotactl",
	/* 149 */	"#149 (excluded { int sys_quota ( void ) ; } oquota)",
	/* 150 */	"#150 (excluded { int sys_getsockname ( int fdec , void * asa , int * alen ) ; } ogetsockname)",
	/* 151 */	"#151 (unimplemented)",
	/* 152 */	"#152 (unimplemented)",
	/* 153 */	"#153 (unimplemented)",
	/* 154 */	"#154 (unimplemented)",
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	/* 155 */	"nfssvc",
#else
	/* 155 */	"#155 (excluded nfssvc)",
#endif
	/* 156 */	"#156 (excluded { int sys_getdirentries ( int fd , char * buf , u_int count , long * basep ) ; } ogetdirentries)",
	/* 157 */	"statfs",
#ifdef COMPAT_20
	/* 158 */	"fstatfs",
#else
	/* 158 */	"#158 (excluded compat_20_sys_fstatfs)",
#endif
	/* 159 */	"#159 (unimplemented)",
	/* 160 */	"#160 (unimplemented)",
#ifdef COMPAT_30
	/* 161 */	"getfh",
#else
	/* 161 */	"#161 (excluded compat_30_sys_getfh)",
#endif
	/* 162 */	"#162 (excluded { int sys_getdomainname ( char * domainname , int len ) ; } ogetdomainname)",
	/* 163 */	"#163 (excluded { int sys_setdomainname ( char * domainname , int len ) ; } osetdomainname)",
	/* 164 */	"#164 (excluded { int sys_uname ( struct outsname * name ) ; } ouname)",
	/* 165 */	"sysarch",
	/* 166 */	"#166 (unimplemented)",
	/* 167 */	"#167 (unimplemented)",
	/* 168 */	"#168 (unimplemented)",
#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(_LP64)
	/* 169 */	"#169 (excluded { int sys_semsys ( int which , int a2 , int a3 , int a4 , int a5 ) ; } osemsys)",
#else
	/* 169 */	"#169 (excluded 1.0 semsys)",
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(_LP64)
	/* 170 */	"#170 (excluded { int sys_msgsys ( int which , int a2 , int a3 , int a4 , int a5 , int a6 ) ; } omsgsys)",
#else
	/* 170 */	"#170 (excluded 1.0 msgsys)",
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(_LP64)
	/* 171 */	"#171 (excluded { int sys_shmsys ( int which , int a2 , int a3 , int a4 ) ; } oshmsys)",
#else
	/* 171 */	"#171 (excluded 1.0 shmsys)",
#endif
	/* 172 */	"#172 (unimplemented)",
	/* 173 */	"pread",
	/* 174 */	"pwrite",
	/* 175 */	"#175 (unimplemented { int sys_ntp_gettime ( struct ntptimeval * ntvp ) ; })",
#if defined(NTP) || !defined(_KERNEL)
	/* 176 */	"ntp_adjtime",
#else
	/* 176 */	"#176 (excluded ntp_adjtime)",
#endif
	/* 177 */	"#177 (unimplemented)",
	/* 178 */	"#178 (unimplemented)",
	/* 179 */	"#179 (unimplemented)",
	/* 180 */	"#180 (unimplemented)",
	/* 181 */	"setgid",
	/* 182 */	"setegid",
	/* 183 */	"seteuid",
#if defined(LFS) || !defined(_KERNEL)
	/* 184 */	"lfs_bmapv",
	/* 185 */	"lfs_markv",
	/* 186 */	"lfs_segclean",
	/* 187 */	"lfs_segwait",
#else
	/* 184 */	"#184 (excluded lfs_bmapv)",
	/* 185 */	"#185 (excluded lfs_markv)",
	/* 186 */	"#186 (excluded lfs_segclean)",
	/* 187 */	"#187 (excluded lfs_segwait)",
#endif
	/* 188 */	"#188 (excluded { int compat_12_sys_stat ( const char * path , struct stat12 * ub ) ; } stat12)",
	/* 189 */	"#189 (excluded { int sys_fstat ( int fd , struct stat12 * sb ) ; } fstat12)",
	/* 190 */	"#190 (excluded { int compat_12_sys_lstat ( const char * path , struct stat12 * ub ) ; } lstat12)",
	/* 191 */	"pathconf",
	/* 192 */	"fpathconf",
	/* 193 */	"#193 (unimplemented)",
	/* 194 */	"getrlimit",
	/* 195 */	"setrlimit",
	/* 196 */	"#196 (excluded { int sys_getdirentries ( int fd , char * buf , u_int count , long * basep ) ; })",
	/* 197 */	"mmap",
	/* 198 */	"__syscall",
	/* 199 */	"lseek",
	/* 200 */	"truncate",
	/* 201 */	"ftruncate",
	/* 202 */	"__sysctl",
	/* 203 */	"mlock",
	/* 204 */	"munlock",
	/* 205 */	"undelete",
	/* 206 */	"futimes",
	/* 207 */	"getpgid",
	/* 208 */	"reboot",
	/* 209 */	"poll",
#if defined(LKM) || !defined(_KERNEL)
	/* 210 */	"lkmnosys",
	/* 211 */	"lkmnosys",
	/* 212 */	"lkmnosys",
	/* 213 */	"lkmnosys",
	/* 214 */	"lkmnosys",
	/* 215 */	"lkmnosys",
	/* 216 */	"lkmnosys",
	/* 217 */	"lkmnosys",
	/* 218 */	"lkmnosys",
	/* 219 */	"lkmnosys",
#else	/* !LKM */
	/* 210 */	"#210 (excluded lkmnosys)",
	/* 211 */	"#211 (excluded lkmnosys)",
	/* 212 */	"#212 (excluded lkmnosys)",
	/* 213 */	"#213 (excluded lkmnosys)",
	/* 214 */	"#214 (excluded lkmnosys)",
	/* 215 */	"#215 (excluded lkmnosys)",
	/* 216 */	"#216 (excluded lkmnosys)",
	/* 217 */	"#217 (excluded lkmnosys)",
	/* 218 */	"#218 (excluded lkmnosys)",
	/* 219 */	"#219 (excluded lkmnosys)",
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
	/* 220 */	"#220 (excluded { int sys___semctl ( int semid , int semnum , int cmd , union __semun * arg ) ; })",
	/* 221 */	"semget",
	/* 222 */	"semop",
	/* 223 */	"semconfig",
#else
	/* 220 */	"#220 (excluded compat_14_semctl)",
	/* 221 */	"#221 (excluded semget)",
	/* 222 */	"#222 (excluded semop)",
	/* 223 */	"#223 (excluded semconfig)",
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	/* 224 */	"#224 (excluded { int sys_msgctl ( int msqid , int cmd , struct msqid_ds14 * buf ) ; })",
	/* 225 */	"msgget",
	/* 226 */	"msgsnd",
	/* 227 */	"msgrcv",
#else
	/* 224 */	"#224 (excluded compat_14_msgctl)",
	/* 225 */	"#225 (excluded msgget)",
	/* 226 */	"#226 (excluded msgsnd)",
	/* 227 */	"#227 (excluded msgrcv)",
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	/* 228 */	"shmat",
	/* 229 */	"#229 (excluded { int sys_shmctl ( int shmid , int cmd , struct shmid_ds14 * buf ) ; })",
	/* 230 */	"shmdt",
	/* 231 */	"shmget",
#else
	/* 228 */	"#228 (excluded shmat)",
	/* 229 */	"#229 (excluded compat_14_shmctl)",
	/* 230 */	"#230 (excluded shmdt)",
	/* 231 */	"#231 (excluded shmget)",
#endif
	/* 232 */	"clock_gettime",
	/* 233 */	"clock_settime",
	/* 234 */	"clock_getres",
	/* 235 */	"timer_create",
	/* 236 */	"timer_delete",
	/* 237 */	"timer_settime",
	/* 238 */	"timer_gettime",
	/* 239 */	"timer_getoverrun",
	/* 240 */	"nanosleep",
	/* 241 */	"fdatasync",
	/* 242 */	"mlockall",
	/* 243 */	"munlockall",
	/* 244 */	"__sigtimedwait",
	/* 245 */	"#245 (unimplemented sys_sigqueue)",
	/* 246 */	"#246 (unimplemented)",
#if defined(P1003_1B_SEMAPHORE) || (!defined(_KERNEL) && defined(_LIBC))
	/* 247 */	"_ksem_init",
	/* 248 */	"_ksem_open",
	/* 249 */	"_ksem_unlink",
	/* 250 */	"_ksem_close",
	/* 251 */	"_ksem_post",
	/* 252 */	"_ksem_wait",
	/* 253 */	"_ksem_trywait",
	/* 254 */	"_ksem_getvalue",
	/* 255 */	"_ksem_destroy",
	/* 256 */	"#256 (unimplemented sys__ksem_timedwait)",
#else
	/* 247 */	"#247 (excluded sys__ksem_init)",
	/* 248 */	"#248 (excluded sys__ksem_open)",
	/* 249 */	"#249 (excluded sys__ksem_unlink)",
	/* 250 */	"#250 (excluded sys__ksem_close)",
	/* 251 */	"#251 (excluded sys__ksem_post)",
	/* 252 */	"#252 (excluded sys__ksem_wait)",
	/* 253 */	"#253 (excluded sys__ksem_trywait)",
	/* 254 */	"#254 (excluded sys__ksem_getvalue)",
	/* 255 */	"#255 (excluded sys__ksem_destroy)",
	/* 256 */	"#256 (unimplemented sys__ksem_timedwait)",
#endif
	/* 257 */	"#257 (unimplemented sys_mq_open)",
	/* 258 */	"#258 (unimplemented sys_mq_close)",
	/* 259 */	"#259 (unimplemented sys_mq_unlink)",
	/* 260 */	"#260 (unimplemented sys_mq_getattr)",
	/* 261 */	"#261 (unimplemented sys_mq_setattr)",
	/* 262 */	"#262 (unimplemented sys_mq_notify)",
	/* 263 */	"#263 (unimplemented sys_mq_send)",
	/* 264 */	"#264 (unimplemented sys_mq_receive)",
	/* 265 */	"#265 (unimplemented sys_mq_timedsend)",
	/* 266 */	"#266 (unimplemented sys_mq_timedreceive)",
	/* 267 */	"#267 (unimplemented)",
	/* 268 */	"#268 (unimplemented)",
	/* 269 */	"#269 (unimplemented)",
	/* 270 */	"__posix_rename",
	/* 271 */	"swapctl",
#ifdef COMPAT_30
	/* 272 */	"getdents",
#endif
	/* 273 */	"minherit",
	/* 274 */	"lchmod",
	/* 275 */	"lchown",
	/* 276 */	"lutimes",
	/* 277 */	"__msync13",
#ifdef COMPAT_30
	/* 278 */	"__stat13",
	/* 279 */	"__fstat13",
	/* 280 */	"__lstat13",
#endif
	/* 281 */	"__sigaltstack14",
	/* 282 */	"__vfork14",
	/* 283 */	"__posix_chown",
	/* 284 */	"__posix_fchown",
	/* 285 */	"__posix_lchown",
	/* 286 */	"getsid",
	/* 287 */	"__clone",
#if defined(KTRACE) || !defined(_KERNEL)
	/* 288 */	"fktrace",
#else
	/* 288 */	"#288 (excluded ktrace)",
#endif
	/* 289 */	"preadv",
	/* 290 */	"pwritev",
#ifdef COMPAT_16
	/* 291 */	"__sigaction14",
#else
	/* 291 */	"#291 (excluded compat_16_sys___sigaction14)",
#endif
	/* 292 */	"__sigpending14",
	/* 293 */	"__sigprocmask14",
	/* 294 */	"__sigsuspend14",
#ifdef COMPAT_16
	/* 295 */	"__sigreturn14",
#else
	/* 295 */	"#295 (excluded compat_16_sys___sigreturn14)",
#endif
	/* 296 */	"__getcwd",
	/* 297 */	"fchroot",
#ifdef COMPAT_30
	/* 298 */	"fhopen",
	/* 299 */	"fhstat",
#else
	/* 298 */	"#298 (excluded compat_30_sys_fhopen)",
	/* 299 */	"#299 (excluded compat_30_sys_fhstat)",
#endif
#ifdef COMPAT_20
	/* 300 */	"fhstatfs",
#else
	/* 300 */	"#300 (excluded compat_20_sys_fhstatfs)",
#endif
#if defined(SYSVSEM) || !defined(_KERNEL)
	/* 301 */	"____semctl13",
#else
	/* 301 */	"#301 (excluded ____semctl13)",
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	/* 302 */	"__msgctl13",
#else
	/* 302 */	"#302 (excluded __msgctl13)",
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	/* 303 */	"__shmctl13",
#else
	/* 303 */	"#303 (excluded __shmctl13)",
#endif
	/* 304 */	"lchflags",
	/* 305 */	"issetugid",
	/* 306 */	"utrace",
	/* 307 */	"getcontext",
	/* 308 */	"setcontext",
	/* 309 */	"_lwp_create",
	/* 310 */	"_lwp_exit",
	/* 311 */	"_lwp_self",
	/* 312 */	"_lwp_wait",
	/* 313 */	"_lwp_suspend",
	/* 314 */	"_lwp_continue",
	/* 315 */	"_lwp_wakeup",
	/* 316 */	"_lwp_getprivate",
	/* 317 */	"_lwp_setprivate",
	/* 318 */	"#318 (unimplemented)",
	/* 319 */	"#319 (unimplemented)",
	/* 320 */	"#320 (unimplemented)",
	/* 321 */	"#321 (unimplemented)",
	/* 322 */	"#322 (unimplemented)",
	/* 323 */	"#323 (unimplemented)",
	/* 324 */	"#324 (unimplemented)",
	/* 325 */	"#325 (unimplemented)",
	/* 326 */	"#326 (unimplemented)",
	/* 327 */	"#327 (unimplemented)",
	/* 328 */	"#328 (unimplemented)",
	/* 329 */	"#329 (unimplemented)",
	/* 330 */	"#330 (obsolete sys_sa_register)",
	/* 331 */	"#331 (obsolete sys_sa_stacks)",
	/* 332 */	"#332 (obsolete sys_sa_enable)",
	/* 333 */	"#333 (obsolete sys_sa_setconcurrency)",
	/* 334 */	"#334 (obsolete sys_sa_yield)",
	/* 335 */	"#335 (obsolete sys_sa_preempt)",
	/* 336 */	"#336 (obsolete sys_sa_unblockyield)",
	/* 337 */	"#337 (unimplemented)",
	/* 338 */	"#338 (unimplemented)",
	/* 339 */	"#339 (unimplemented)",
	/* 340 */	"__sigaction_sigtramp",
	/* 341 */	"pmc_get_info",
	/* 342 */	"pmc_control",
	/* 343 */	"rasctl",
	/* 344 */	"kqueue",
	/* 345 */	"kevent",
	/* 346 */	"#346 (unimplemented sys_sched_setparam)",
	/* 347 */	"#347 (unimplemented sys_sched_getparam)",
	/* 348 */	"#348 (unimplemented sys_sched_setscheduler)",
	/* 349 */	"#349 (unimplemented sys_sched_getscheduler)",
	/* 350 */	"#350 (unimplemented sys_sched_yield)",
	/* 351 */	"#351 (unimplemented sys_sched_get_priority_max)",
	/* 352 */	"#352 (unimplemented sys_sched_get_priority_min)",
	/* 353 */	"#353 (unimplemented sys_sched_rr_get_interval)",
	/* 354 */	"fsync_range",
	/* 355 */	"uuidgen",
	/* 356 */	"getvfsstat",
	/* 357 */	"statvfs1",
	/* 358 */	"fstatvfs1",
#ifdef COMPAT_30
	/* 359 */	"fhstatvfs1",
#else
	/* 359 */	"#359 (excluded compat_30_sys_fhstatvfs1)",
#endif
	/* 360 */	"extattrctl",
	/* 361 */	"extattr_set_file",
	/* 362 */	"extattr_get_file",
	/* 363 */	"extattr_delete_file",
	/* 364 */	"extattr_set_fd",
	/* 365 */	"extattr_get_fd",
	/* 366 */	"extattr_delete_fd",
	/* 367 */	"extattr_set_link",
	/* 368 */	"extattr_get_link",
	/* 369 */	"extattr_delete_link",
	/* 370 */	"extattr_list_fd",
	/* 371 */	"extattr_list_file",
	/* 372 */	"extattr_list_link",
	/* 373 */	"pselect",
	/* 374 */	"pollts",
	/* 375 */	"setxattr",
	/* 376 */	"lsetxattr",
	/* 377 */	"fsetxattr",
	/* 378 */	"getxattr",
	/* 379 */	"lgetxattr",
	/* 380 */	"fgetxattr",
	/* 381 */	"listxattr",
	/* 382 */	"llistxattr",
	/* 383 */	"flistxattr",
	/* 384 */	"removexattr",
	/* 385 */	"lremovexattr",
	/* 386 */	"fremovexattr",
	/* 387 */	"__stat30",
	/* 388 */	"__fstat30",
	/* 389 */	"__lstat30",
	/* 390 */	"__getdents30",
	/* 391 */	"posix_fadvise",
#ifdef COMPAT_30
	/* 392 */	"__fhstat30",
#else
	/* 392 */	"#392 (excluded compat_30_sys___fhstat30)",
#endif
	/* 393 */	"__ntp_gettime30",
	/* 394 */	"__socket30",
	/* 395 */	"__getfh30",
	/* 396 */	"__fhopen40",
	/* 397 */	"__fhstatvfs140",
};
