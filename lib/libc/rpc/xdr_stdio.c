/*	$NetBSD: xdr_stdio.c,v 1.7 1998/02/11 11:53:01 lukem Exp $	*/

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char *sccsid = "@(#)xdr_stdio.c 1.16 87/08/11 Copyr 1984 Sun Micro";
static char *sccsid = "@(#)xdr_stdio.c	2.1 88/07/29 4.0 RPCSRC";
#else
__RCSID("$NetBSD: xdr_stdio.c,v 1.7 1998/02/11 11:53:01 lukem Exp $");
#endif
#endif

/*
 * xdr_stdio.c, XDR implementation on standard i/o file.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * This set of routines implements a XDR on a stdio stream.
 * XDR_ENCODE serializes onto the stream, XDR_DECODE de-serializes
 * from the stream.
 */

#include "namespace.h"

#include <stdio.h>

#include <rpc/types.h>
#include <rpc/xdr.h>

#ifdef __weak_alias
__weak_alias(xdrstdio_create,_xdrstdio_create);
#endif

static void xdrstdio_destroy __P((XDR *));
static bool_t xdrstdio_getlong __P((XDR *, int32_t *));
static bool_t xdrstdio_putlong __P((XDR *, int32_t *));
static bool_t xdrstdio_getbytes __P((XDR *, caddr_t, u_int32_t));
static bool_t xdrstdio_putbytes __P((XDR *, caddr_t, u_int32_t));
static u_int32_t xdrstdio_getpos __P((XDR *));
static bool_t xdrstdio_setpos __P((XDR *, u_int32_t));
static int32_t *xdrstdio_inline __P((XDR *, u_int32_t));

/*
 * Ops vector for stdio type XDR
 */
static struct xdr_ops	xdrstdio_ops = {
	xdrstdio_getlong,	/* deseraialize a long int */
	xdrstdio_putlong,	/* seraialize a long int */
	xdrstdio_getbytes,	/* deserialize counted bytes */
	xdrstdio_putbytes,	/* serialize counted bytes */
	xdrstdio_getpos,	/* get offset in the stream */
	xdrstdio_setpos,	/* set offset in the stream */
	xdrstdio_inline,	/* prime stream for inline macros */
	xdrstdio_destroy	/* destroy stream */
};

/*
 * Initialize a stdio xdr stream.
 * Sets the xdr stream handle xdrs for use on the stream file.
 * Operation flag is set to op.
 */
void
xdrstdio_create(xdrs, file, op)
	XDR *xdrs;
	FILE *file;
	enum xdr_op op;
{

	xdrs->x_op = op;
	xdrs->x_ops = &xdrstdio_ops;
	xdrs->x_private = (void *)file;
	xdrs->x_handy = 0;
	xdrs->x_base = 0;
}

/*
 * Destroy a stdio xdr stream.
 * Cleans up the xdr stream handle xdrs previously set up by xdrstdio_create.
 */
static void
xdrstdio_destroy(xdrs)
	XDR *xdrs;
{
	(void)fflush((FILE *)xdrs->x_private);
	/* xx should we close the file ?? */
};

static bool_t
xdrstdio_getlong(xdrs, lp)
	XDR *xdrs;
	int32_t *lp;
{

	if (fread((void *)lp, sizeof(int32_t), 1,
	    (FILE *)xdrs->x_private) != 1)
		return (FALSE);
	*lp = (int32_t)ntohl((int32_t)*lp);
	return (TRUE);
}

static bool_t
xdrstdio_putlong(xdrs, lp)
	XDR *xdrs;
	int32_t *lp;
{
	int32_t mycopy = (int32_t)htonl((int32_t)*lp);

	if (fwrite((void *)&mycopy, sizeof(int32_t), 1,
	    (FILE *)xdrs->x_private) != 1)
		return (FALSE);
	return (TRUE);
}

static bool_t
xdrstdio_getbytes(xdrs, addr, len)
	XDR *xdrs;
	caddr_t addr;
	u_int32_t len;
{

	if ((len != 0) &&
	    (fread(addr, (int)len, 1, (FILE *)xdrs->x_private) != 1))
		return (FALSE);
	return (TRUE);
}

static bool_t
xdrstdio_putbytes(xdrs, addr, len)
	XDR *xdrs;
	caddr_t addr;
	u_int32_t len;
{

	if ((len != 0) && (fwrite(addr, (int)len, 1, (FILE *)xdrs->x_private) != 1))
		return (FALSE);
	return (TRUE);
}

static u_int32_t
xdrstdio_getpos(xdrs)
	XDR *xdrs;
{

	return ((u_int32_t) ftell((FILE *)xdrs->x_private));
}

static bool_t
xdrstdio_setpos(xdrs, pos) 
	XDR *xdrs;
	u_int32_t pos;
{ 

	return ((fseek((FILE *)xdrs->x_private, (int32_t)pos, 0) < 0) ?
		FALSE : TRUE);
}

static int32_t *
xdrstdio_inline(xdrs, len)
	XDR *xdrs;
	u_int32_t len;
{

	/*
	 * Must do some work to implement this: must insure
	 * enough data in the underlying stdio buffer,
	 * that the buffer is aligned so that we can indirect through a
	 * int32_t *, and stuff this pointer in xdrs->x_buf.  Doing
	 * a fread or fwrite to a scratch buffer would defeat
	 * most of the gains to be had here and require storage
	 * management on this buffer, so we don't do this.
	 */
	return (NULL);
}
