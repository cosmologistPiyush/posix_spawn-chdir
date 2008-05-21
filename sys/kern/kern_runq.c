/*	$NetBSD: kern_runq.c,v 1.8 2008/05/21 15:41:03 ad Exp $	*/

/*
 * Copyright (c) 2007, 2008 Mindaugas Rasiukevicius <rmind at NetBSD org>
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: kern_runq.c,v 1.8 2008/05/21 15:41:03 ad Exp $");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/bitops.h>
#include <sys/cpu.h>
#include <sys/idle.h>
#include <sys/intr.h>
#include <sys/kmem.h>
#include <sys/lwp.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/syscallargs.h>
#include <sys/sysctl.h>
#include <sys/systm.h>
#include <sys/types.h>

/*
 * Priority related defintions.
 */
#define	PRI_TS_COUNT	(NPRI_USER)
#define	PRI_RT_COUNT	(PRI_COUNT - PRI_TS_COUNT)
#define	PRI_HTS_RANGE	(PRI_TS_COUNT / 10)

#define	PRI_HIGHEST_TS	(MAXPRI_USER)

/*
 * Bits per map.
 */
#define	BITMAP_BITS	(32)
#define	BITMAP_SHIFT	(5)
#define	BITMAP_MSB	(0x80000000U)
#define	BITMAP_MASK	(BITMAP_BITS - 1)

/*
 * Structures, runqueue.
 */

const int	schedppq = 1;

typedef struct {
	TAILQ_HEAD(, lwp) q_head;
} queue_t;

typedef struct {
	/* Lock and bitmap */
	uint32_t	r_bitmap[PRI_COUNT >> BITMAP_SHIFT];
	/* Counters */
	u_int		r_count;	/* Count of the threads */
	u_int		r_avgcount;	/* Average count of threads */
	u_int		r_mcount;	/* Count of migratable threads */
	/* Runqueues */
	queue_t		r_rt_queue[PRI_RT_COUNT];
	queue_t		r_ts_queue[PRI_TS_COUNT];
} runqueue_t;

static void *	sched_getrq(runqueue_t *, const pri_t);
#ifdef MULTIPROCESSOR
static lwp_t *	sched_catchlwp(void);
static void	sched_balance(void *);
#endif

/*
 * Preemption control.
 */
int		sched_upreempt_pri = PRI_KERNEL;
#if defined(__HAVE_PREEMPTION) && defined(i386)
int		sched_kpreempt_pri = PRI_USER_RT;
#else
/* XXX disable for now until any bugs are worked out. */
int		sched_kpreempt_pri = 1000;
#endif

/*
 * Migration and balancing.
 */
static u_int	cacheht_time;		/* Cache hotness time */
static u_int	min_catch;		/* Minimal LWP count for catching */
static u_int	balance_period;		/* Balance period */
static struct cpu_info *worker_ci;	/* Victim CPU */
#ifdef MULTIPROCESSOR
static struct callout balance_ch;	/* Callout of balancer */
#endif

void
runq_init(void)
{

	/* Balancing */
	worker_ci = curcpu();
	cacheht_time = mstohz(3);		/* ~3 ms  */
	balance_period = mstohz(300);		/* ~300ms */

	/* Minimal count of LWPs for catching */
	min_catch = 1;

	/* Initialize balancing callout and run it */
#ifdef MULTIPROCESSOR
	callout_init(&balance_ch, CALLOUT_MPSAFE);
	callout_setfunc(&balance_ch, sched_balance, NULL);
	callout_schedule(&balance_ch, balance_period);
#endif
}

void
sched_cpuattach(struct cpu_info *ci)
{
	runqueue_t *ci_rq;
	void *rq_ptr;
	u_int i, size;

	if (ci->ci_schedstate.spc_lwplock == NULL) {
		ci->ci_schedstate.spc_lwplock =
		    mutex_obj_alloc(MUTEX_DEFAULT, IPL_SCHED);
	}
	if (ci == lwp0.l_cpu) {
		/* Initialize the scheduler structure of the primary LWP */
		lwp0.l_mutex = ci->ci_schedstate.spc_lwplock;
	}
	if (ci->ci_schedstate.spc_mutex != NULL) {
		/* Already initialized. */
		return;
	}

	/* Allocate the run queue */
	size = roundup2(sizeof(runqueue_t), coherency_unit) + coherency_unit;
	rq_ptr = kmem_zalloc(size, KM_SLEEP);
	if (rq_ptr == NULL) {
		panic("sched_cpuattach: could not allocate the runqueue");
	}
	ci_rq = (void *)(roundup2((uintptr_t)(rq_ptr), coherency_unit));

	/* Initialize run queues */
	ci->ci_schedstate.spc_mutex =
	    mutex_obj_alloc(MUTEX_DEFAULT, IPL_SCHED);
	for (i = 0; i < PRI_RT_COUNT; i++)
		TAILQ_INIT(&ci_rq->r_rt_queue[i].q_head);
	for (i = 0; i < PRI_TS_COUNT; i++)
		TAILQ_INIT(&ci_rq->r_ts_queue[i].q_head);

	ci->ci_schedstate.spc_sched_info = ci_rq;
}

/*
 * Control of the runqueue.
 */

static void *
sched_getrq(runqueue_t *ci_rq, const pri_t prio)
{

	KASSERT(prio < PRI_COUNT);
	return (prio <= PRI_HIGHEST_TS) ?
	    &ci_rq->r_ts_queue[prio].q_head :
	    &ci_rq->r_rt_queue[prio - PRI_HIGHEST_TS - 1].q_head;
}

void
sched_enqueue(struct lwp *l, bool swtch)
{
	runqueue_t *ci_rq;
	struct schedstate_percpu *spc;
	TAILQ_HEAD(, lwp) *q_head;
	const pri_t eprio = lwp_eprio(l);
	struct cpu_info *ci;
	int type;

	ci = l->l_cpu;
	spc = &ci->ci_schedstate;
	ci_rq = spc->spc_sched_info;
	KASSERT(lwp_locked(l, l->l_cpu->ci_schedstate.spc_mutex));

	/* Update the last run time on switch */
	if (__predict_true(swtch == true))
		l->l_rticksum += (hardclock_ticks - l->l_rticks);
	else if (l->l_rticks == 0)
		l->l_rticks = hardclock_ticks;

	/* Enqueue the thread */
	q_head = sched_getrq(ci_rq, eprio);
	if (TAILQ_EMPTY(q_head)) {
		u_int i;
		uint32_t q;

		/* Mark bit */
		i = eprio >> BITMAP_SHIFT;
		q = BITMAP_MSB >> (eprio & BITMAP_MASK);
		KASSERT((ci_rq->r_bitmap[i] & q) == 0);
		ci_rq->r_bitmap[i] |= q;
	}
	TAILQ_INSERT_TAIL(q_head, l, l_runq);
	ci_rq->r_count++;
	if ((l->l_pflag & LP_BOUND) == 0)
		ci_rq->r_mcount++;

	/*
	 * Update the value of highest priority in the runqueue,
	 * if priority of this thread is higher.
	 */
	if (eprio > spc->spc_maxpriority)
		spc->spc_maxpriority = eprio;

	sched_newts(l);

	/*
	 * Wake the chosen CPU or cause a preemption if the newly
	 * enqueued thread has higher priority.  Don't cause a 
	 * preemption if the thread is yielding (swtch).
	 */
	if (!swtch && eprio > spc->spc_curpriority) {
		if (eprio >= sched_kpreempt_pri)
			type = RESCHED_KPREEMPT;
		else if (eprio >= sched_upreempt_pri)
			type = RESCHED_IMMED;
		else
			type = 0;
		cpu_need_resched(ci, type);
	}
}

void
sched_dequeue(struct lwp *l)
{
	runqueue_t *ci_rq;
	TAILQ_HEAD(, lwp) *q_head;
	struct schedstate_percpu *spc;
	const pri_t eprio = lwp_eprio(l);

	spc = & l->l_cpu->ci_schedstate;
	ci_rq = spc->spc_sched_info;
	KASSERT(lwp_locked(l, spc->spc_mutex));

	KASSERT(eprio <= spc->spc_maxpriority); 
	KASSERT(ci_rq->r_bitmap[eprio >> BITMAP_SHIFT] != 0);
	KASSERT(ci_rq->r_count > 0);

	ci_rq->r_count--;
	if ((l->l_pflag & LP_BOUND) == 0)
		ci_rq->r_mcount--;

	q_head = sched_getrq(ci_rq, eprio);
	TAILQ_REMOVE(q_head, l, l_runq);
	if (TAILQ_EMPTY(q_head)) {
		u_int i;
		uint32_t q;

		/* Unmark bit */
		i = eprio >> BITMAP_SHIFT;
		q = BITMAP_MSB >> (eprio & BITMAP_MASK);
		KASSERT((ci_rq->r_bitmap[i] & q) != 0);
		ci_rq->r_bitmap[i] &= ~q;

		/*
		 * Update the value of highest priority in the runqueue, in a
		 * case it was a last thread in the queue of highest priority.
		 */
		if (eprio != spc->spc_maxpriority)
			return;

		do {
			if (ci_rq->r_bitmap[i] != 0) {
				q = ffs(ci_rq->r_bitmap[i]);
				spc->spc_maxpriority =
				    (i << BITMAP_SHIFT) + (BITMAP_BITS - q);
				return;
			}
		} while (i--);

		/* If not found - set the lowest value */
		spc->spc_maxpriority = 0;
	}
}

/*
 * Migration and balancing.
 */

#ifdef MULTIPROCESSOR

/* Estimate if LWP is cache-hot */
static inline bool
lwp_cache_hot(const struct lwp *l)
{

	if (l->l_slptime || l->l_rticks == 0)
		return false;

	return (hardclock_ticks - l->l_rticks <= cacheht_time);
}

/* Check if LWP can migrate to the chosen CPU */
static inline bool
sched_migratable(const struct lwp *l, struct cpu_info *ci)
{
	const struct schedstate_percpu *spc = &ci->ci_schedstate;

	/* CPU is offline */
	if (__predict_false(spc->spc_flags & SPCF_OFFLINE))
		return false;

	/* Affinity bind */
	if (__predict_false(l->l_flag & LW_AFFINITY))
		return CPU_ISSET(cpu_index(ci), &l->l_affinity);

	/* Processor-set */
	return (spc->spc_psid == l->l_psid);
}

/*
 * Estimate the migration of LWP to the other CPU.
 * Take and return the CPU, if migration is needed.
 */
struct cpu_info *
sched_takecpu(struct lwp *l)
{
	struct cpu_info *ci, *tci, *first, *next;
	struct schedstate_percpu *spc;
	runqueue_t *ci_rq, *ici_rq;
	pri_t eprio, lpri, pri;

	KASSERT(lwp_locked(l, NULL));

	ci = l->l_cpu;
	spc = &ci->ci_schedstate;
	ci_rq = spc->spc_sched_info;

	/* If thread is strictly bound, do not estimate other CPUs */
	if (l->l_pflag & LP_BOUND)
		return ci;

	/* CPU of this thread is idling - run there */
	if (ci_rq->r_count == 0)
		return ci;

	eprio = lwp_eprio(l);

	/* Stay if thread is cache-hot */
	if (__predict_true(l->l_stat != LSIDL) &&
	    lwp_cache_hot(l) && eprio >= spc->spc_curpriority)
		return ci;

	/* Run on current CPU if priority of thread is higher */
	ci = curcpu();
	spc = &ci->ci_schedstate;
	if (eprio > spc->spc_curpriority && sched_migratable(l, ci))
		return ci;

	/*
	 * Look for the CPU with the lowest priority thread.  In case of
	 * equal priority, choose the CPU with the fewest of threads.
	 */
	first = l->l_cpu;
	ci = first;
	tci = first;
	lpri = PRI_COUNT;
	do {
		next = CIRCLEQ_LOOP_NEXT(&cpu_queue, ci, ci_data.cpu_qchain);
		spc = &ci->ci_schedstate;
		ici_rq = spc->spc_sched_info;
		pri = max(spc->spc_curpriority, spc->spc_maxpriority);
		if (pri > lpri)
			continue;

		if (pri == lpri && ci_rq->r_count < ici_rq->r_count)
			continue;

		if (!sched_migratable(l, ci))
			continue;

		lpri = pri;
		tci = ci;
		ci_rq = ici_rq;
	} while (ci = next, ci != first);

	return tci;
}

/*
 * Tries to catch an LWP from the runqueue of other CPU.
 */
static struct lwp *
sched_catchlwp(void)
{
	struct cpu_info *curci = curcpu(), *ci = worker_ci;
	struct schedstate_percpu *spc;
	TAILQ_HEAD(, lwp) *q_head;
	runqueue_t *ci_rq;
	struct lwp *l;

	if (curci == ci)
		return NULL;

	/* Lockless check */
	spc = &ci->ci_schedstate;
	ci_rq = spc->spc_sched_info;
	if (ci_rq->r_mcount < min_catch)
		return NULL;

	/*
	 * Double-lock the runqueues.
	 */
	if (curci < ci) {
		spc_lock(ci);
	} else if (!mutex_tryenter(ci->ci_schedstate.spc_mutex)) {
		const runqueue_t *cur_rq = curci->ci_schedstate.spc_sched_info;

		spc_unlock(curci);
		spc_lock(ci);
		spc_lock(curci);

		if (cur_rq->r_count) {
			spc_unlock(ci);
			return NULL;
		}
	}

	if (ci_rq->r_mcount < min_catch) {
		spc_unlock(ci);
		return NULL;
	}

	/* Take the highest priority thread */
	q_head = sched_getrq(ci_rq, spc->spc_maxpriority);
	l = TAILQ_FIRST(q_head);

	for (;;) {
		/* Check the first and next result from the queue */
		if (l == NULL)
			break;
		KASSERT(l->l_stat == LSRUN);
		KASSERT(l->l_flag & LW_INMEM);

		/* Look for threads, whose are allowed to migrate */
		if ((l->l_pflag & LP_BOUND) || lwp_cache_hot(l) ||
		    !sched_migratable(l, curci)) {
			l = TAILQ_NEXT(l, l_runq);
			continue;
		}

		/* Grab the thread, and move to the local run queue */
		sched_dequeue(l);
		l->l_cpu = curci;
		lwp_unlock_to(l, curci->ci_schedstate.spc_mutex);
		sched_enqueue(l, false);
		return l;
	}
	spc_unlock(ci);

	return l;
}

/*
 * Periodical calculations for balancing.
 */
static void
sched_balance(void *nocallout)
{
	struct cpu_info *ci, *hci;
	runqueue_t *ci_rq;
	CPU_INFO_ITERATOR cii;
	u_int highest;

	hci = curcpu();
	highest = 0;

	/* Make lockless countings */
	for (CPU_INFO_FOREACH(cii, ci)) {
		ci_rq = ci->ci_schedstate.spc_sched_info;

		/* Average count of the threads */
		ci_rq->r_avgcount = (ci_rq->r_avgcount + ci_rq->r_mcount) >> 1;

		/* Look for CPU with the highest average */
		if (ci_rq->r_avgcount > highest) {
			hci = ci;
			highest = ci_rq->r_avgcount;
		}
	}

	/* Update the worker */
	worker_ci = hci;

	if (nocallout == NULL)
		callout_schedule(&balance_ch, balance_period);
}

#else

struct cpu_info *
sched_takecpu(struct lwp *l)
{

	return l->l_cpu;
}

#endif	/* MULTIPROCESSOR */

/*
 * Scheduling statistics and balancing.
 */
void
sched_lwp_stats(struct lwp *l)
{
	int batch;

	if (l->l_stat == LSSLEEP || l->l_stat == LSSTOP ||
	    l->l_stat == LSSUSPENDED)
		l->l_slptime++;

	/*
	 * Set that thread is more CPU-bound, if sum of run time exceeds the
	 * sum of sleep time.  Check if thread is CPU-bound a first time.
	 */
	batch = (l->l_rticksum > l->l_slpticksum);
	if (batch != 0) {
		if ((l->l_flag & LW_BATCH) == 0)
			batch = 0;
		l->l_flag |= LW_BATCH;
	} else
		l->l_flag &= ~LW_BATCH;

	/*
	 * If thread is CPU-bound and never sleeps, it would occupy the CPU.
	 * In such case reset the value of last sleep, and check it later, if
	 * it is still zero - perform the migration, unmark the batch flag.
	 */
	if (batch && (l->l_slptime + l->l_slpticksum) == 0) {
		if (l->l_slpticks == 0) {
			if (l->l_target_cpu == NULL &&
			    (l->l_stat == LSRUN || l->l_stat == LSONPROC)) {
				struct cpu_info *ci = sched_takecpu(l);
				l->l_target_cpu = (ci != l->l_cpu) ? ci : NULL;
			}
			l->l_flag &= ~LW_BATCH;
		} else {
			l->l_slpticks = 0;
		}
	}

	/* Reset the time sums */
	l->l_slpticksum = 0;
	l->l_rticksum = 0;

	/* Scheduler-specific hook */
	sched_pstats_hook(l, batch);
}

/*
 * Scheduler mill.
 */
struct lwp *
sched_nextlwp(void)
{
	struct cpu_info *ci = curcpu();
	struct schedstate_percpu *spc;
	TAILQ_HEAD(, lwp) *q_head;
	runqueue_t *ci_rq;
	struct lwp *l;

	spc = &ci->ci_schedstate;
	ci_rq = spc->spc_sched_info;

#ifdef MULTIPROCESSOR
	/* If runqueue is empty, try to catch some thread from other CPU */
	if (__predict_false(spc->spc_flags & SPCF_OFFLINE)) {
		if ((ci_rq->r_count - ci_rq->r_mcount) == 0)
			return NULL;
	} else if (ci_rq->r_count == 0) {
		/* Reset the counter, and call the balancer */
		ci_rq->r_avgcount = 0;
		sched_balance(ci);

		/* The re-locking will be done inside */
		return sched_catchlwp();
	}
#else
	if (ci_rq->r_count == 0)
		return NULL;
#endif

	/* Take the highest priority thread */
	KASSERT(ci_rq->r_bitmap[spc->spc_maxpriority >> BITMAP_SHIFT]);
	q_head = sched_getrq(ci_rq, spc->spc_maxpriority);
	l = TAILQ_FIRST(q_head);
	KASSERT(l != NULL);

	sched_oncpu(l);
	l->l_rticks = hardclock_ticks;

	return l;
}

bool
sched_curcpu_runnable_p(void)
{
	const struct cpu_info *ci;
	const struct schedstate_percpu *spc;
	const runqueue_t *ci_rq;
	bool rv;

	kpreempt_disable();
	ci = curcpu();
	spc = &ci->ci_schedstate;
	ci_rq = spc->spc_sched_info;

#ifndef __HAVE_FAST_SOFTINTS
	if (ci->ci_data.cpu_softints) {
		kpreempt_enable();
		return true;
	}
#endif

	if (__predict_false(spc->spc_flags & SPCF_OFFLINE))
		rv = (ci_rq->r_count - ci_rq->r_mcount);
	else
		rv = ci_rq->r_count != 0;
	kpreempt_enable();

	return rv;
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
	sysctl_createv(clog, 0, NULL, &node,
		CTLFLAG_PERMANENT,
		CTLTYPE_NODE, "sched",
		SYSCTL_DESCR("Scheduler options"),
		NULL, 0, NULL, 0,
		CTL_KERN, CTL_CREATE, CTL_EOL);

	if (node == NULL)
		return;

	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_READWRITE,
		CTLTYPE_INT, "cacheht_time",
		SYSCTL_DESCR("Cache hotness time (in ticks)"),
		NULL, 0, &cacheht_time, 0,
		CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_READWRITE,
		CTLTYPE_INT, "balance_period",
		SYSCTL_DESCR("Balance period (in ticks)"),
		NULL, 0, &balance_period, 0,
		CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_READWRITE,
		CTLTYPE_INT, "min_catch",
		SYSCTL_DESCR("Minimal count of threads for catching"),
		NULL, 0, &min_catch, 0,
		CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_READWRITE,
		CTLTYPE_INT, "timesoftints",
		SYSCTL_DESCR("Track CPU time for soft interrupts"),
		NULL, 0, &softint_timing, 0,
		CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_READWRITE,
		CTLTYPE_INT, "kpreempt_pri",
		SYSCTL_DESCR("Minimum priority to trigger kernel preemption"),
		NULL, 0, &sched_kpreempt_pri, 0,
		CTL_CREATE, CTL_EOL);
	sysctl_createv(clog, 0, &node, NULL,
		CTLFLAG_PERMANENT | CTLFLAG_READWRITE,
		CTLTYPE_INT, "upreempt_pri",
		SYSCTL_DESCR("Minimum priority to trigger user preemption"),
		NULL, 0, &sched_upreempt_pri, 0,
		CTL_CREATE, CTL_EOL);
}

/*
 * Debugging.
 */

#ifdef DDB

void
sched_print_runqueue(void (*pr)(const char *, ...)
    __attribute__((__format__(__printf__,1,2))))
{
	runqueue_t *ci_rq;
	struct schedstate_percpu *spc;
	struct lwp *l;
	struct proc *p;
	int i;
	struct cpu_info *ci;
	CPU_INFO_ITERATOR cii;

	for (CPU_INFO_FOREACH(cii, ci)) {
		spc = &ci->ci_schedstate;
		ci_rq = spc->spc_sched_info;

		(*pr)("Run-queue (CPU = %u):\n", ci->ci_index);
		(*pr)(" pid.lid = %d.%d, threads count = %u, "
		    "avgcount = %u, highest pri = %d\n",
#ifdef MULTIPROCESSOR
		    ci->ci_curlwp->l_proc->p_pid, ci->ci_curlwp->l_lid,
#else
		    curlwp->l_proc->p_pid, curlwp->l_lid,
#endif
		    ci_rq->r_count, ci_rq->r_avgcount, spc->spc_maxpriority);
		i = (PRI_COUNT >> BITMAP_SHIFT) - 1;
		do {
			uint32_t q;
			q = ci_rq->r_bitmap[i];
			(*pr)(" bitmap[%d] => [ %d (0x%x) ]\n", i, ffs(q), q);
		} while (i--);
	}

	(*pr)("   %5s %4s %4s %10s %3s %18s %4s %s\n",
	    "LID", "PRI", "EPRI", "FL", "ST", "LWP", "CPU", "LRTIME");

	PROCLIST_FOREACH(p, &allproc) {
		if ((p->p_flag & PK_MARKER) != 0)
			continue;
		(*pr)(" /- %d (%s)\n", (int)p->p_pid, p->p_comm);
		LIST_FOREACH(l, &p->p_lwps, l_sibling) {
			ci = l->l_cpu;
			(*pr)(" | %5d %4u %4u 0x%8.8x %3s %18p %4u %u\n",
			    (int)l->l_lid, l->l_priority, lwp_eprio(l),
			    l->l_flag, l->l_stat == LSRUN ? "RQ" :
			    (l->l_stat == LSSLEEP ? "SQ" : "-"),
			    l, ci->ci_index,
			    (u_int)(hardclock_ticks - l->l_rticks));
		}
	}
}

#endif
