/*
 * Copyright (c) 1997 - 2004 Kungliga Tekniska H�gskolan
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "krb5_locl.h"
#include "store-int.h"

__RCSID("$Heimdal: store_fd.c 17779 2006-06-30 21:23:19Z lha $"
        "$NetBSD: store_fd.c,v 1.2 2008/03/22 08:37:15 mlelstv Exp $");

typedef struct fd_storage {
    int fd;
} fd_storage;

#define FD(S) (((fd_storage*)(S)->data)->fd)

static ssize_t
fd_fetch(krb5_storage * sp, void *data, size_t size)
{
    return net_read(FD(sp), data, size);
}

static ssize_t
fd_store(krb5_storage * sp, const void *data, size_t size)
{
    return net_write(FD(sp), data, size);
}

static off_t
fd_seek(krb5_storage * sp, off_t offset, int whence)
{
    return lseek(FD(sp), offset, whence);
}

static void
fd_free(krb5_storage * sp)
{
    close(FD(sp));
}

krb5_storage * KRB5_LIB_FUNCTION
krb5_storage_from_fd(int fd)
{
    krb5_storage *sp;

    fd = dup(fd);
    if (fd < 0)
	return NULL;

    sp = malloc(sizeof(krb5_storage));
    if (sp == NULL) {
	close(fd);
	return NULL;
    }

    sp->data = malloc(sizeof(fd_storage));
    if (sp->data == NULL) {
	close(fd);
	free(sp);
	return NULL;
    }
    sp->flags = 0;
    sp->eof_code = HEIM_ERR_EOF;
    FD(sp) = fd;
    sp->fetch = fd_fetch;
    sp->store = fd_store;
    sp->seek = fd_seek;
    sp->free = fd_free;
    return sp;
}
