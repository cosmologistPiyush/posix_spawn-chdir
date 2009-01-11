/* $NetBSD: localeio_lc_time.c,v 1.2 2009/01/11 02:46:28 christos Exp $ */

/*
 * Copyright (c) 2008, The NetBSD Foundation, Inc.
 * All rights reserved.
 * 
 * This code is derived from software contributed to The NetBSD Foundation
 * by Brian Ginsbach.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: localeio_lc_time.c,v 1.2 2009/01/11 02:46:28 christos Exp $");
#endif /* LIBC_SCCS and not lint */

#include "reentrant.h"
#include <sys/types.h>
#include <sys/localedef.h>
#include <sys/queue.h>
#include <assert.h>
#include <errno.h>
#include <langinfo.h>
#include <limits.h>
#define __SETLOCALE_SOURCE__
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setlocale_local.h"

#include "aliasname_local.h"
#include "fix_grouping.h"
#include "localeio.h"

/*
 * macro required by all template headers
 */
#define _PREFIX(name)		__CONCAT(_localeio_LC_TIME_, name)

#include "nb_lc_time_misc.h"
#include "nb_lc_template_decl.h"

#define NSTRINGS (sizeof(_TimeLocale)/sizeof(const char **))

static int
_localeio_LC_TIME_create_impl(const char * __restrict root,
    const char * __restrict name, _TimeLocale ** __restrict pdata)
{
	char path[PATH_MAX + 1];

	_DIAGASSERT(root != NULL);
	_DIAGASSERT(name != NULL);
	_DIAGASSERT(pdata != NULL);

	snprintf(path, sizeof(path),
	    "%s/%s/LC_TIME", root, name);
	return __loadlocale(path, NSTRINGS, 0, sizeof(_TimeLocale),
	    (void *)pdata);
}

#include "nb_lc_template.h"
_LOCALE_CATEGORY_ENTRY(_localeio_LC_TIME_);
