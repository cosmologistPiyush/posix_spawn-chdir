/*	$NetBSD: vers.c,v 1.1.1.1 1998/10/05 18:01:59 tron Exp $	*/

#ifndef lint
static char Version[] = "@(#)vers.c	e07@nikhef.nl (Eric Wassenaar) 961113";
#endif

char *version = "961113";

#if defined(apollo)
int h_errno = 0;
#endif
