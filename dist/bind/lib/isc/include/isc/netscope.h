/*	$NetBSD: netscope.h,v 1.1.1.1 2004/05/17 23:45:04 christos Exp $	*/

/*
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2002  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: netscope.h,v 1.4.142.5 2004/03/08 09:04:52 marka Exp */

#ifndef ISC_NETSCOPE_H
#define ISC_NETSCOPE_H 1

ISC_LANG_BEGINDECLS

/*
 * Convert a string of an IPv6 scope zone to zone index.  If the conversion
 * succeeds, 'zoneid' will store the index value.
 * XXXJT: when a standard interface for this purpose is defined,
 * we should use it.
 *
 * Returns:
 *	ISC_R_SUCCESS: conversion succeeds
 *	ISC_R_FAILURE: conversion fails
 */
isc_result_t
isc_netscope_pton(int af, char *scopename, void *addr, isc_uint32_t *zoneid);

ISC_LANG_ENDDECLS

#endif /* ISC_NETADDR_H */
