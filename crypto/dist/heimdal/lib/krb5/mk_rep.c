/*
 * Copyright (c) 1997-1999 Kungliga Tekniska H�gskolan
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

#include <krb5_locl.h>

RCSID("$Id: mk_rep.c,v 1.1.1.1 2000/06/16 18:33:00 thorpej Exp $");

krb5_error_code
krb5_mk_rep(krb5_context context,
	    krb5_auth_context *auth_context,
	    krb5_data *outbuf)
{
  krb5_error_code ret;
  AP_REP ap;
  EncAPRepPart body;
  u_char *buf = NULL;
  size_t buf_size;
  size_t len;
  krb5_crypto crypto;

  ap.pvno = 5;
  ap.msg_type = krb_ap_rep;

  memset (&body, 0, sizeof(body));

  body.ctime = (*auth_context)->authenticator->ctime;
  body.cusec = (*auth_context)->authenticator->cusec;
  body.subkey = NULL;
  if ((*auth_context)->flags & KRB5_AUTH_CONTEXT_DO_SEQUENCE) {
    krb5_generate_seq_number (context,
			      (*auth_context)->keyblock,
			      &(*auth_context)->local_seqnumber);
    body.seq_number = malloc (sizeof(*body.seq_number));
    if (body.seq_number == NULL)
	return ENOMEM;
    *(body.seq_number) = (*auth_context)->local_seqnumber;
  } else
    body.seq_number = NULL;

  ap.enc_part.etype = (*auth_context)->keyblock->keytype;
  ap.enc_part.kvno  = NULL;

  buf_size = length_EncAPRepPart(&body);
  buf = malloc (buf_size);
  if (buf == NULL) {
      free_EncAPRepPart (&body);
      return ENOMEM;
  }

  ret = krb5_encode_EncAPRepPart (context, 
				  buf + buf_size - 1,
				  buf_size,
				  &body, 
				  &len);

  free_EncAPRepPart (&body);
  krb5_crypto_init(context, (*auth_context)->keyblock, 
		   0 /* ap.enc_part.etype */, &crypto);
  ret = krb5_encrypt (context,
		      crypto,
		      KRB5_KU_AP_REQ_ENC_PART,
		      buf + buf_size - len, 
		      len,
		      &ap.enc_part.cipher);
  krb5_crypto_destroy(context, crypto);
  if (ret) {
      free(buf);
      return ret;
  }

  buf_size = length_AP_REP(&ap);
  buf = realloc(buf, buf_size);
  if(buf == NULL) {
      free_AP_REP (&ap);
      return ENOMEM;
  }
  ret = encode_AP_REP (buf + buf_size - 1, buf_size, &ap, &len);
  
  free_AP_REP (&ap);

  if(len != buf_size)
      krb5_abortx(context, "krb5_mk_rep: encoded length != calculated length");
  outbuf->data = buf;
  outbuf->length = len;
  return 0;
}
