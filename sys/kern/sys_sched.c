/*	$NetBSD: sys_sched.c,v 1.16 2008/02/22 22:32:49 rmind Exp $	*/

/*
 * Copyright (c) 2008, Mindaugas Rasiukevicius <rmind at NetBSD org>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * TODO:
 *  - Handle pthread_setschedprio() as defined by POSIX;
 *  - Handle sched_yield() case for SCHED_FIFO as defined by POSIX;
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: sys_sched.c,v 1.16 2008/02/22 22:32:49 rmind Exp $");

#include <sys/param.h>

#include <sys/cpu.h>
#include <sys/kauth.h>
#include <sys/kmem.h>
#include <sys/lwp.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/pset.h>
#include <sys/sched.h>
#include <sys/syscallargs.h>
#include <sys/sysctl.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/unistd.h>

/*
 * Convert user priority or the in-kernel priority or convert the current
 * priority to the appropriate range according to the policy change.
 */
static pri_t
convert_pri(lwp_t *l, int policy, pri_t pri)
{
	int delta = 0;

	switch (policy) {
	case SCHED_OTHER:
		delta = PRI_USER;
		break;
	case SCHED_FIFO:
	case SCHED_RR:
		delta = PRI_USER_RT;
		break;
	default:
		panic("upri_to_kpri");
	}

	if (pri != PRI_NONE) {
		/* Convert user priority to the in-kernel */
		KASSERT(pri >= SCHED_PRI_MIN && pri <= SCHED_PRI_MAX);
		return pri + delta;
	}
	if (l->l_class == policy)
		return l->l_priority;

	/* Change the current priority to the appropriate range */
	if (l->l_class == SCHED_OTHER) {
		KASSERT(policy == SCHED_FIFO || policy == SCHED_RR);
		return l->l_priority + delta;
	}
	if (policy == SCHED_OTHER) {
		KASSERT(l->l_class == SCHED_FIFO || l->l_class == SCHED_RR);
		return l->l_priority - delta;
	}
	KASSERT(l->l_class != SCHED_OTHER && policy != SCHED_OTHER);
	return l->l_class;
}

/*
 * Set scheduling parameters.
 */
int
sys__sched_setparam(struct lwp *l, const struct sys__sched_setparam_args *uap,
    register_t *retval)
{
	/* {
		syscallarg(pid_t) pid;
		syscallarg(lwpid_t) lid;
		syscallarg(int) policy;
		syscallarg(const struct sched_param *) params;
	} */
	struct sched_param param;
	struct proc *p;
	struct lwp *t;
	lwpid_t lid;
	u_int lcnt;
	int policy;
	pri_t pri;
	int error;

	/* Get the parameters from the user-space */
	error = copyin(SCARG(uap, params), &param, sizeof(param));
	if (error) {
		return error;
	}
	pri = param.sched_priority;
	policy = SCARG(uap, policy);

	/* If no parameters specified, just return (this should not happen) */
	if (pri == PRI_NONE && policy == SCHED_NONE)
		return 0;

	/* Validate scheduling class */
	if (policy != SCHED_NONE && (policy < SCHED_OTHER || policy > SCHED_RR))
		return EINVAL;

	/* Validate priority */
	if (pri != PRI_NONE && (pri < SCHED_PRI_MIN || pri > SCHED_PRI_MAX))
		return EINVAL;

	if (SCARG(uap, pid) != 0) {
		/* Find the process */
		p = p_find(SCARG(uap, pid), PFIND_UNLOCK_FAIL);
		if (p == NULL)
			return ESRCH;
		mutex_enter(&p->p_smutex);
		mutex_exit(&proclist_lock);
		/* Disallow modification of system processes */
		if (p->p_flag & PK_SYSTEM) {
			mutex_exit(&p->p_smutex);
			return EPERM;
		}
	} else {
		/* Use the calling process */
		p = l->l_proc;
		mutex_enter(&p->p_smutex);
	}

	/* Find the LWP(s) */
	lcnt = 0;
	lid = SCARG(uap, lid);
	LIST_FOREACH(t, &p->p_lwps, l_sibling) {
		pri_t kpri;
		int lpolicy;

		if (lid && lid != t->l_lid)
			continue;
		lcnt++;
		KASSERT(pri != PRI_NONE || policy != SCHED_NONE);
		lwp_lock(t);

		if (policy == SCHED_NONE)
			lpolicy = t->l_class;
		else
			lpolicy = policy;

		/*
		 * Note that, priority may need to be changed to get into
		 * the correct priority range of the new scheduling class.
		 */
		kpri = convert_pri(t, lpolicy, pri);

		/* Check the permission */
		error = kauth_authorize_process(l->l_cred,
		    KAUTH_PROCESS_SCHEDULER_SETPARAM, p, t, KAUTH_ARG(lpolicy),
		    KAUTH_ARG(kpri));
		if (error) {
			lwp_unlock(t);
			break;
		}

		/* Set the scheduling class */
		if (policy != SCHED_NONE)
			t->l_class = policy;

		/* Change the priority */
		if (t->l_priority != kpri)
			lwp_changepri(t, kpri);

		lwp_unlock(t);
	}
	mutex_exit(&p->p_smutex);
	return (lcnt == 0) ? ESRCH : error;
}

/*
 * Get scheduling parameters.
 */
int
sys__sched_getparam(struct lwp *l, const struct sys__sched_getparam_args *uap,
    register_t *retval)
{
	/* {
		syscallarg(pid_t) pid;
		syscallarg(lwpid_t) lid;
		syscallarg(int *) policy;
		syscallarg(struct sched_param *) params;
	} */
	struct sched_param param;
	struct lwp *t;
	int error, policy;

	/* Locks the LWP */
	t = lwp_find2(SCARG(uap, pid), SCARG(uap, lid));
	if (t == NULL)
		return ESRCH;

	/* Check the permission */
	error = kauth_authorize_process(l->l_cred,
	    KAUTH_PROCESS_SCHEDULER_GETPARAM, t->l_proc, NULL, NULL, NULL);
	if (error != 0) {
		lwp_unlock(t);
		return error;
	}

	param.sched_priority = t->l_priority;
	policy = t->l_class;
	lwp_unlock(t);

	switch (policy) {
	case SCHED_OTHER:
		param.sched_priority -= PRI_USER;
		break;
	case SCHED_RR:
	case SCHED_FIFO:
		param.sched_priority -= PRI_USER_RT;
		break;
	}
	error = copyout(&param, SCARG(uap, params), sizeof(param));
	if (error == 0 && SCARG(uap, policy) != NULL)
		error = copyout(&policy, SCARG(uap, policy), sizeof(int));
	return error;
}

/*
 * Set affinity.
 */
int
sys__sched_setaffinity(struct lwp *l,
    const struct sys__sched_setaffinity_args *uap, register_t *retval)
{
	/* {
		syscallarg(pid_t) pid;
		syscallarg(lwpid_t) lid;
		syscallarg(size_t) size;
		syscallarg(void *) cpuset;
	} */
	cpuset_t *cpuset;
	struct cpu_info *ci = NULL;
	struct proc *p;
	struct lwp *t;
	CPU_INFO_ITERATOR cii;
	lwpid_t lid;
	u_int lcnt;
	int error;

	/* Allocate the CPU set, and get it from userspace */
	cpuset = kmem_zalloc(sizeof(cpuset_t), KM_SLEEP);
	error = copyin(SCARG(uap, cpuset), cpuset,
	    min(SCARG(uap, size), sizeof(cpuset_t)));
	if (error)
		goto error;

	/* Look for a CPU in the set */
	for (CPU_INFO_FOREACH(cii, ci))
		if (CPU_ISSET(cpu_index(ci), cpuset))
			break;
	if (ci == NULL) {
		/* Empty set */
		kmem_free(cpuset, sizeof(cpuset_t));
		cpuset = NULL; 
	}

	if (SCARG(uap, pid) != 0) {
		/* Find the process */
		p = p_find(SCARG(uap, pid), PFIND_UNLOCK_FAIL);
		if (p == NULL) {
			error = ESRCH;
			goto error;
		}
		mutex_enter(&p->p_smutex);
		mutex_exit(&proclist_lock);
	} else {
		/* Use the calling process */
		p = l->l_proc;
		mutex_enter(&p->p_smutex);
	}

	/*
	 * Check the permission.
	 * Disallow modification of system processes.
	 */
	error = kauth_authorize_process(l->l_cred,
	    KAUTH_PROCESS_SCHEDULER_SETAFFINITY, p, NULL, NULL, NULL);
	if (error != 0) {
		mutex_exit(&p->p_smutex);
		goto error;
	}
	if ((p->p_flag & PK_SYSTEM) != 0) {
		mutex_exit(&p->p_smutex);
		error = EPERM;
		goto error;
	}

	/* Find the LWP(s) */
	lcnt = 0;
	lid = SCARG(uap, lid);
	LIST_FOREACH(t, &p->p_lwps, l_sibling) {
		if (lid && lid != t->l_lid)
			continue;
		lwp_lock(t);
		if (cpuset) {
			/* Set the affinity flag and new CPU set */
			t->l_flag |= LW_AFFINITY;
			memcpy(&t->l_affinity, cpuset, sizeof(cpuset_t));
			/* Migrate to another CPU, unlocks LWP */
			lwp_migrate(t, ci);
		} else {
			/* Unset the affinity flag */
			t->l_flag &= ~LW_AFFINITY;
			lwp_unlock(t);
		}
		lcnt++;
	}
	mutex_exit(&p->p_smutex);
	if (lcnt == 0)
		error = ESRCH;
error:
	if (cpuset != NULL)
		kmem_free(cpuset, sizeof(cpuset_t));
	return error;
}

/*
 * Get affinity.
 */
int
sys__sched_getaffinity(struct lwp *l,
    const struct sys__sched_getaffinity_args *uap, register_t *retval)
{
	/* {
		syscallarg(pid_t) pid;
		syscallarg(lwpid_t) lid;
		syscallarg(size_t) size;
		syscallarg(void *) cpuset;
	} */
	struct lwp *t;
	void *cpuset;
	int error;

	if (SCARG(uap, size) <= 0)
		return EINVAL;
	cpuset = kmem_zalloc(sizeof(cpuset_t), KM_SLEEP);

	/* Locks the LWP */
	t = lwp_find2(SCARG(uap, pid), SCARG(uap, lid));
	if (t == NULL) {
		kmem_free(cpuset, sizeof(cpuset_t));
		return ESRCH;
	}
	/* Check the permission */
	if (kauth_authorize_process(l->l_cred,
	    KAUTH_PROCESS_SCHEDULER_GETAFFINITY, t->l_proc, NULL, NULL, NULL)) {
		lwp_unlock(t);
		kmem_free(cpuset, sizeof(cpuset_t));
		return EPERM;
	}
	if (t->l_flag & LW_AFFINITY)
		memcpy(cpuset, &t->l_affinity, sizeof(cpuset_t));
	lwp_unlock(t);

	error = copyout(cpuset, SCARG(uap, cpuset),
	    min(SCARG(uap, size), sizeof(cpuset_t)));

	kmem_free(cpuset, sizeof(cpuset_t));
	return error;
}

/*
 * Yield.
 */
int
sys_sched_yield(struct lwp *l, const void *v, register_t *retval)
{

	yield();
	return 0;
}

/*
 * Sysctl nodes and initialization.
 */
SYSCTL_SETUP(sysctl_sched_setup, "sysctl sched setup")
{
	const struct sysctlnode *node = NULL;

	sysctl_createv(clog, 0, NULL, NULL,
		CTLFLAG_PERMANENT,
		CTLTYPE_NODE, "kern", NULL,
		NULL, 0, NULL, 0,
		CTL_KERN, CTL_EOL);
	sysctl_createv(clog, 0, NULL, NULL,
		CTLFLAG_PERMANENT|CTLFLAG_IMMEDIATE,
		CTLTYPE_INT, "posix_sched",
		SYSCTL_DESCR("Version of IEEE Std 1003.1 and its "
			     "Process Scheduling option to which the "
			     "system attempts to conform"),
		NULL, _POSIX_PRIORITY_SCHEDULING, NULL, 0,
		CTL_KERN, CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, NULL, &node,
		CTLFLAG_PERMANENT,
		CTLTYPE_NODE, "sched",
		SYSCTL_DESCR("Scheduler options"),
		NULL, 0, NULL, 0,
		CTL_KERN, CTL_CREATE, CTL_EOL);

	if (node == NULL)
		return;

	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_IMMEDIATE,
		CTLTYPE_INT, "pri_min",
		SYSCTL_DESCR("Minimal POSIX real-time priority"),
		NULL, SCHED_PRI_MIN, NULL, 0,
		CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_IMMEDIATE,
		CTLTYPE_INT, "pri_max",
		SYSCTL_DESCR("Minimal POSIX real-time priority"),
		NULL, SCHED_PRI_MAX, NULL, 0,
		CTL_CREATE, CTL_EOL);
}
