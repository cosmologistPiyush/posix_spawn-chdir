/*	$NetBSD: opt.c,v 1.1.1.4 1997/05/27 22:16:47 thorpej Exp $	*/

/*
 * (C)opyright 1993,1994,1995 by Darren Reed.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and due credit is given
 * to the original author and the contributors.
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <net/if.h>
#include <netinet/ip_compat.h>
#include "ipf.h"

#if !defined(lint) && defined(LIBC_SCCS)
static	char	sccsid[] = "@(#)opt.c	1.8 4/10/96 (C) 1993-1995 Darren Reed";
static	char	rcsid[] = "Id: opt.c,v 2.0.2.3 1997/03/10 08:10:40 darrenr Exp ";
#endif

extern	int	opts;

struct	ipopt_names	ionames[] ={
	{ IPOPT_NOP,	0x000001,	1,	"nop" },
	{ IPOPT_RR,	0x000002,	7,	"rr" },		/* 1 route */
	{ IPOPT_ZSU,	0x000004,	3,	"zsu" },
	{ IPOPT_MTUP,	0x000008,	3,	"mtup" },
	{ IPOPT_MTUR,	0x000010,	3,	"mtur" },
	{ IPOPT_ENCODE,	0x000020,	3,	"encode" },
	{ IPOPT_TS,	0x000040,	8,	"ts" },		/* 1 TS */
	{ IPOPT_TR,	0x000080,	3,	"tr" },
	{ IPOPT_SECURITY,0x000100,	11,	"sec" },
	{ IPOPT_SECURITY,0x000100,	11,	"sec-class" },
	{ IPOPT_LSRR,	0x000200,	7,	"lsrr" },	/* 1 route */
	{ IPOPT_E_SEC,	0x000400,	3,	"e-sec" },
	{ IPOPT_CIPSO,	0x000800,	3,	"cipso" },
	{ IPOPT_SATID,	0x001000,	4,	"satid" },
	{ IPOPT_SSRR,	0x002000,	7,	"ssrr" },	/* 1 route */
	{ IPOPT_ADDEXT,	0x004000,	3,	"addext" },
	{ IPOPT_VISA,	0x008000,	3,	"visa" },
	{ IPOPT_IMITD,	0x010000,	3,	"imitd" },
	{ IPOPT_EIP,	0x020000,	3,	"eip" },
	{ IPOPT_FINN,	0x040000,	3,	"finn" },
	{ 0, 		0,	0,	(char *)NULL }     /* must be last */
};

struct	ipopt_names	secclass[] = {
	{ IPSO_CLASS_RES4,	0x01,	0, "reserv-4" },
	{ IPSO_CLASS_TOPS,	0x02,	0, "topsecret" },
	{ IPSO_CLASS_SECR,	0x04,	0, "secret" },
	{ IPSO_CLASS_RES3,	0x08,	0, "reserv-3" },
	{ IPSO_CLASS_CONF,	0x10,	0, "confid" },
	{ IPSO_CLASS_UNCL,	0x20,	0, "unclass" },
	{ IPSO_CLASS_RES2,	0x40,	0, "reserv-2" },
	{ IPSO_CLASS_RES1,	0x80,	0, "reserv-1" },
	{ 0, 0, 0, NULL }	/* must be last */
};

static	u_char	seclevel __P((char *));

static u_char seclevel(slevel)
char *slevel;
{
	struct ipopt_names *so;

	for (so = secclass; so->on_name; so++)
		if (!strcasecmp(slevel, so->on_name))
			break;

	if (!so->on_name) {
		fprintf(stderr, "no such security level: %s\n", slevel);
		return 0;
	}
	return (u_char)so->on_value;
}


u_long buildopts(cp, op)
char *cp, *op;
{
	struct ipopt_names *io;
	u_char lvl;
	u_long msk = 0;
	char *s, *t;
	int len = 0;

	for (s = strtok(cp, ","); s; s = strtok(NULL, ",")) {
		if ((t = strchr(s, '=')))
			*t++ = '\0';
		for (io = ionames; io->on_name; io++) {
			if (strcasecmp(s, io->on_name) || (msk & io->on_bit))
				continue;
			if ((len + io->on_siz) > 48) {
				fprintf(stderr, "options too long\n");
				return 0;
			}
			len += io->on_siz;
			*op++ = io->on_value;
			if (io->on_siz > 1) {
				*op++ = io->on_siz;
				*op++ = IPOPT_MINOFF;

				if (t && !strcasecmp(s, "sec-class")) {
					lvl = seclevel(t);
					*(op - 1) = lvl;
				}
				op += io->on_siz - 3;
				if (len & 3) {
					*op++ = IPOPT_NOP;
					len++;
				}
			}
			if (opts & OPT_DEBUG)
				fprintf(stderr, "bo: %s %d %#x: %d\n",
					io->on_name, io->on_value,
					io->on_bit, len);
			msk |= io->on_bit;
			break;
		}
		if (!io->on_name) {
			fprintf(stderr, "unknown IP option name %s\n", s);
			return 0;
		}
	}
	*op++ = IPOPT_EOL;
	len++;
	return len;
}
