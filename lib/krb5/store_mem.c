/*
 * Copyright (c) 1997 Kungliga Tekniska H�gskolan
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
 * 3. All advertising materials mentioning features or use of this software 
 *    must display the following acknowledgement: 
 *      This product includes software developed by Kungliga Tekniska 
 *      H�gskolan and its contributors. 
 *
 * 4. Neither the name of the Institute nor the names of its contributors 
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

RCSID("$Id$");

typedef struct mem_storage{
    unsigned char *base;
    size_t size;
    unsigned char *ptr;
}mem_storage;

static size_t
mem_fetch(krb5_storage *sp, void *data, size_t size)
{
    mem_storage *s = (mem_storage*)sp->data;
    if(size > s->base + s->size - s->ptr)
	size = s->base + s->size - s->ptr;
    memmove(data, s->ptr, size);
    sp->seek(sp, size, SEEK_CUR);
    return size;
}

size_t
mem_store(krb5_storage *sp, void *data, size_t size)
{
    mem_storage *s = (mem_storage*)sp->data;
    if(size > s->base + s->size - s->ptr)
	size = s->base + s->size - s->ptr;
    memmove(s->ptr, data, size);
    sp->seek(sp, size, SEEK_CUR);
    return size;
}

off_t
mem_seek(krb5_storage *sp, off_t offset, int whence)
{
    mem_storage *s = (mem_storage*)sp->data;
    switch(whence){
    case SEEK_SET:
	if(offset > s->size)
	    offset = s->size;
	if(offset < 0)
	    offset = 0;
	s->ptr = s->base + offset;
	break;
    case SEEK_CUR:
	return sp->seek(sp, s->ptr - s->base + offset, SEEK_SET);
    case SEEK_END:
	return sp->seek(sp, s->size + offset, SEEK_SET);
    default:
	errno = EINVAL;
	return -1;
    }
    return s->ptr - s->base;
}

krb5_storage *
krb5_storage_from_mem(void *buf, size_t len)
{
    krb5_storage *sp = malloc(sizeof(krb5_storage));
    mem_storage *s = malloc(sizeof(*s));
    sp->data = s;
    s->base = buf;
    s->size = len;
    s->ptr = buf;
    sp->fetch = mem_fetch;
    sp->store = mem_store;
    sp->seek = mem_seek;
    sp->free = NULL;
    return sp;
}


