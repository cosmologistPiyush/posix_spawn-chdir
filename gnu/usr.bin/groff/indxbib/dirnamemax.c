/* dir_name_max(dir) does the same as pathconf(dir, _PC_NAME_MAX) */

#ifndef lint
static char rcsid[] = "$Id: dirnamemax.c,v 1.3 1993/08/15 19:02:56 mycroft Exp $";
#endif /* not lint */

#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if defined(_POSIX_VERSION) && !defined(__NetBSD__)

long dir_name_max(dir)
     char *dir;
{
  return pathconf(dir, _PC_NAME_MAX);
}

#else /* not _POSIX_VERSION */

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else /* not HAVE_DIRENT_H */
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif /* HAVE_SYS_DIR_H */
#endif /* not HAVE_DIRENT_H */

#ifndef NAME_MAX
#ifdef MAXNAMLEN
#define NAME_MAX MAXNAMLEN
#else /* !MAXNAMLEN */
#ifdef MAXNAMELEN
#define NAME_MAX MAXNAMELEN
#else /* !MAXNAMELEN */
#define NAME_MAX 14
#endif /* !MAXNAMELEN */
#endif /* !MAXNAMLEN */
#endif /* !NAME_MAX */

long dir_name_max(dir)
     char *dir;
{
  return NAME_MAX;
}

#endif /* not _POSIX_VERSION */
