/*	$NetBSD: clock.c,v 1.18 2006/03/07 23:08:14 jld Exp $	*/

/*
 *
 * Copyright (c) 2004 Christian Limpach.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christian Limpach.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "opt_xen.h"

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: clock.c,v 1.18 2006/03/07 23:08:14 jld Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/device.h>

#include <machine/xen.h>
#include <machine/hypervisor.h>
#include <machine/evtchn.h>
#include <machine/cpu_counter.h>

#include <dev/clock_subr.h>

#include "config_time.h"		/* for CONFIG_TIME */

/* #define XEN_CLOCK_DEBUG */

static int xen_timer_handler(void *, struct intrframe *);

/* These are peridically updated in shared_info, and then copied here. */
volatile static uint64_t shadow_tsc_stamp;
volatile static uint64_t shadow_system_time;
#ifndef XEN3
volatile static unsigned long shadow_time_version;
#endif
volatile static struct timeval shadow_tv;

static int timeset;

static uint64_t processed_system_time;

#define NS_PER_TICK (1000000000ULL/hz)

/*
 * Reads a consistent set of time-base values from Xen, into a shadow data
 * area.  Must be called at splclock.
 */
static void
get_time_values_from_xen(void)
{
#ifdef XEN3
	volatile struct vcpu_time_info *t =
	    &HYPERVISOR_shared_info->vcpu_info[0].time;
	uint32_t tversion;
	do {
		tversion = t->version;
		__insn_barrier();
		shadow_tsc_stamp = t->tsc_timestamp;
		shadow_system_time = t->system_time;
		__insn_barrier();
	} while ((t->version & 1) || (tversion != t->version));
	do {
		tversion = HYPERVISOR_shared_info->wc_version;
		__insn_barrier();
		shadow_tv.tv_sec = HYPERVISOR_shared_info->wc_sec;
		shadow_tv.tv_usec = HYPERVISOR_shared_info->wc_nsec;
		__insn_barrier();
	} while ((HYPERVISOR_shared_info->wc_version & 1) ||
	    (tversion != HYPERVISOR_shared_info->wc_version));
	shadow_tv.tv_usec = shadow_tv.tv_usec / 1000;
#else /* XEN3 */
	do {
		shadow_time_version = HYPERVISOR_shared_info->time_version2;
		__insn_barrier();
		shadow_tv.tv_sec = HYPERVISOR_shared_info->wc_sec;
		shadow_tv.tv_usec = HYPERVISOR_shared_info->wc_usec;
		shadow_tsc_stamp = HYPERVISOR_shared_info->tsc_timestamp;
		shadow_system_time = HYPERVISOR_shared_info->system_time;
		__insn_barrier();
	} while (shadow_time_version != HYPERVISOR_shared_info->time_version1);
#endif
}

static uint64_t
get_tsc_offset_ns(void)
{
	uint64_t tsc_delta, offset;
	struct cpu_info *ci = curcpu();

	tsc_delta = cpu_counter() - shadow_tsc_stamp;
	offset = tsc_delta * 1000000000ULL / cpu_frequency(ci);
#ifdef XEN_CLOCK_DEBUG
	if (offset > 10 * NS_PER_TICK)
		printf("get_tsc_offset_ns: tsc_delta=%llu offset=%llu\n",
		    tsc_delta, offset); 
#endif
	return offset;
}

void
inittodr(time_t base)
{
	int s;
	struct cpu_info *ci = curcpu();

	/*
	 * if the file system time is more than a year older than the
	 * kernel, warn and then set the base time to the CONFIG_TIME.
	 */
	if (base && base < (CONFIG_TIME-SECYR)) {
		printf("WARNING: preposterous time in file system\n");
		base = CONFIG_TIME;
	}

	s = splclock();
	get_time_values_from_xen();
	splx(s);

	time.tv_usec = shadow_tv.tv_usec;
	time.tv_sec = shadow_tv.tv_sec + rtc_offset * 60;
#ifdef DEBUG_CLOCK
	printf("readclock: %ld (%ld)\n", time.tv_sec, base);
#endif
	/* reset microset, so that the next call to microset() will init */
	ci->ci_cc.cc_denom = 0;

	if (base != 0 && base < time.tv_sec - 5*SECYR)
		printf("WARNING: file system time much less than clock time\n");
	else if (base > time.tv_sec + 5*SECYR) {
		printf("WARNING: clock time much less than file system time\n");
		printf("WARNING: using file system time\n");
		goto fstime;
	}

	timeset = 1;
	return;

fstime:
	timeset = 1;
	time.tv_sec = base;
	printf("WARNING: CHECK AND RESET THE DATE!\n");
}

void
resettodr()
{
#ifdef DOM0OPS
	dom0_op_t op;
	int s;
#endif
#ifdef DEBUG_CLOCK
	struct clock_ymdhms dt;
#endif

	/*
	 * We might have been called by boot() due to a crash early
	 * on.  Don't reset the clock chip in this case.
	 */
	if (!timeset)
		return;

#ifdef DEBUG_CLOCK
	clock_secs_to_ymdhms(time.tv_sec - rtc_offset * 60, &dt);

	printf("setclock: %d/%d/%d %02d:%02d:%02d\n", dt.dt_year,
	    dt.dt_mon, dt.dt_day, dt.dt_hour, dt.dt_min, dt.dt_sec);
#endif
#ifdef DOM0OPS
	if (xen_start_info.flags & SIF_PRIVILEGED) {
		s = splclock();

		op.cmd = DOM0_SETTIME;
		op.u.settime.secs	 = time.tv_sec - rtc_offset * 60;
#ifdef XEN3
		op.u.settime.nsecs	 = time.tv_usec * 1000;
#else
		op.u.settime.usecs	 = time.tv_usec;
#endif
		op.u.settime.system_time = shadow_system_time;
		HYPERVISOR_dom0_op(&op);

		splx(s);
	}
#endif
}

void
startrtclock()
{

}

/*
 * Wait approximately `n' microseconds.
 */
void
xen_delay(int n)
{
	if (n < 500000) {
		/*
		 * shadow_system_time is updated every hz tick, it's not
		 * precise enouth for short delays. Use the CPU counter
		 * instead. We assume it's working at this point.
		 */
		u_int64_t cc, cc2, when;
		struct cpu_info *ci = curcpu();

		cc = cpu_counter();
		when = cc + (u_int64_t)n * cpu_frequency(ci) / 1000000LL;
		if (when < cc) {
			/* wait for counter to wrap */
			cc2 = cpu_counter();
			while (cc2 > cc)
				cc2 = cpu_counter();
		}
		cc2 = cpu_counter();
		while (cc2 < when)
			cc2 = cpu_counter();
		
		return;
	} else {
		uint64_t when;

		/* for large delays, shadow_system_time is OK */
		get_time_values_from_xen();
		when = shadow_system_time + n * 1000;
		while (shadow_system_time < when)
			get_time_values_from_xen();
	}
}

/*
 * A MD microtime for xen.
 *
 * This abuses/reuses the cc_microtime fields already in cpuinfo:
 *  cc_ms_delta = usec added to time(9) on last call to hardclock;
 *                this is used to scale the actual elapsed time 
 *        cc_cc = reference value of cpu_counter()
 *     cc_denom = nsec between last hardclock(9) and time of cc_cc setting
 *                (provided by Xen)
 *
 * We are taking Xen's word for the CPU frequency rather than trying to
 * time it ourselves like cc_microtime does, since Xen could reschedule
 * our virtual CPU(s) onto any physical CPU and only tell us afterwards
 * with a clock interrupt -- and that could invalidate all stored
 * cpu_counter values.
 */
void
xen_microtime(struct timeval *tv)
{
	int s = splclock();
	struct cpu_info *ci = curcpu();
	int64_t cycles;
	
	*tv = time;
	/* Extrapolate from hardclock()'s last step. */
	cycles = cpu_counter() - ci->ci_cc.cc_cc;
	KDASSERT(cycles > 0);
	cycles += ci->ci_cc.cc_denom * cpu_frequency(ci) / 1000000000LL;
	tv->tv_usec += cycles * ci->ci_cc.cc_ms_delta * hz / cpu_frequency(ci);
	KDASSERT(tv->tv_usec < 2000000);
	if (tv->tv_usec >= 1000000) {
		tv->tv_sec++;
		tv->tv_usec -= 1000000;
	}
	/* Avoid backsteps, e.g. at the beginning of a negative adjustment. */
	if (timerisset(&ci->ci_cc.cc_time) &&	
	    timercmp(tv, &ci->ci_cc.cc_time, <)) {
		struct timeval backstep; /* XXX hook resettodr() instead of doing this. */

		timersub(&ci->ci_cc.cc_time, tv, &backstep);
		if (backstep.tv_sec == 0) { /* if it was < 1sec */
			*tv = ci->ci_cc.cc_time;
#ifdef XEN_CLOCK_DEBUG
			printf("xen_microtime[%d]: clamping at %ld.%06ld (-%ldus)\n",
			    (int)ci->ci_cpuid, tv->tv_sec, tv->tv_usec, backstep.tv_usec);
		} else {
			printf("xen_microtime[%d]: allowing large backstep "
			    "%lds to %ld.%06ld\n", (int)ci->ci_cpuid,
			    backstep.tv_sec, tv->tv_sec, tv->tv_usec);
#endif
		}
	}
	ci->ci_cc.cc_time = *tv;
	splx(s);
}

void
xen_initclocks()
{
	int evtch = bind_virq_to_evtch(VIRQ_TIMER);

	aprint_verbose("Xen clock: using event channel %d\n", evtch);

	get_time_values_from_xen();
	processed_system_time = shadow_system_time;

	event_set_handler(evtch, (int (*)(void *))xen_timer_handler,
	    NULL, IPL_CLOCK, "clock");
	hypervisor_enable_event(evtch);
}

static int
xen_timer_handler(void *arg, struct intrframe *regs)
{
	int64_t delta, newcc;
	int ticks_done;
	struct timeval oldtime, elapsed;
	struct cpu_info *ci = curcpu();
	
	get_time_values_from_xen();
	newcc = cpu_counter();

	ticks_done = 0;
	delta = (int64_t)(shadow_system_time + get_tsc_offset_ns()
	    - processed_system_time);
	while (delta >= NS_PER_TICK) {
		/* Have hardclock do its thing. */
		oldtime = time;
		hardclock((struct clockframe *)regs);
		
		/* Use that tick length for the coming tick's microtimes. */
		timersub(&time, &oldtime, &elapsed);
		KDASSERT(elapsed.tv_sec == 0);
		ci->ci_cc.cc_ms_delta = elapsed.tv_usec;

		delta -= NS_PER_TICK;
		processed_system_time += NS_PER_TICK;
		ticks_done++;
	}

	/*
	 * Right now, delta holds the number of ns elapsed from when the last
	 * hardclock(9) allegedly was to when this domain/vcpu was actually
	 * rescheduled.
	 */
	ci->ci_cc.cc_denom = delta;
	ci->ci_cc.cc_cc = newcc;

	return 0;
}

void
setstatclockrate(int arg)
{
}

void
idle_block(void)
{

	/*
	 * We set the timer to when we expect the next timer
	 * interrupt.  We could set the timer to later if we could
	 * easily find out when we will have more work (callouts) to
	 * process from hardclock.
	 */
	if (HYPERVISOR_set_timer_op(processed_system_time + NS_PER_TICK) == 0)
		HYPERVISOR_block();
}
