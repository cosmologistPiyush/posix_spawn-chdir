/*	$NetBSD: mach_port.h,v 1.5 2002/12/10 21:36:45 manu Exp $ */

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Emmanuel Dreyfus
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
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#ifndef	_MACH_PORT_H_
#define	_MACH_PORT_H_

/* port_deallocate */

typedef struct {
	mach_msg_header_t req_msgh;
	mach_ndr_record_t req_ndr;
	mach_port_name_t req_name;
} mach_port_deallocate_request_t;

typedef struct {
	mach_msg_header_t rep_msgh;
	mach_ndr_record_t rep_ndr;
	mach_kern_return_t rep_retval;
	mach_msg_trailer_t rep_trailer;
} mach_port_deallocate_reply_t;

/* port_allocate */

typedef struct {
	mach_msg_header_t req_msgh;
	mach_ndr_record_t req_ndr;
	mach_port_right_t req_right;
} mach_port_allocate_request_t;  

typedef struct {
	mach_msg_header_t rep_msgh;
	mach_ndr_record_t rep_ndr;
	mach_kern_return_t rep_retval;
	mach_port_name_t rep_name;
	mach_msg_trailer_t rep_trailer;
} mach_port_allocate_reply_t;  

/* port_insert_right */

typedef struct {
	mach_msg_header_t req_msgh;
	mach_msg_body_t req_body;
	mach_msg_port_descriptor_t req_poly;
	mach_ndr_record_t req_ndr;
	mach_port_name_t req_name;
} mach_port_insert_right_request_t;  

typedef struct {
	mach_msg_header_t rep_msgh;
	mach_ndr_record_t rep_ndr;
	mach_kern_return_t rep_retval;
	mach_msg_trailer_t rep_trailer;
} mach_port_insert_right_reply_t;  

/* port_type */

typedef struct {
	mach_msg_header_t req_msgh;
	mach_ndr_record_t req_ndr;
	mach_port_name_t req_name;
} mach_port_type_request_t;  

typedef struct {
	mach_msg_header_t rep_msgh;
	mach_ndr_record_t rep_ndr;
	mach_kern_return_t rep_retval;
	mach_port_type_t rep_ptype;
	mach_msg_trailer_t rep_trailer;
} mach_port_type_reply_t;  

/* port_get_attributes */

typedef int mach_port_flavor_t;
typedef mach_natural_t mach_port_seqno_t; 
typedef mach_natural_t mach_port_mscount_t;
typedef mach_natural_t mach_port_msgcount_t;
typedef mach_natural_t mach_port_rights_t;
#define MACH_PORT_LIMITS_INFO 1
typedef struct mach_port_status {
	mach_port_name_t	mps_pset;
	mach_port_seqno_t	mps_seqno;
	mach_port_mscount_t	mps_mscount;
	mach_port_msgcount_t	mps_qlimit;
	mach_port_msgcount_t	mps_msgcount;
	mach_port_rights_t	mps_sorights;
	mach_boolean_t		mps_srights;
	mach_boolean_t		mps_pdrequest;
	mach_boolean_t		mps_nsrequest;
	unsigned int		mps_flags;
} mach_port_status_t;
#define MACH_PORT_RECEIVE_STATUS 2
typedef struct mach_port_limits {
	mach_port_msgcount_t	mpl_qlimit;
} mach_port_limits_t;
#define MACH_PORT_DNREQUESTS_SIZE 3

typedef struct {
	mach_msg_header_t req_msgh;
	mach_ndr_record_t req_ndr;
	mach_port_name_t req_name;
	mach_port_flavor_t req_flavor;
	mach_msg_type_number_t req_count;
	mach_integer_t req_port_info[10];
} mach_port_set_attributes_request_t;

typedef struct {
	mach_msg_header_t rep_msgh;
	mach_ndr_record_t rep_ndr;
	mach_kern_return_t rep_retval;
	mach_msg_trailer_t rep_trailer;
} mach_port_set_attributes_reply_t;
	
/* port_insert_member */

typedef struct {
	mach_msg_header_t req_msgh;
	mach_ndr_record_t req_ndr;
	mach_port_name_t req_name;
	mach_port_name_t req_pset;
} mach_port_insert_member_request_t;

typedef struct {
	mach_msg_header_t rep_msgh;
	mach_ndr_record_t rep_ndr;
	mach_kern_return_t rep_retval;
	mach_msg_trailer_t rep_trailer;
} mach_port_insert_member_reply_t;

int mach_port_deallocate(struct proc *, mach_msg_header_t *, 
    size_t,  mach_msg_header_t *);
int mach_port_allocate(struct proc *, mach_msg_header_t *,
    size_t,  mach_msg_header_t *);
int mach_port_insert_right(struct proc *, mach_msg_header_t *,
    size_t,  mach_msg_header_t *);
int mach_port_type(struct proc *, mach_msg_header_t *,
    size_t,  mach_msg_header_t *);
int mach_port_set_attributes(struct proc *, mach_msg_header_t *,
    size_t,  mach_msg_header_t *);
int mach_port_insert_member(struct proc *, mach_msg_header_t *,
    size_t,  mach_msg_header_t *);

#endif /* _MACH_PORT_H_ */
