/*	$NetBSD: amrandom.h,v 1.1.1.1 2001/05/17 20:47:09 itojun Exp $	*/

/* Copyright (C) RSA Data Security, Inc. created 1994, 1996.  This is an
   unpublished work protected as such under copyright law.  This work
   contains proprietary, confidential, and trade secret information of
   RSA Data Security, Inc.  Use, disclosure or reproduction without the
   express written authorization of RSA Data Security, Inc. is
   prohibited.
 */

typedef struct {
  int (*Query) PROTO_LIST ((unsigned int *, POINTER));
  int (*Init) PROTO_LIST ((POINTER, POINTER, A_SURRENDER_CTX *));
  int (*Update) PROTO_LIST
    ((POINTER, unsigned char *, unsigned int, A_SURRENDER_CTX *));
  int (*Generate) PROTO_LIST
    ((POINTER, unsigned char *, unsigned int, A_SURRENDER_CTX *));
} A_RANDOM_ALGA;

