/*	$NetBSD: mach_port.c,v 1.24 2002/12/30 12:46:19 manu Exp $ */

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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_port.c,v 1.24 2002/12/30 12:46:19 manu Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/pool.h>
#include <sys/queue.h>
#include <sys/malloc.h>
#include <sys/proc.h>

#include <compat/mach/mach_types.h>
#include <compat/mach/mach_message.h>
#include <compat/mach/mach_port.h>
#include <compat/mach/mach_clock.h>
#include <compat/mach/mach_exec.h>
#include <compat/mach/mach_errno.h>
#include <compat/mach/mach_syscallargs.h>

/* Right and port pools, list of all rights and its lock */
static struct pool mach_port_pool;
static struct pool mach_right_pool;
static LIST_HEAD(mach_right_list, mach_right) mach_right_list;
struct lock mach_right_list_lock;

struct mach_port *mach_bootstrap_port;
struct mach_port *mach_clock_port;

int
mach_sys_reply_port(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval; 
{
	struct mach_right *mr;

	mr = mach_right_get(mach_port_get(), p, MACH_PORT_TYPE_RECEIVE);
	*retval = (register_t)mr;

	return 0;
}

int
mach_sys_thread_self_trap(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct mach_emuldata *med;
	struct mach_right *mr;

	/* 
	 * XXX for now thread kernel port and task kernel port are the same 
	 * awaiting for struct lwp ...
	 */
	med = (struct mach_emuldata *)p->p_emuldata;
	mr = mach_right_get(med->med_kernel, p, MACH_PORT_TYPE_SEND);
	*retval = (register_t)mr;

	return 0;
}


int
mach_sys_task_self_trap(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct mach_emuldata *med;
	struct mach_right *mr;

	med = (struct mach_emuldata *)p->p_emuldata;
	mr = mach_right_get(med->med_kernel, p, MACH_PORT_TYPE_SEND);
	*retval = (register_t)mr;

	return 0;
}


int
mach_sys_host_self_trap(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct mach_emuldata *med;
	struct mach_right *mr;

	med = (struct mach_emuldata *)p->p_emuldata;
	mr = mach_right_get(med->med_host, p, MACH_PORT_TYPE_SEND);
	*retval = (register_t)mr;

	return 0;
}

int 
mach_port_deallocate(args)
	struct mach_trap_args *args;
{
	mach_port_deallocate_request_t *req = args->smsg;
	mach_port_deallocate_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;
	struct proc *p = args->p;
	struct mach_right *mr;

	mr = (struct mach_right *)req->req_name;
	if (mach_right_check(mr, p, MACH_PORT_TYPE_PORT_RIGHTS) != 0) 
		mach_right_put(mr);
	else 
		return mach_msg_error(args, EINVAL);

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

int 
mach_port_allocate(args)
	struct mach_trap_args *args;
{
	mach_port_allocate_request_t *req = args->smsg;
	mach_port_allocate_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;
	struct proc *p = args->p;
	struct mach_right *mr;

	switch (req->req_right) {
	case MACH_PORT_RIGHT_RECEIVE:
		mr = mach_right_get(mach_port_get(), p, MACH_PORT_TYPE_RECEIVE);
		break;

	case MACH_PORT_RIGHT_DEAD_NAME:
		mr = mach_right_get(NULL, p, MACH_PORT_TYPE_DEAD_NAME);
		break;

	case MACH_PORT_RIGHT_PORT_SET:
		mr = mach_right_get(NULL, p, MACH_PORT_TYPE_PORT_SET);
		break;

	default:
		uprintf("mach_port_allocate: unknown right %x\n", 
		    req->req_right);
		break;
	}

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_name = (mach_port_name_t)mr;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

int 
mach_port_insert_right(args)
	struct mach_trap_args *args;
{
	mach_port_insert_right_request_t *req = args->smsg;
	mach_port_insert_right_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;
	struct proc *p = args->p;
	struct mach_right *mr;
	struct mach_right *nmr;
	struct proc *tp;

	mr = (struct mach_right *)req->req_name;
	nmr = NULL;

	/* 
	 * Sanity check: does tmr exist in another process? 
	 */
	if (mach_right_check_all(mr, MACH_PORT_TYPE_ALL_RIGHTS) == 0)
		return mach_msg_error(args, EPERM);
		
	tp = mr->mr_p; /* The target process */

	switch (req->req_poly.disposition) {
	case MACH_MSG_TYPE_MAKE_SEND:
	case MACH_MSG_TYPE_MOVE_SEND:
	case MACH_MSG_TYPE_COPY_SEND:
		/* 
		 * XXX We require that requester has any right. 
		 * Not sure this is right
		 */
		if (mach_right_check(mr, p, MACH_PORT_TYPE_PORT_RIGHTS) == 0)
			return mach_msg_error(args, EPERM);
		nmr = mach_right_get(mr->mr_port, tp, MACH_PORT_TYPE_SEND);
		break;

	case MACH_MSG_TYPE_MAKE_SEND_ONCE:
	case MACH_MSG_TYPE_MOVE_SEND_ONCE:
		/* 
		 * XXX We require that requester has any right. 
		 * Not sure this is right
		 */
		if (mach_right_check(mr, p, MACH_PORT_TYPE_PORT_RIGHTS) == 0)
			return mach_msg_error(args, EPERM);
		nmr = mach_right_get(mr->mr_port, 
		    tp, MACH_PORT_TYPE_SEND_ONCE);
		break;

	case MACH_MSG_TYPE_MOVE_RECEIVE:
		/* 
		 * XXX We require that requester has any right. 
		 * Not sure this is right
		 */
		if (mach_right_check(mr, p, MACH_PORT_TYPE_PORT_RIGHTS) == 0)
			return mach_msg_error(args, EPERM);
		nmr = mach_right_get(mr->mr_port, tp, MACH_PORT_TYPE_RECEIVE);
		break;

	default:
		uprintf("mach_port_insert_right: unknown right %x\n",
		    req->req_poly.disposition);
		break;
	}

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

int 
mach_port_type(args)
	struct mach_trap_args *args;
{
	mach_port_type_request_t *req = args->smsg;
	mach_port_type_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;
	struct proc *p = args->p;
	struct mach_right *mr;

	mr = (struct mach_right *)req->req_name;
	if (mach_right_check(mr, p, MACH_PORT_TYPE_PORT_RIGHTS) == 0)
		return mach_msg_error(args, EPERM);

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_retval = 0;
	rep->rep_ptype = mr->mr_type;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

int 
mach_port_set_attributes(args)
	struct mach_trap_args *args;
{
	mach_port_set_attributes_request_t *req = args->smsg;
	mach_port_set_attributes_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;

	switch(req->req_flavor) {
	case MACH_PORT_LIMITS_INFO:
	case MACH_PORT_RECEIVE_STATUS:
	case MACH_PORT_DNREQUESTS_SIZE:
		break;
	default:
		uprintf("mach_port_get_attributes: unknown flavor %d\n",
		    req->req_flavor);
		break;
	}

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

/* XXX need to implement port sets before doing that */
int 
mach_port_insert_member(args)
	struct mach_trap_args *args;
{
	mach_port_insert_member_request_t *req = args->smsg;
	mach_port_insert_member_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

int 
mach_port_move_member(args)
	struct mach_trap_args *args;
{
	mach_port_move_member_request_t *req = args->smsg;
	mach_port_move_member_reply_t *rep = args->rmsg;
	size_t *msglen = args->rsize;
	struct proc *p = args->p;
	struct mach_right *mrr = (struct mach_right *)req->req_member;
	struct mach_right *mrs = (struct mach_right *)req->req_after;

	/* XXX the target task may be another task */
	if (mach_right_check(mrr, p, MACH_PORT_TYPE_RECEIVE) == 0)
		return mach_msg_error(args, EPERM);

	if (mach_right_check(mrs, p, MACH_PORT_TYPE_PORT_SET) == 0)
		return mach_msg_error(args, EPERM);
		
	lockmgr(&mach_right_list_lock, LK_EXCLUSIVE, NULL);
	/* If it was already in a port set, remove it */
	if (mrr->mr_sethead != mrr)
		LIST_REMOVE(mrr, mr_setlist);

	/* Insert it into the new port set */
	LIST_INSERT_HEAD(&mrs->mr_set, mrr, mr_setlist);
	mrr->mr_sethead = mrs;

	lockmgr(&mach_right_list_lock, LK_RELEASE, NULL);

	rep->rep_msgh.msgh_bits =
	    MACH_MSGH_REPLY_LOCAL_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	rep->rep_msgh.msgh_size = sizeof(*rep) - sizeof(rep->rep_trailer);
	rep->rep_msgh.msgh_local_port = req->req_msgh.msgh_local_port;
	rep->rep_msgh.msgh_id = req->req_msgh.msgh_id + 100;
	rep->rep_trailer.msgh_trailer_size = 8;

	*msglen = sizeof(*rep);
	return 0;
}

void 
mach_port_init(void) 
{
	pool_init(&mach_port_pool, sizeof (struct mach_port),
	    0, 0, 128, "mach_port_pool", NULL);
	pool_init(&mach_right_pool, sizeof (struct mach_right),
	    0, 0, 128, "mach_right_pool", NULL);
	LIST_INIT(&mach_right_list);
	lockinit(&mach_right_list_lock, PZERO|PCATCH, "mach_right_list", 0, 0);

	mach_bootstrap_port = mach_port_get();
	mach_clock_port = mach_port_get();

	return;
}

struct mach_port *
mach_port_get(void)
{
	struct mach_port *mp;

	mp = (struct mach_port *)pool_get(&mach_port_pool, M_WAITOK);
	bzero(mp, sizeof(*mp));
	mp->mp_recv = NULL;
	mp->mp_count = 0;
	TAILQ_INIT(&mp->mp_msglist);
	lockinit(&mp->mp_msglock, PZERO|PCATCH, "mach_port", 0, 0);

	return mp;
}

void
mach_port_put(mp)
	struct mach_port *mp;
{
	struct mach_message *mm;

	if (mp->mp_refcount != 0) {
		uprintf("mach_port_put: trying to free a referenced port\n");
		return;
	}

	lockmgr(&mp->mp_msglock, LK_EXCLUSIVE, NULL);
	while ((mm = TAILQ_FIRST(&mp->mp_msglist)) != NULL)
		mach_message_put_exclocked(mm);
	lockmgr(&mp->mp_msglock, LK_RELEASE, NULL);
	lockmgr(&mp->mp_msglock, LK_DRAIN, NULL);
	
	pool_put(&mach_port_pool, mp);

	return;
}

struct mach_right *
mach_right_get(mp, p, type)
	struct mach_port *mp;
	struct proc *p;
	int type;
{
	struct mach_right *mr;
	struct mach_emuldata *med;
	int rights;

	med = (struct mach_emuldata *)p->p_emuldata;

	/* Send and receive right must return an existing right */
	rights = (MACH_PORT_TYPE_SEND | MACH_PORT_TYPE_RECEIVE);
	if (type & rights) {
		lockmgr(&mach_right_list_lock, LK_SHARED, NULL);
		LIST_FOREACH(mr, &med->med_right, mr_list) {
			if ((mr->mr_port == mp) && (mr->mr_type & rights))
				break;
		}
		lockmgr(&mach_right_list_lock, LK_RELEASE, NULL);

		if (mr != NULL) {
			mr->mr_type |= type;
			mr->mr_refcount++;
			goto rcvck;
		}
	}

	mr = pool_get(&mach_right_pool, M_WAITOK);

	mr->mr_port = mp;
	mr->mr_p = p;
	mr->mr_type = type;
	mr->mr_sethead = mr; 
	mr->mr_refcount = 1;

	LIST_INIT(&mr->mr_set); /* Usefull only for a right on a port set */

	if (mp != NULL)
		mp->mp_refcount++;

	/* Insert the right in one of the process right lists */
	if (type & MACH_PORT_TYPE_ALL_RIGHTS) {
		lockmgr(&mach_right_list_lock, LK_EXCLUSIVE, NULL);
		LIST_INSERT_HEAD(&med->med_right, mr, mr_list);
		LIST_INSERT_HEAD(&mach_right_list, mr, mr_listall);
		lockmgr(&mach_right_list_lock, LK_RELEASE, NULL);
	}
		
rcvck:
	if (type & MACH_PORT_TYPE_RECEIVE) {
		/* 
		 * Destroy the former receive right on this port, and
		 * register the new right.
		 */
		if (mr->mr_port->mp_recv != NULL)
			mach_right_put(mr->mr_port->mp_recv);
		mr->mr_port->mp_recv = mr;
	}
	return mr;
}

void 
mach_right_put(mr)
	struct mach_right *mr;
{
	lockmgr(&mach_right_list_lock, LK_EXCLUSIVE, NULL);
	mach_right_put_exclocked(mr);
	lockmgr(&mach_right_list_lock, LK_RELEASE, NULL);

	return;
}

void
mach_right_put_shlocked(mr)
	struct mach_right *mr;
{
	lockmgr(&mach_right_list_lock, LK_UPGRADE, NULL);
	mach_right_put_exclocked(mr);
	lockmgr(&mach_right_list_lock, LK_DOWNGRADE, NULL);

	return;
}

void
mach_right_put_exclocked(mr)
	struct mach_right *mr;
{
	struct mach_right *cmr;

	mr->mr_refcount--;
	if (mr->mr_refcount != 0)
		return;

	if (mr->mr_type & MACH_PORT_TYPE_PORT_SET) {
		while((cmr = LIST_FIRST(&mr->mr_set)) != NULL)
			mach_right_put_exclocked(cmr);			
	}

	if (mr->mr_type & MACH_PORT_TYPE_ALL_RIGHTS) {
		LIST_REMOVE(mr, mr_list);
		LIST_REMOVE(mr, mr_listall);
	}

	if (mr->mr_port != NULL) {
		mr->mr_port->mp_refcount--;
		if (mr->mr_port->mp_refcount == 0)
			mach_port_put(mr->mr_port);
	}

	pool_put(&mach_right_pool, mr);
	return;
}

/* 
 * Check that a process do have a given right
 */
int
mach_right_check(mr, p, type)
	struct mach_right *mr;
	struct proc *p;
	int type;
{
	struct mach_right *cmr;
	struct mach_emuldata *med;

	med = (struct mach_emuldata *)p->p_emuldata;

	lockmgr(&mach_right_list_lock, LK_SHARED, NULL);

	LIST_FOREACH(cmr, &med->med_right, mr_list) {
		if (cmr != mr)
			continue;
		if (type & cmr->mr_type)
			break;
	}

	lockmgr(&mach_right_list_lock, LK_RELEASE, NULL);

	if (cmr != NULL)
		return 1;
	return 0;
}


/* 
 * Check for a right in any process
 */
int
mach_right_check_all(mr, type)
	struct mach_right *mr;
	int type;
{
	struct mach_right *cmr;

	lockmgr(&mach_right_list_lock, LK_SHARED, NULL);

	LIST_FOREACH(cmr, &mach_right_list, mr_listall) 
		if ((cmr == mr) && (mr->mr_type & type))
			break;

	lockmgr(&mach_right_list_lock, LK_RELEASE, NULL);

	if (cmr == NULL)
		return 0;
	return 1;
}

#ifdef DEBUG_MACH
void 
mach_debug_port(p, more)
	struct proc *p;
	int more;
{
	struct mach_emuldata *med;
	struct mach_right *mr;
	
	printf("mach_debug_port(%p)\n", p);

	med = (struct mach_emuldata *)p->p_emuldata;

	LIST_FOREACH(mr, &med->med_right, mr_list) {
		printf(" right = %p\n", mr);
		if (more == 0)
			continue;
		printf("  port = %p\n", mr->mr_port);
		printf("    recv = %p\n", mr->mr_port->mp_recv);
		printf("    count = %d\n", mr->mr_port->mp_count);
		printf("    refcount = %d\n", mr->mr_port->mp_refcount);
		printf("  proc = %p\n", mr->mr_p);
		printf("  type = 0x%x\n\n", mr->mr_type);
	}
	return;
}

#endif /* DEBUG_MACH */
