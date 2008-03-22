/*
 * Copyright (c) 2005 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of KTH nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY KTH AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL KTH OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "krb5_locl.h"
#include <err.h>
#include <getarg.h>

__RCSID("$Heimdal: test_pkinit_dh2key.c 18809 2006-10-22 07:11:43Z lha $"
        "$NetBSD: test_pkinit_dh2key.c,v 1.1 2008/03/22 08:37:15 mlelstv Exp $");

static void
test_dh2key(int i,
	    krb5_context context, 
	    const heim_octet_string *dh,
	    const heim_octet_string *c_n,
	    const heim_octet_string *k_n,
	    krb5_enctype etype,
	    const heim_octet_string *result)
{
    krb5_error_code ret;
    krb5_keyblock key;

    ret = _krb5_pk_octetstring2key(context,
				   etype,
				   dh->data, dh->length,
				   c_n,
				   k_n,
				   &key);
    if (ret != 0)
	krb5_err(context, 1, ret, "_krb5_pk_octetstring2key: %d", i);

    if (key.keyvalue.length != result->length ||
	memcmp(key.keyvalue.data, result->data, result->length) != 0)
	krb5_errx(context, 1, "resulting key wrong: %d", i);

    krb5_free_keyblock_contents(context, &key);
}


struct {
    krb5_enctype type;
    krb5_data X;
    krb5_data key;
} tests[] = {
    /* 0 */
    {
	ETYPE_AES256_CTS_HMAC_SHA1_96,
	{
	    256,
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	},
	{
	    32,
	    "\x5e\xe5\x0d\x67\x5c\x80\x9f\xe5\x9e\x4a\x77\x62\xc5\x4b\x65\x83"
	    "\x75\x47\xea\xfb\x15\x9b\xd8\xcd\xc7\x5f\xfc\xa5\x91\x1e\x4c\x41"
	}
    },
    /* 1 */
    {
	ETYPE_AES256_CTS_HMAC_SHA1_96,
	{
	    128,
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	},
	{
	    32,
	    "\xac\xf7\x70\x7c\x08\x97\x3d\xdf\xdb\x27\xcd\x36\x14\x42\xcc\xfb"
	    "\xa3\x55\xc8\x88\x4c\xb4\x72\xf3\x7d\xa6\x36\xd0\x7d\x56\x78\x7e"
	}
    },
    /* 2 */
    {
	ETYPE_AES256_CTS_HMAC_SHA1_96,
	{
	    128,
	    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	    "\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e"
	    "\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d"
	    "\x0e\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c"
	    "\x0d\x0e\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b"
	    "\x0c\x0d\x0e\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a"
	    "\x0b\x0c\x0d\x0e\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09"
	    "\x0a\x0b\x0c\x0d\x0e\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08"
	},
	{
	    32,
	    "\xc4\x42\xda\x58\x5f\xcb\x80\xe4\x3b\x47\x94\x6f\x25\x40\x93\xe3"
	    "\x73\x29\xd9\x90\x01\x38\x0d\xb7\x83\x71\xdb\x3a\xcf\x5c\x79\x7e"
	}
    },
    /* 3 */
    {
	ETYPE_AES256_CTS_HMAC_SHA1_96,
	{
	    77,
	    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
	    "\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e"
	    "\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d"
	    "\x0e\x0f\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c"
	    "\x0d\x0e\x0f\x10\x00\x01\x02\x03"
	    "\x04\x05\x06\x07\x08"
	},
	{
	    32,
	    "\x00\x53\x95\x3b\x84\xc8\x96\xf4\xeb\x38\x5c\x3f\x2e\x75\x1c\x4a"
	    "\x59\x0e\xd6\xff\xad\xca\x6f\xf6\x4f\x47\xeb\xeb\x8d\x78\x0f\xfc"
	}
    }
};


static int version_flag = 0;
static int help_flag	= 0;

static struct getargs args[] = {
    {"version",	0,	arg_flag,	&version_flag,
     "print version", NULL },
    {"help",	0,	arg_flag,	&help_flag,
     NULL, NULL }
};

static void
usage (int ret)
{
    arg_printusage (args,
		    sizeof(args)/sizeof(*args),
		    NULL,
		    "");
    exit (ret);
}


int
main(int argc, char **argv)
{
    krb5_context context;
    krb5_error_code ret;
    int i, optidx = 0;

    setprogname(argv[0]);

    if(getarg(args, sizeof(args) / sizeof(args[0]), argc, argv, &optidx))
	usage(1);
    
    if (help_flag)
	usage (0);

    if(version_flag){
	print_version(NULL);
	exit(0);
    }

    argc -= optidx;
    argv += optidx;

    ret = krb5_init_context(&context);
    if (ret)
	errx (1, "krb5_init_context failed: %d", ret);

    for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
	test_dh2key(i, context, &tests[i].X, NULL, NULL, 
		    tests[i].type, &tests[i].key);
    }

    krb5_free_context(context);

    return 0;
}
