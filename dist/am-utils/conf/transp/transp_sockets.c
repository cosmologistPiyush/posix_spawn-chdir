/*	$NetBSD: transp_sockets.c,v 1.6 2004/11/27 01:24:35 christos Exp $	*/

/*
 * Copyright (c) 1997-2004 Erez Zadok
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
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
 *    must display the following acknowledgment:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
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
 *
 * Id: transp_sockets.c,v 1.28 2004/01/22 05:01:06 ezk Exp
 *
 * Socket specific utilities.
 *      -Erez Zadok <ezk@cs.columbia.edu>
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */
#include <am_defs.h>
#include <amu.h>

#ifndef RPC_MAXDATASIZE
#define RPC_MAXDATASIZE 9000
#endif

/* provide a definition for systems that don't have this */
#ifndef INADDR_LOOPBACK
# define INADDR_LOOPBACK	0x7f000001
#endif /* not INADDR_LOOPBACK */


/*
 * find the IP address that can be used to connect to the local host
 */
void
amu_get_myaddress(struct in_addr *iap, const char *preferred_localhost)
{
  struct hostent *hp;

#ifdef DEBUG_off
#error this code is old and probably not useful any longer.
#error Erez, Jan 21, 2004.
  struct sockaddr_in sin;

  /*
   * Most modern systems should use 127.0.0.1 as the localhost address over
   * which you can do NFS mounts.  In the past we found that some NFS
   * clients may not allow mounts from localhost.  So we used
   * get_myaddress() and that seemed to work.  Alas, on some other systems,
   * get_myaddress() may return one of the interface addresses at random,
   * and thus use a less efficient IP address than 127.0.0.1.  The solution
   * is to hard-code 127.0.0.1, but still check if get_myaddress() returns a
   * different value and warn about it.
   */
  memset((char *) &sin, 0, sizeof(sin));
  get_myaddress(&sin);
  if (sin.sin_addr.s_addr != htonl(INADDR_LOOPBACK))
    dlog("amu_get_myaddress: myaddress conflict (0x%x vs. 0x%lx)",
	 sin.sin_addr.s_addr, (u_long) htonl(INADDR_LOOPBACK));
#endif /* DEBUG_off */

  if (preferred_localhost == NULL)
    goto out;

  /* if specified preferred locahost, then try to use it */
  hp = gethostbyname(preferred_localhost);
  if (hp == NULL) {
    /* XXX: if hstrerror()/h_errno aren't portable, then need to port the next statement */
    plog(XLOG_ERROR, "Unable to resolve localhost_address \"%s\" (%s): using default",
	 preferred_localhost, hstrerror(h_errno));
    goto out;
  }
  if (hp->h_addr_list == NULL) {
    plog(XLOG_ERROR, "localhost_address \"%s\" has no IP addresses: using default",
	 preferred_localhost);
    goto out;
  }
  if (hp->h_addr_list[1] != NULL) {
    plog(XLOG_ERROR, "localhost_address \"%s\" has more than one IP addresses: using first",
	 preferred_localhost);
    goto out;
  }
  memmove((voidp) &iap->s_addr, (voidp) hp->h_addr_list[0], sizeof(iap->s_addr));
  plog(XLOG_INFO, "localhost_address \"%s\" requested", preferred_localhost);
  return;

 out:
  iap->s_addr = htonl(INADDR_LOOPBACK);
}


/*
 * How to bind to reserved ports.
 */
int
bind_resv_port(int so, u_short *pp)
{
  struct sockaddr_in sin;
  int rc;
  u_short port;

  memset((voidp) &sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  port = IPPORT_RESERVED;

  do {
    --port;
    sin.sin_port = htons(port);
    rc = bind(so, (struct sockaddr *) &sin, sizeof(sin));
  } while (rc < 0 && (int) port > IPPORT_RESERVED / 2);

  if (pp && rc == 0)
    *pp = port;

  return rc;
}


/*
 * close a descriptor, Sockets style
 */
int
amu_close(int fd)
{
  return close(fd);
}


/*
 * Create an rpc client attached to the mount daemon.
 */
CLIENT *
get_mount_client(char *unused_host, struct sockaddr_in *sin, struct timeval *tv, int *sock, u_long mnt_version)
{
  CLIENT *client;

  /*
   * First try a TCP socket
   */
  if ((*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) > 0) {
    /*
     * Bind to a privileged port
     */
    if (bind_resv_port(*sock, (u_short *) 0) < 0)
      plog(XLOG_ERROR, "can't bind privileged port (socket)");

    /*
     * Find mountd port to connect to.
     * Connect to mountd.
     * Create a tcp client.
     */
    if ((sin->sin_port = htons(pmap_getport(sin, MOUNTPROG, mnt_version, IPPROTO_TCP))) != 0) {
      if (connect(*sock, (struct sockaddr *) sin, sizeof(*sin)) >= 0
	  && ((client = clnttcp_create(sin, MOUNTPROG, mnt_version, sock, 0, 0)) != NULL))
	return client;
    }
    /*
     * Failed so close socket
     */
    (void) close(*sock);
  }				/* tcp socket opened */
  /* TCP failed so try UDP */
  if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    plog(XLOG_ERROR, "Can't create socket to connect to mountd: %m");
    *sock = RPC_ANYSOCK;
    return NULL;
  }
  /*
   * Bind to a privileged port
   */
  if (bind_resv_port(*sock, (u_short *) 0) < 0)
    plog(XLOG_ERROR, "can't bind privileged port");

  /*
   * Zero out the port - make sure we recompute
   */
  sin->sin_port = 0;

  /*
   * Make a UDP client
   */
  if ((client = clntudp_create(sin, MOUNTPROG, mnt_version, *tv, sock)) == NULL) {
    (void) close(*sock);
    *sock = RPC_ANYSOCK;
    return NULL;
  }
  dlog("get_mount_client: Using udp, port %d", sin->sin_port);
  return client;
}


/*
 * find the address of the caller of an RPC procedure.
 */
struct sockaddr_in *
amu_svc_getcaller(SVCXPRT *xprt)
{
  /* glibc 2.2 returns a sockaddr_storage ??? */
  return (struct sockaddr_in *)svc_getcaller(xprt);
}


/*
 * register an RPC server
 */
int
amu_svc_register(SVCXPRT *xprt, u_long prognum, u_long versnum,
		 void (*dispatch)(struct svc_req *rqstp, SVCXPRT *transp),
		 u_long protocol, struct netconfig *dummy)
{
  return svc_register(xprt, prognum, versnum, dispatch, protocol);
}


/*
 * Create the nfs service for amd
 */
int
create_nfs_service(int *soNFSp, u_short *nfs_portp, SVCXPRT **nfs_xprtp, void (*dispatch_fxn)(struct svc_req *rqstp, SVCXPRT *transp))
{

  *soNFSp = socket(AF_INET, SOCK_DGRAM, 0);

  if (*soNFSp < 0 || bind_resv_port(*soNFSp, NULL) < 0) {
    plog(XLOG_FATAL, "Can't create privileged nfs port (socket)");
    return 1;
  }
  if ((*nfs_xprtp = svcudp_create(*soNFSp)) == NULL) {
    plog(XLOG_FATAL, "cannot create rpc/udp service");
    return 2;
  }
  if ((*nfs_portp = (*nfs_xprtp)->xp_port) >= IPPORT_RESERVED) {
    plog(XLOG_FATAL, "Can't create privileged nfs port");
    return 1;
  }
  if (!svc_register(*nfs_xprtp, NFS_PROGRAM, NFS_VERSION, dispatch_fxn, 0)) {
    plog(XLOG_FATAL, "unable to register (%ld, %ld, 0)",
	 (u_long) NFS_PROGRAM, (u_long) NFS_VERSION);
    return 3;
  }

  return 0;			/* all is well */
}


/*
 * Create the amq service for amd (both TCP and UDP)
 */
int
create_amq_service(int *udp_soAMQp, SVCXPRT **udp_amqpp,
		   struct netconfig **dummy1, int *tcp_soAMQp,
		   SVCXPRT **tcp_amqpp, struct netconfig **dummy2)
{
  int maxrec = RPC_MAXDATASIZE;

  /* first create TCP service */
  if (tcp_soAMQp) {
    *tcp_soAMQp = socket(AF_INET, SOCK_STREAM, 0);
    if (*tcp_soAMQp < 0) {
      plog(XLOG_FATAL, "cannot create tcp socket for amq service: %m");
      return 1;
    }

    /* now create RPC service handle for amq */
    if (tcp_amqpp &&
	(*tcp_amqpp = svctcp_create(*tcp_soAMQp, AMQ_SIZE, AMQ_SIZE)) == NULL) {
      plog(XLOG_FATAL, "cannot create tcp service for amq: soAMQp=%d", *tcp_soAMQp);
      return 2;
    }

#ifdef SVCSET_CONNMAXREC
    /*
     * This is *BSD at its best.
     * They just had to do things differently than everyone else
     * so they fixed a library DoS issue by forcing client-side changes...
     */
# ifndef RPC_MAXDATASIZE
#  define RPC_MAXDATASIZE 9000
# endif /* not RPC_MAXDATASIZE */
    {
      int maxrec = RPC_MAXDATASIZE;
      SVC_CONTROL(*tcp_amqpp, SVCSET_CONNMAXREC, &maxrec);
    }
#endif /* not SVCSET_CONNMAXREC */
  }

  /* next create UDP service */
  if (udp_soAMQp) {
    *udp_soAMQp = socket(AF_INET, SOCK_DGRAM, 0);
    if (*udp_soAMQp < 0) {
      plog(XLOG_FATAL, "cannot create udp socket for amq service: %m");
      return 3;
    }

    /* now create RPC service handle for amq */
    if (udp_amqpp &&
	(*udp_amqpp = svcudp_bufcreate(*udp_soAMQp, AMQ_SIZE, AMQ_SIZE)) == NULL) {
      plog(XLOG_FATAL, "cannot create udp service for amq: soAMQp=%d", *udp_soAMQp);
      return 4;
    }
  }

  return 0;			/* all is well */
}


/*
 * Check if the portmapper is running and reachable
 */
int check_pmap_up(char *host, struct sockaddr_in* sin)
{
  CLIENT *client;
  enum clnt_stat clnt_stat = RPC_TIMEDOUT; /* assume failure */
  int socket = RPC_ANYSOCK;
  struct timeval timeout;

  timeout.tv_sec = 3;
  timeout.tv_usec = 0;
  sin->sin_port = htons(PMAPPORT);
  client = clntudp_create(sin, PMAPPROG, PMAPVERS, timeout, &socket);
  if (client != (CLIENT *) NULL) {
    /* Ping the portmapper on a remote system by calling the nullproc */
    clnt_stat = clnt_call(client,
			  PMAPPROC_NULL,
			  (XDRPROC_T_TYPE) xdr_void,
			  NULL,
			  (XDRPROC_T_TYPE) xdr_void,
			  NULL,
			  timeout);
    clnt_destroy(client);
  }
  close(socket);
  sin->sin_port = 0;

  if (clnt_stat == RPC_TIMEDOUT) {
    plog(XLOG_ERROR, "check_pmap_up: failed to contact portmapper on host \"%s\": %s", host, clnt_sperrno(clnt_stat));
    return 0;
  }
  return 1;
}


/*
 * Find the best NFS version for a host and protocol.
 */
u_long
get_nfs_version(char *host, struct sockaddr_in *sin, u_long nfs_version, const char *proto)
{
  CLIENT *clnt;
  int again = 0;
  enum clnt_stat clnt_stat;
  struct timeval tv;
  int sock;
  char *errstr;

  /*
   * If not set or set wrong, then try from NFS_VERS_MAX on down. If
   * set, then try from nfs_version on down.
   */
  if (nfs_version <= 0 || nfs_version > NFS_VERS_MAX) {
    nfs_version = NFS_VERS_MAX;
    again = 1;
  }
  tv.tv_sec = 3;		/* retry every 3 seconds, but also timeout */
  tv.tv_usec = 0;

#ifdef HAVE_FS_NFS3
try_again:
#endif /* HAVE_FS_NFS3 */

  sock = RPC_ANYSOCK;
  errstr = NULL;
  if (STREQ(proto, "tcp"))
    clnt = clnttcp_create(sin, NFS_PROGRAM, nfs_version, &sock, 0, 0);
  else if (STREQ(proto, "udp"))
    clnt = clntudp_create(sin, NFS_PROGRAM, nfs_version, tv, &sock);
  else
    clnt = NULL;

  if (clnt != NULL) {
    /* Try a couple times to verify the CLIENT handle. */
    tv.tv_sec = 6;
    clnt_stat = clnt_call(clnt,
			  NFSPROC_NULL,
			  (XDRPROC_T_TYPE) xdr_void,
			  0,
			  (XDRPROC_T_TYPE) xdr_void,
			  0,
			  tv);

    if (clnt_stat != RPC_SUCCESS)
      errstr = clnt_sperrno(clnt_stat);

    close(sock);
    clnt_destroy(clnt);
  } else {
#ifdef HAVE_CLNT_SPCREATEERROR
    errstr = clnt_spcreateerror("");
#else /* not HAVE_CLNT_SPCREATEERROR */
    errstr = "";
#endif /* not HAVE_CLNT_SPCREATEERROR */
  }

  if (errstr) {
    plog(XLOG_INFO, "get_nfs_version NFS(%d,%s) failed for %s%s",
 	 (int) nfs_version, proto, host, errstr);
    if (again) {
#ifdef HAVE_FS_NFS3
      if (nfs_version == NFS_VERSION3) {
	nfs_version = NFS_VERSION;
	again = 0;
	plog(XLOG_INFO, "get_nfs_version trying a lower version: NFS(%d,%s)", (int) nfs_version, proto);
      }
      goto try_again;
#endif /* HAVE_FS_NFS3 */
    }
    return 0;
  }

  plog(XLOG_INFO, "get_nfs_version: returning NFS(%d,%s) on host %s",
       (int) nfs_version, proto, host);
  return nfs_version;
}


#if defined(HAVE_FS_AUTOFS) && defined(AUTOFS_PROG)
/*
 * Register the autofs service for amd
 */
int
register_autofs_service(char *autofs_conftype, void (*autofs_dispatch)(struct svc_req *rqstp, SVCXPRT *transp))
{
  int autofs_socket;
  SVCXPRT *autofs_xprt = NULL;

  autofs_socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (autofs_socket < 0 || bind_resv_port(autofs_socket, NULL) < 0) {
    plog(XLOG_FATAL, "Can't create privileged autofs port (socket)");
    return 1;
  }
  if ((autofs_xprt = svcudp_create(autofs_socket)) == NULL) {
    plog(XLOG_FATAL, "Can't create autofs rpc/udp service");
    return 2;
  }
  if (autofs_xprt->xp_port >= IPPORT_RESERVED) {
    plog(XLOG_FATAL, "Can't create privileged autofs port");
    return 1;
  }
  if (!svc_register(autofs_xprt, AUTOFS_PROG, AUTOFS_VERS, autofs_dispatch, 0)) {
    plog(XLOG_FATAL, "unable to register (%ld, %ld, 0)",
	 (u_long) AUTOFS_PROG, (u_long) AUTOFS_VERS);
    return 3;
  }

  return 0;			/* all is well */
}


int
unregister_autofs_service(char *autofs_conftype)
{
  svc_unregister(AUTOFS_PROG, AUTOFS_VERS);
  return 0;
}
#endif /* HAVE_FS_AUTOFS && AUTOFS_PROG */
