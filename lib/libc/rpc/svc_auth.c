/*	$NetBSD: svc_auth.c,v 1.11 2000/06/02 23:11:16 fvdl Exp $	*/

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
/*
 * Copyright (c) 1986-1991 by Sun Microsystems Inc. 
 */

/* #ident	"@(#)svc_auth.c	1.16	94/04/24 SMI" */

#if 0
#if !defined(lint) && defined(SCCSIDS)
static char sccsid[] = "@(#)svc_auth.c 1.26 89/02/07 Copyr 1984 Sun Micro";
#endif
#endif

/*
 * svc_auth.c, Server-side rpc authenticator interface.
 *
 */

#include "namespace.h"
#include "reentrant.h"
#include <sys/types.h>
#include <rpc/rpc.h>
#include <stdlib.h>

#ifdef __weak_alias
__weak_alias(svc_auth_reg,_svc_auth_reg)
#endif

/*
 * svcauthsw is the bdevsw of server side authentication.
 *
 * Server side authenticators are called from authenticate by
 * using the client auth struct flavor field to index into svcauthsw.
 * The server auth flavors must implement a routine that looks
 * like:
 *
 *	enum auth_stat
 *	flavorx_auth(rqst, msg)
 *		register struct svc_req *rqst;
 *		register struct rpc_msg *msg;
 *
 */

/* declarations to allow servers to specify new authentication flavors */
struct authsvc {
	int	flavor;
	enum	auth_stat (*handler) __P((struct svc_req *, struct rpc_msg *));
	struct	authsvc	  *next;
};
static struct authsvc *Auths = NULL;

/*
 * The call rpc message, msg has been obtained from the wire.  The msg contains
 * the raw form of credentials and verifiers.  authenticate returns AUTH_OK
 * if the msg is successfully authenticated.  If AUTH_OK then the routine also
 * does the following things:
 * set rqst->rq_xprt->verf to the appropriate response verifier;
 * sets rqst->rq_client_cred to the "cooked" form of the credentials.
 *
 * NB: rqst->rq_cxprt->verf must be pre-alloctaed;
 * its length is set appropriately.
 *
 * The caller still owns and is responsible for msg->u.cmb.cred and
 * msg->u.cmb.verf.  The authentication system retains ownership of
 * rqst->rq_client_cred, the cooked credentials.
 *
 * There is an assumption that any flavour less than AUTH_NULL is
 * invalid.
 */
enum auth_stat
_authenticate(rqst, msg)
	register struct svc_req *rqst;
	struct rpc_msg *msg;
{
	register int cred_flavor;
	register struct authsvc *asp;
	enum auth_stat dummy;
#ifdef __REENT
	extern mutex_t authsvc_lock;
#endif

/* VARIABLES PROTECTED BY authsvc_lock: asp, Auths */

	rqst->rq_cred = msg->rm_call.cb_cred;
	rqst->rq_xprt->xp_verf.oa_flavor = _null_auth.oa_flavor;
	rqst->rq_xprt->xp_verf.oa_length = 0;
	cred_flavor = rqst->rq_cred.oa_flavor;
	switch (cred_flavor) {
	case AUTH_NULL:
		dummy = _svcauth_null(rqst, msg);
		return (dummy);
	case AUTH_SYS:
		dummy = _svcauth_unix(rqst, msg);
		return (dummy);
	case AUTH_SHORT:
		dummy = _svcauth_short(rqst, msg);
		return (dummy);
#if 0
	case AUTH_DES:
		dummy = __svcauth_des(rqst, msg);
		return (dummy);
#endif
	default:
		break;
	}

	/* flavor doesn't match any of the builtin types, so try new ones */
	mutex_lock(&authsvc_lock);
	for (asp = Auths; asp; asp = asp->next) {
		if (asp->flavor == cred_flavor) {
			enum auth_stat as;

			as = (*asp->handler)(rqst, msg);
			mutex_unlock(&authsvc_lock);
			return (as);
		}
	}
	mutex_unlock(&authsvc_lock);

	return (AUTH_REJECTEDCRED);
}

/*ARGSUSED*/
enum auth_stat
_svcauth_null(rqst, msg)
	struct svc_req *rqst;
	struct rpc_msg *msg;
{
	return (AUTH_OK);
}

/*
 *  Allow the rpc service to register new authentication types that it is
 *  prepared to handle.  When an authentication flavor is registered,
 *  the flavor is checked against already registered values.  If not
 *  registered, then a new Auths entry is added on the list.
 *
 *  There is no provision to delete a registration once registered.
 *
 *  This routine returns:
 *	 0 if registration successful
 *	 1 if flavor already registered
 *	-1 if can't register (errno set)
 */

int
svc_auth_reg(cred_flavor, handler)
	register int cred_flavor;
	enum auth_stat (*handler) __P((struct svc_req *, struct rpc_msg *));
{
	register struct authsvc *asp;
#ifdef __REENT
	extern mutex_t authsvc_lock;
#endif

	switch (cred_flavor) {
	    case AUTH_NULL:
	    case AUTH_SYS:
	    case AUTH_SHORT:
#if 0
	    case AUTH_DES:
#endif
		/* already registered */
		return (1);

	    default:
		mutex_lock(&authsvc_lock);
		for (asp = Auths; asp; asp = asp->next) {
			if (asp->flavor == cred_flavor) {
				/* already registered */
				mutex_unlock(&authsvc_lock);
				return (1);
			}
		}

		/* this is a new one, so go ahead and register it */
		asp = (struct authsvc *)mem_alloc(sizeof (*asp));
		if (asp == NULL) {
			mutex_unlock(&authsvc_lock);
			return (-1);
		}
		asp->flavor = cred_flavor;
		asp->handler = handler;
		asp->next = Auths;
		Auths = asp;
		mutex_unlock(&authsvc_lock);
		break;
	}
	return (0);
}
