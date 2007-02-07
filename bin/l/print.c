/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: print.c,v 1.2 2007/02/07 21:56:08 tls Exp $
 */

#ifndef lint
static char const sccsid[] = "@(#)print.c	8.4 (Berkeley) 4/17/94";
#endif /* not lint */

#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fts.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ls.h"
#include "extern.h"

static int	printaname __P((FTSENT *, u_long, u_long));
static void	printlink __P((FTSENT *));
static void	printtime __P((time_t));
static int	printtype __P((u_int));

#define	IS_NOPRINT(p)	((p)->fts_number == NO_PRINT)

/* Most of these are taken from <sys/stat.h> */
typedef enum Colors {
    C_DIR,     /* directory */
    C_LNK,     /* symbolic link */
    C_SOCK,    /* socket */
    C_FIFO,    /* pipe */
    C_EXEC,    /* executable */
    C_BLK,     /* block special */
    C_CHR,     /* character special */
    C_SUID,    /* setuid executable */
    C_SGID,    /* setgid executable */
    C_WSDIR,   /* directory writeble to others, with sticky bit */
    C_WDIR,    /* directory writeble to others, without sticky bit */
    C_NUMCOLORS        /* just a place-holder */
} Colors ;

char *defcolors = "4x5x2x3x1x464301060203";

static int colors[C_NUMCOLORS][2];

void
printscol(dp)
	DISPLAY *dp;
{
	FTSENT *p;

	for (p = dp->list; p; p = p->fts_link) {
		if (IS_NOPRINT(p))
			continue;
		(void)printaname(p, dp->s_inode, dp->s_block);
		(void)putchar('\n');
	}
}

void
printlong(dp)
	DISPLAY *dp;
{
	struct stat *sp;
	FTSENT *p;
	NAMES *np;
	char buf[20];

	if (dp->list->fts_level != FTS_ROOTLEVEL && (f_longform || f_size))
		(void)printf("total %lu\n", howmany(dp->btotal, blocksize));

	for (p = dp->list; p; p = p->fts_link) {
		if (IS_NOPRINT(p))
			continue;
		sp = p->fts_statp;
		if (f_inode)
			(void)printf("%*lu ", dp->s_inode, (u_long)sp->st_ino);
		if (f_size)
			(void)printf("%*qd ",
			    dp->s_block, howmany(sp->st_blocks, blocksize));
		(void)strmode(sp->st_mode, buf);
		np = p->fts_pointer;
		(void)printf("%s %*u %-*s  %-*s  ", buf, dp->s_nlink,
		    sp->st_nlink, dp->s_user, np->user, dp->s_group,
		    np->group);
		if (f_flags)
			(void)printf("%-*s ", dp->s_flags, np->flags);
		if (S_ISCHR(sp->st_mode) || S_ISBLK(sp->st_mode))
			if (minor(sp->st_rdev) > 255)
				(void)printf("%3d, 0x%08x ",
				    major(sp->st_rdev), minor(sp->st_rdev));
			else
				(void)printf("%3d, %3d ",
				    major(sp->st_rdev), minor(sp->st_rdev));
		else if (dp->bcfile)
			(void)printf("%*s%*qd ",
			    8 - dp->s_size, "", dp->s_size, sp->st_size);
		else
			(void)printf("%*qd ", dp->s_size, sp->st_size);
		if (f_accesstime)
			printtime(sp->st_atime);
		else if (f_statustime)
			printtime(sp->st_ctime);
		else
			printtime(sp->st_mtime);
		if (f_color)
			(void)colortype(sp->st_mode);
		(void)printf("%s", p->fts_name);
		if (f_type)
			(void)printtype(sp->st_mode);
		if (f_color)
			(void)printf("\033[m");
	if (S_ISLNK(sp->st_mode))
			printlink(p);
		(void)putchar('\n');
	}
}

#define	TAB	8

void
printcol(dp)
	DISPLAY *dp;
{
	extern int termwidth;
	static FTSENT **array;
	static int lastentries = -1;
	FTSENT *p;
	int base, chcnt, cnt, col, colwidth, num;
	int endcol, numcols, numrows, row;

	/*
	 * Have to do random access in the linked list -- build a table
	 * of pointers.
	 */
	if (dp->entries > lastentries) {
		lastentries = dp->entries;
		if ((array =
		    realloc(array, dp->entries * sizeof(FTSENT *))) == NULL) {
			warn(NULL);
			printscol(dp);
		}
	}
	for (p = dp->list, num = 0; p; p = p->fts_link)
		if (p->fts_number != NO_PRINT)
			array[num++] = p;

	colwidth = dp->maxlen;
	if (f_inode)
		colwidth += dp->s_inode + 1;
	if (f_size)
		colwidth += dp->s_block + 1;
	if (f_type)
		colwidth += 1;

	colwidth = (colwidth + TAB) & ~(TAB - 1);
	if (termwidth < 2 * colwidth) {
		printscol(dp);
		return;
	}

	numcols = termwidth / colwidth;
	numrows = num / numcols;
	if (num % numcols)
		++numrows;

	if (dp->list->fts_level != FTS_ROOTLEVEL && (f_longform || f_size))
		(void)printf("total %lu\n", howmany(dp->btotal, blocksize));
	for (row = 0; row < numrows; ++row) {
		endcol = colwidth;
		for (base = row, chcnt = col = 0; col < numcols; ++col) {
			chcnt += printaname(array[base], dp->s_inode,
			    dp->s_block);
			if ((base += numrows) >= num)
				break;

			/*
			 * some terminals get confused if we mix tabs
			 * with color sequences
			 */
			if (f_color)
				while ((cnt = (chcnt + 1)) <= endcol) {
					(void)putchar(' ');
					chcnt = cnt;
				}
			else
				while ((cnt = ((chcnt + TAB) & ~(TAB - 1)))
				       <= endcol) {
					(void)putchar('\t');
					chcnt = cnt;
				}
			endcol += colwidth;
		}
		(void)putchar('\n');
	}
}

/*
 * print [inode] [size] name
 * return # of characters printed, no trailing characters.
 */
static int
printaname(p, inodefield, sizefield)
	FTSENT *p;
	u_long sizefield, inodefield;
{
	struct stat *sp;
	int chcnt;

	sp = p->fts_statp;
	chcnt = 0;
	if (f_inode)
		chcnt += printf("%*lu ", (int)inodefield, (u_long)sp->st_ino);
	if (f_size)
		chcnt += printf("%*qd ",
		    (int)sizefield, howmany(sp->st_blocks, blocksize));
	if (f_color)
		(void)colortype(sp->st_mode);
	chcnt += printf("%s", p->fts_name);
	if (f_type)
		chcnt += printtype(sp->st_mode);
	if (f_color)
		printf("\033[m");
	return (chcnt);
}

static void
printtime(ftime)
	time_t ftime;
{
	int i;
	char longstring[80];

	strftime(longstring, sizeof(longstring), "%c", localtime(&ftime));
	for (i = 4; i < 11; ++i)
		(void)putchar(longstring[i]);

#define	SIXMONTHS	((365 / 2) * 86400)
	if (f_sectime)
		for (i = 11; i < 24; i++)
			(void)putchar(longstring[i]);
	else if (ftime + SIXMONTHS > time(NULL))
		for (i = 11; i < 16; ++i)
			(void)putchar(longstring[i]);
	else {
		(void)putchar(' ');
		for (i = 20; i < 24; ++i)
			(void)putchar(longstring[i]);
	}
	(void)putchar(' ');
}

static int
printtype(mode)
	u_int mode;
{
	switch (mode & S_IFMT) {
	case S_IFDIR:
		(void)putchar('/');
		return (1);
	case S_IFIFO:
		(void)putchar('|');
		return (1);
	case S_IFLNK:
		(void)putchar('@');
		return (1);
	case S_IFSOCK:
		(void)putchar('=');
		return (1);
#ifndef BSD4_4_LITE
	case S_IFWHT:
		(void)putchar('%');
		return (1);
#endif
	}
	if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
		(void)putchar('*');
		return (1);
	}
	return (0);
}

void
printcolor(c)
       Colors c;
{
	printf("\033[");
	if (colors[c][0] != -1) {
		printf("3%d", colors[c][0]);
		if (colors[c][1] != -1)
		    printf(";");
	}
	if (colors[c][1] != -1)
	    printf("4%d", colors[c][1]);
	printf("m");
}

colortype(mode)
       mode_t mode;
{
	switch(mode & S_IFMT) {
	      case S_IFDIR:
		if (mode & S_IWOTH)
		    if (mode & S_ISTXT)
			printcolor(C_WSDIR);
		    else
			printcolor(C_WDIR);
		else
		    printcolor(C_DIR);
		return(1);
	      case S_IFLNK:
		printcolor(C_LNK);
		return(1);
	      case S_IFSOCK:
		printcolor(C_SOCK);
		return(1);
	      case S_IFIFO:
		printcolor(C_FIFO);
		return(1);
	      case S_IFBLK:
		printcolor(C_BLK);
		return(1);
	      case S_IFCHR:
		printcolor(C_CHR);
		return(1);
	}
	if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
		if (mode & S_ISUID)
		    printcolor(C_SUID);
		else if (mode & S_ISGID)
		    printcolor(C_SGID);
		else
		    printcolor(C_EXEC);
		return(1);
	}
	return(0);
}

void
parsecolors(cs)
char *cs;
{
	int i, j, len;
	char c[2];
	if (cs == NULL)    cs = ""; /* LSCOLORS not set */
	len = strlen(cs);
	for (i = 0 ; i < C_NUMCOLORS ; i++) {
		if (len <= 2*i) {
			c[0] = defcolors[2*i];
			c[1] = defcolors[2*i+1];
		}
		else {
			c[0] = cs[2*i];
			c[1] = cs[2*i+1];
		}
		for (j = 0 ; j < 2 ; j++) {
			if ((c[j] < '0' || c[j] > '7') &&
			    tolower(c[j]) != 'x') {
				fprintf(stderr,
					"error: invalid character '%c' in LSCOLORS env var\n",
					c[j]);
				c[j] = defcolors[2*i+j];
			}
			if (c[j] == 'x')
			    colors[i][j] = -1;
			else
			    colors[i][j] = c[j]-'0';
		}
	}
}
 
static void
printlink(p)
	FTSENT *p;
{
	int lnklen;
	char name[MAXPATHLEN + 1], path[MAXPATHLEN + 1];

	if (p->fts_level == FTS_ROOTLEVEL)
		(void)snprintf(name, sizeof(name), "%s", p->fts_name);
	else
		(void)snprintf(name, sizeof(name),
		    "%s/%s", p->fts_parent->fts_accpath, p->fts_name);
	if ((lnklen = readlink(name, path, sizeof(path) - 1)) == -1) {
		(void)fprintf(stderr, "\nls: %s: %s\n", name, strerror(errno));
		return;
	}
	path[lnklen] = '\0';
	(void)printf(" -> %s", path);
}
