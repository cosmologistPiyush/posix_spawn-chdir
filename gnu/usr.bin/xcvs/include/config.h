/* config.h.  Generated by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Enable AUTH_CLIENT_SUPPORT to enable pserver as a remote access method in
   the CVS client (default) */
#define AUTH_CLIENT_SUPPORT 1

/* Define if you want to use the password authenticated server. */
#define AUTH_SERVER_SUPPORT 1

/* Define if you want CVS to be able to be a remote repository client. */
#define CLIENT_SUPPORT 1

/* Define to 1 if the `closedir' function returns void instead of `int'. */
/* #undef CLOSEDIR_VOID */

/* The CVS admin command is restricted to the members of the group
   CVS_ADMIN_GROUP. If this group does not exist, all users are allowed to run
   CVS admin. To disable the CVS admin command for all users, create an empty
   CVS_ADMIN_GROUP by running configure with the --with-cvs-admin-group=
   option. To disable access control for CVS admin, run configure with the
   --without-cvs-admin-group option in order to comment out the define below.
   */
#define CVS_ADMIN_GROUP "cvsadmin"

/* When committing a permanent change, CVS and RCS make a log entry of who
   committed the change. If you are committing the change logged in as "root"
   (not under "su" or other root-priv giving program), CVS/RCS cannot
   determine who is actually making the change. As such, by default, CVS
   prohibits changes committed by users logged in as "root". You can disable
   checking by passing the "--enable-rootcommit" option to configure or by
   commenting out the lines below. */
#define CVS_BADROOT 1

/* The default editor to use, if one does not specify the "-e" option to cvs,
   or does not have an EDITOR environment variable. If this is not set to an
   absolute path to an executable, use the shell to find where the editor
   actually is. This allows sites with /usr/bin/vi or /usr/ucb/vi to work
   equally well (assuming that their PATH is reasonable). */
#define EDITOR_DFLT "vi"

/* Define to enable encryption support. */
/* #undef ENCRYPTION */

/* Define if this executable will be running on case insensitive file systems.
   In the client case, this means that it will request that the server pretend
   to be case insensitive if it isn't already. */
/* #undef FILENAMES_CASE_INSENSITIVE */

/* When committing or importing files, you must enter a log message. Normally,
   you can do this either via the -m flag on the command line, the -F flag on
   the command line, or an editor will be started for you. If you like to use
   logging templates (the rcsinfo file within the $CVSROOT/CVSROOT directory),
   you might want to force people to use the editor even if they specify a
   message with -m or -F. Enabling FORCE_USE_EDITOR will cause the -m or -F
   message to be appended to the temp file when the editor is started. */
/* #undef FORCE_USE_EDITOR */

/* Define to an alternative value if GSS_C_NT_HOSTBASED_SERVICE isn't defined
   in the gssapi.h header file. MIT Kerberos 1.2.1 requires this. Only
   relevant when using GSSAPI. */
/* #undef GSS_C_NT_HOSTBASED_SERVICE */

/* Define if you have the connect function. */
#define HAVE_CONNECT 1

/* Define if you have the crypt function. */
#define HAVE_CRYPT 1

/* Define to 1 if you have the <direct.h> header file. */
/* #undef HAVE_DIRECT_H */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the `dup2' function. */
#define HAVE_DUP2 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the `fchdir' function. */
#define HAVE_FCHDIR 1

/* Define to 1 if you have the `fchmod' function. */
#define HAVE_FCHMOD 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if your system has a working POSIX `fnmatch' function. */
#define HAVE_FNMATCH 1

/* Define to 1 if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `fsync' function. */
#define HAVE_FSYNC 1

/* Define to 1 if you have the `ftime' function. */
/* #undef HAVE_FTIME */

/* Define to 1 if you have the `ftruncate' function. */
#define HAVE_FTRUNCATE 1

/* Define to 1 if you have the `geteuid' function. */
#define HAVE_GETEUID 1

/* Define to 1 if you have the `getgroups' function. */
#define HAVE_GETGROUPS 1

/* Define to 1 if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getspnam function. */
/* #undef HAVE_GETSPNAM */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have GSSAPI with Kerberos version 5 available. */
/* #undef HAVE_GSSAPI */

/* Define to 1 if you have the <gssapi/gssapi_generic.h> header file. */
/* #undef HAVE_GSSAPI_GSSAPI_GENERIC_H */

/* Define to 1 if you have the <gssapi/gssapi.h> header file. */
#define HAVE_GSSAPI_GSSAPI_H 1

/* Define to 1 if you have the <gssapi.h> header file. */
/* #undef HAVE_GSSAPI_H */

/* Define to 1 if you have the `initgroups' function. */
#define HAVE_INITGROUPS 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define if you have MIT Kerberos version 4 available. */
/* #undef HAVE_KERBEROS */

/* Define to 1 if you have the <krb5.h> header file. */
/* #undef HAVE_KRB5_H */

/* Define to 1 if you have the `krb_get_err_text' function. */
/* #undef HAVE_KRB_GET_ERR_TEXT */

/* Define to 1 if you have the `krb' library (-lkrb). */
/* #undef HAVE_LIBKRB */

/* Define to 1 if you have the `krb4' library (-lkrb4). */
/* #undef HAVE_LIBKRB4 */

/* Define if IPv6 is supported.  */
#define ENABLE_IPV6 1

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `login' function. */
/* #undef HAVE_LOGIN */

/* Define to 1 if you have the `logout' function. */
/* #undef HAVE_LOGOUT */

/* Define to 1 if you support file names longer than 14 characters. */
#define HAVE_LONG_FILE_NAMES 1

/* Define if you have the getaddrinfo function.  */
#define HAVE_GETADDRINFO 1

/* Define if you have the getgroups function.  */
#define HAVE_GETGROUPS 1

/* Define if you have the getnameinfo function.  */
#define HAVE_GETNAMEINFO 1

/* Define if you have memchr (always for CVS). */
#define HAVE_MEMCHR 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkdir' function. */
#define HAVE_MKDIR 1

/* Define to 1 if you have the `mknod' function. */
#define HAVE_MKNOD 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the `mktemp' function. */
#define HAVE_MKTEMP 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have the <ndbm.h> header file. */
#define HAVE_NDBM_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1

/* Define to 1 if you have the `regcomp' function. */
#define HAVE_REGCOMP 1

/* Define to 1 if you have the `regerror' function. */
#define HAVE_REGERROR 1

/* Define to 1 if you have the `regexec' function. */
#define HAVE_REGEXEC 1

/* Define to 1 if you have the `regfree' function. */
#define HAVE_REGFREE 1

/* Define to 1 if you have the `rename' function. */
#define HAVE_RENAME 1

/* Define to 1 if you have the `select' function. */
/* #undef HAVE_SELECT */

/* Define if the diff library should use setmode for binary files. */
/* #undef HAVE_SETMODE */

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `sigblock' function. */
#define HAVE_SIGBLOCK 1

/* Define to 1 if you have the `sigprocmask' function. */
#define HAVE_SIGPROCMASK 1

/* Define to 1 if you have the `sigsetmask' function. */
#define HAVE_SIGSETMASK 1

/* Define to 1 if you have the `sigvec' function. */
#define HAVE_SIGVEC 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have strchr (always for CVS). */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if `st_blksize' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLKSIZE 1

/* Define to 1 if `st_rdev' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_RDEV 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/bsdtypes.h> header file. */
/* #undef HAVE_SYS_BSDTYPES_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the `tempnam' function. */
#define HAVE_TEMPNAM 1

/* Define to 1 if you have the `timezone' function. */
#define HAVE_TIMEZONE 1

/* Define to 1 if you have the `tzset' function. */
#define HAVE_TZSET 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `usleep' function. */
/* #undef HAVE_USLEEP */

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to 1 if `utime(file, NULL)' sets file's timestamp to the present. */
#define HAVE_UTIME_NULL 1

/* Define to 1 if you have the `valloc' function. */
#define HAVE_VALLOC 1

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the `wait3' function. */
#define HAVE_WAIT3 1

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* By default, CVS stores its modules and other such items in flat text files
   (MY_NDBM enables this). Turning off MY_NDBM causes CVS to look for a
   system-supplied ndbm database library and use it instead. That may speed
   things up, but the default setting generally works fine too. */
#define MY_NDBM 1

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "bug-cvs@nongnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Concurrent Versions System (CVS)"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Concurrent Versions System (CVS) 1.11.21"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "cvs"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.11.21"

/* Path to the pr utility */
#define PR_PROGRAM "/usr/bin/pr"

/* Define to force lib/regex.c to use malloc instead of alloca. */
#define REGEX_MALLOC 1

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* If you are working with a large remote repository and a 'cvs checkout' is
   swamping your network and memory, define these to enable flow control. You
   will end up with even less probability of a consistent checkout (see
   Concurrency in cvs.texinfo), but CVS doesn't try to guarantee that anyway.
   The master server process will monitor how far it is getting behind, if it
   reaches the high water mark, it will signal the child process to stop
   generating data when convenient (ie: no locks are held, currently at the
   beginning of a new directory). Once the buffer has drained sufficiently to
   reach the low water mark, it will be signalled to start again. */
#define SERVER_FLOWCONTROL 1

/* The high water mark in bytes for server flow control. Required if
   SERVER_FLOWCONTROL is defined, and useless otherwise. */
#define SERVER_HI_WATER (2 * 1024 * 1024)

/* The low water mark in bytes for server flow control. Required if
   SERVER_FLOWCONTROL is defined, and useless otherwise. */
#define SERVER_LO_WATER (1 * 1024 * 1024)

/* Define if you want CVS to be able to serve repositories to remote clients.
   */
#define SERVER_SUPPORT 1

/* Define as the maximum value of type 'size_t', if the system doesn't define
   it. */
/* #undef SIZE_MAX */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Directory used for storing temporary files, if not overridden by
   environment variables or the -T global option. There should be little need
   to change this (-T is a better mechanism if you need to use a different
   directory for temporary files). */
#define TMPDIR_DFLT "/tmp"

/* The default umask to use when creating or otherwise setting file or
   directory permissions in the repository. Must be a value in the range of 0
   through 0777. For example, a value of 002 allows group rwx access and world
   rx access; a value of 007 allows group rwx access but no world access. This
   value is overridden by the value of the CVSUMASK environment variable,
   which is interpreted as an octal number. */
#define UMASK_DFLT 002

/* Define if setmode is required when writing binary data to stdout. */
/* #undef USE_SETMODE_STDOUT */

/* Define if utime requires write access to the file (true on Windows, but not
   Unix). */
/* #undef UTIME_EXPECTS_WRITABLE */

/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* # undef _ALL_SOURCE */
#endif

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to force lib/regex.c to define re_comp et al. */
#define _REGEX_RE_COMP 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* We want to always use the GNULIB version of getpass which we have in lib,
   so define getpass to something that won't conflict with any existing system
   declarations. */
/* #define getpass cvs_getpass */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */
