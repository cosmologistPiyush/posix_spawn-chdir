/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: l64a.c,v 1.7 1998/06/14 17:28:15 kleink Exp $");
#endif

#include "namespace.h"
#include <stdlib.h>

#ifdef __weak_alias
__weak_alias(l64a,_l64a);
#endif

char *
l64a (value)
	long value;
{
	static char buf[8];
	char *s = buf;
	int digit;
	int i;

	if (!value) 
		goto out;

	for (i = 0; value != 0 && i < 6; i++) {
		digit = value & 0x3f;

		if (digit < 2) 
			*s = digit + '.';
		else if (digit < 12)
			*s = digit + '0' - 2;
		else if (digit < 38)
			*s = digit + 'A' - 12;
		else
			*s = digit + 'a' - 38;

		value >>= 6;
		s++;
	}

out:
	*s = '\0';

	return buf;
}
