/*	$NetBSD: base64.h,v 1.1.1.1 2004/05/17 23:45:03 christos Exp $	*/

/*
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
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

/* Id: base64.h,v 1.15.206.1 2004/03/06 08:14:38 marka Exp */

#ifndef ISC_BASE64_H
#define ISC_BASE64_H 1

#include <isc/lang.h>
#include <isc/types.h>

ISC_LANG_BEGINDECLS

/***
 *** Functions
 ***/

isc_result_t
isc_base64_totext(isc_region_t *source, int wordlength,
		  const char *wordbreak, isc_buffer_t *target);
/*
 * Convert data into base64 encoded text.
 *
 * Notes:
 *	The base64 encoded text in 'target' will be divided into
 *	words of at most 'wordlength' characters, separated by
 * 	the 'wordbreak' string.  No parentheses will surround
 *	the text.
 *
 * Requires:
 *	'source' is a region containing binary data
 *	'target' is a text buffer containing available space
 *	'wordbreak' points to a null-terminated string of
 *		zero or more whitespace characters
 *
 * Ensures:
 *	target will contain the base64 encoded version of the data
 *	in source.  The 'used' pointer in target will be advanced as
 *	necessary.
 */

isc_result_t
isc_base64_decodestring(const char *cstr, isc_buffer_t *target);
/*
 * Decode a null-terminated base64 string.
 *
 * Requires:
 *	'cstr' is non-null.
 *	'target' is a valid buffer.
 *
 * Returns:
 *	ISC_R_SUCCESS	-- the entire decoded representation of 'cstring'
 *			   fit in 'target'.
 *	ISC_R_BADBASE64 -- 'cstr' is not a valid base64 encoding.
 *
 * 	Other error returns are any possible error code from:
 *		isc_lex_create(),
 *		isc_lex_openbuffer(),
 *		isc_base64_tobuffer().
 */

isc_result_t
isc_base64_tobuffer(isc_lex_t *lexer, isc_buffer_t *target, int length);
/*
 * Convert base64 encoded text from a lexer context into data.
 *
 * Requires:
 *	'lex' is a valid lexer context
 *	'target' is a buffer containing binary data
 *	'length' is an integer
 *
 * Ensures:
 *	target will contain the data represented by the base64 encoded
 *	string parsed by the lexer.  No more than length bytes will be read,
 *	if length is positive.  The 'used' pointer in target will be
 *	advanced as necessary.
 */



ISC_LANG_ENDDECLS

#endif /* ISC_BASE64_H */
