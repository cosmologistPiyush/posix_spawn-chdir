/*	$NetBSD: unvis.c,v 1.12 2009/02/10 23:06:31 christos Exp $	*/

/*-
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 */

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1989, 1993\
 The Regents of the University of California.  All rights reserved.");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)unvis.c	8.1 (Berkeley) 6/6/93";
#endif
__RCSID("$NetBSD: unvis.c,v 1.12 2009/02/10 23:06:31 christos Exp $");
#endif /* not lint */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vis.h>

static int eflags;

static void process(FILE *fp, const char *filename);

int
main(int argc, char *argv[])
{
	FILE *fp;
	int ch;

	setprogname(argv[0]);
	while ((ch = getopt(argc, argv, "hm")) != -1)
		switch((char)ch) {
		case 'h':
			eflags |= VIS_HTTPSTYLE;
			break;
		case 'm':
			eflags |= VIS_MIMESTYLE;
			break;
		case '?':
		default:
			(void)fprintf(stderr,
			    "Usage: %s [-h|-m] [file...]\n", getprogname());
			return 1;
		}
	argc -= optind;
	argv += optind;

	if ((eflags & (VIS_HTTPSTYLE|VIS_MIMESTYLE)) ==
	    (VIS_HTTPSTYLE|VIS_MIMESTYLE))
		errx(1, "Can't specify -m and -h at the same time");

	if (*argv)
		while (*argv) {
			if ((fp = fopen(*argv, "r")) != NULL)
				process(fp, *argv);
			else
				warn("%s", *argv);
			argv++;
		}
	else
		process(stdin, "<stdin>");
	return 0;
}

static void
process(FILE *fp, const char *filename)
{
	int offset = 0, c, ret;
	int state = 0;
	char outc;

	while ((c = getc(fp)) != EOF) {
		offset++;
	again:
		switch(ret = unvis(&outc, (char)c, &state, eflags)) {
		case UNVIS_VALID:
			(void)putchar(outc);
			break;
		case UNVIS_VALIDPUSH:
			(void)putchar(outc);
			goto again;
		case UNVIS_SYNBAD:
			warnx("%s: offset: %d: can't decode", filename, offset);
			state = 0;
			break;
		case 0:
		case UNVIS_NOCHAR:
			break;
		default:
			errx(1, "bad return value (%d), can't happen", ret);
			/* NOTREACHED */
		}
	}
	if (unvis(&outc, (char)0, &state, eflags | UNVIS_END) == UNVIS_VALID)
		(void)putchar(outc);
}
