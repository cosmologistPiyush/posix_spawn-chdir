/*	$NetBSD: is_tar.c,v 1.1.1.1 2003/03/25 22:30:18 pooka Exp $	*/

/*
 * Copyright (c) Ian F. Darwin 1986-1995.
 * Software written by Ian F. Darwin and others;
 * maintained 1995-present by Christos Zoulas and others.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice immediately at the beginning of the file, without modification,
 *    this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Ian F. Darwin and others.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * is_tar() -- figure out whether file is a tar archive.
 *
 * Stolen (by the author!) from the public domain tar program:
 * Public Domain version written 26 Aug 1985 John Gilmore (ihnp4!hoptoad!gnu).
 *
 * @(#)list.c 1.18 9/23/86 Public Domain - gnu
 *
 * Comments changed and some code/comments reformatted
 * for file command by Ian Darwin.
 */

#include "file.h"
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "tar.h"

#ifndef lint
#if 0
FILE_RCSID("@(#)Id: is_tar.c,v 1.19 2003/03/23 21:16:26 christos Exp")
#else
__RCSID("$NetBSD: is_tar.c,v 1.1.1.1 2003/03/25 22:30:18 pooka Exp $");
#endif
#endif

#define	isodigit(c)	( ((c) >= '0') && ((c) <= '7') )

private int from_oct(int, char *);	/* Decode octal number */

/*
 * Return 
 *	0 if the checksum is bad (i.e., probably not a tar archive), 
 *	1 for old UNIX tar file,
 *	2 for Unix Std (POSIX) tar file.
 */
protected int
file_is_tar(const unsigned char *buf, size_t nbytes)
{
	union record *header = (union record *)buf;
	int	i;
	int	sum, recsum;
	char	*p;

	if (nbytes < sizeof(union record))
		return 0;

	recsum = from_oct(8,  header->header.chksum);

	sum = 0;
	p = header->charptr;
	for (i = sizeof(union record); --i >= 0;) {
		/*
		 * We can't use unsigned char here because of old compilers,
		 * e.g. V7.
		 */
		sum += 0xFF & *p++;
	}

	/* Adjust checksum to count the "chksum" field as blanks. */
	for (i = sizeof(header->header.chksum); --i >= 0;)
		sum -= 0xFF & header->header.chksum[i];
	sum += ' '* sizeof header->header.chksum;	

	if (sum != recsum)
		return 0;	/* Not a tar archive */
	
	if (0==strcmp(header->header.magic, TMAGIC)) 
		return 2;		/* Unix Standard tar archive */

	return 1;			/* Old fashioned tar archive */
}


/*
 * Quick and dirty octal conversion.
 *
 * Result is -1 if the field is invalid (all blank, or nonoctal).
 */
private int
from_oct(int digs, char *where)
{
	int	value;

	while (isspace((unsigned char)*where)) {	/* Skip spaces */
		where++;
		if (--digs <= 0)
			return -1;		/* All blank field */
	}
	value = 0;
	while (digs > 0 && isodigit(*where)) {	/* Scan til nonoctal */
		value = (value << 3) | (*where++ - '0');
		--digs;
	}

	if (digs > 0 && *where && !isspace((unsigned char)*where))
		return -1;			/* Ended on non-space/nul */

	return value;
}
