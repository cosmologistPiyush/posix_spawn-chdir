/*	$NetBSD: compat_defs.h,v 1.2 2002/01/24 04:05:27 lukem Exp $	*/

#ifndef	__NETBSD_COMPAT_DEFS_H__
#define	__NETBSD_COMPAT_DEFS_H__

/* System headers needed for (re)definitions below. */

#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>

#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#if HAVE_STDDEF_H
#include <stddef.h>
#endif
#if !HAVE_VIS_H
#include "compat_vis.h"
#endif

#if !defined(__attribute__) && !defined(__GNUC__)
#define __attribute__(x)
#endif

/* Type substitutes. */

#if !HAVE_ID_T
typedef unsigned long id_t;
#endif

/* Prototypes for replacement functions. */

#if !HAVE_BASENAME
char *basename(char *);
#endif

#if !HAVE_DIRNAME
char *dirname(char *);
#endif

#if !HAVE_ERR_H
void err(int, const char *, ...) __attribute__((noreturn));
void errx(int, const char *, ...) __attribute__((noreturn));
void warn(const char *, ...);
void warnx(const char *, ...);
#endif

#if !HAVE_FGETLN
char *fgetln(FILE *, size_t *);
#endif

#if !HAVE_FLOCK
# define LOCK_SH		0x01
# define LOCK_EX		0x02
# define LOCK_NB		0x04
# define LOCK_UN		0x08
int flock(int, int);
#endif

#if !HAVE_FPARSELN
# define FPARSELN_UNESCESC	0x01
# define FPARSELN_UNESCCONT	0x02
# define FPARSELN_UNESCCOMM	0x04
# define FPARSELN_UNESCREST	0x08
# define FPARSELN_UNESCALL	0x0f
char *fparseln(FILE *, size_t *, size_t *, const char [3], int);
#endif

#if !HAVE_SETPROGNAME
const char *getprogname(void);
void setprogname(const char *);
#endif

#if !HAVE_STRLCAT
size_t strlcat(char *, const char *, size_t);
#endif

#if !HAVE_STRLCPY
size_t strlcpy(char *, const char *, size_t);
#endif

#if !HAVE_PREAD
ssize_t pread(int, void *, size_t, off_t);
#endif

#if !HAVE_PWCACHE_USERDB
const char *user_from_uid(uid_t, int);
int uid_from_user(const char *, uid_t *);
int pwcache_userdb(int (*)(int), void (*)(void),
		struct passwd * (*)(const char *), struct passwd * (*)(uid_t));
const char *group_from_gid(gid_t, int);
int gid_from_group(const char *, gid_t *);
int pwcache_groupdb(int (*)(int), void (*)(void),
		struct group * (*)(const char *), struct group * (*)(gid_t));
#endif

#if !HAVE_PWRITE
ssize_t pwrite(int, const void *, size_t, off_t);
#endif

/*
 * getmode() and setmode() are always defined, as these function names
 * exist but with very different meanings on other OS's.  The compat
 * versions here simply accept an octal mode number; the "u+x,g-w" type
 * of syntax is not accepted.
 */

#define getmode __nbcompat_getmode
#define setmode __nbcompat_setmode

mode_t getmode(const void *, mode_t);
void *setmode(const char *);

/* Eliminate RCS Id strings, assertions embedded in binaries. */

#undef _DIAGASSERT
#define _DIAGASSERT(x) /**/
#define lint

/* Assume an ANSI compiler for the host. */

#undef __P
#define __P(x) x

#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#define BIG_ENDIAN 4321
#define LITTLE_ENDIAN 1234

#undef BYTE_ORDER
#if WORDS_BIGENDIAN
#define BYTE_ORDER BIG_ENDIAN
#else
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#undef MIN
#undef MAX
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/* Some definitions not available on all systems. */

#ifndef _PATH_DEFPATH
#define _PATH_DEFPATH "/usr/bin:/bin:/usr/local/bin"
#endif
#ifndef _PATH_DEV
#define _PATH_DEV "/dev/"
#endif
#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL _PATH_DEV "null"
#endif
#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

#ifndef EFTYPE
#define EFTYPE EIO
#endif

#ifndef MAXBSIZE
#define MAXBSIZE (64 * 1024)
#endif

#ifndef O_EXLOCK
#define O_EXLOCK 0
#endif
#ifndef O_SHLOCK
#define O_SHLOCK 0
#endif

#endif	/* !__NETBSD_COMPAT_DEFS_H__ */
