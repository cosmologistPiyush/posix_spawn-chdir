/* $NetBSD: mach_syscalls.c,v 1.9 2002/12/30 18:44:34 manu Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.4 2002/11/26 08:10:19 manu Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_syscalls.c,v 1.9 2002/12/30 18:44:34 manu Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_ntp.h"
#include "opt_sysv.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/syscallargs.h>
#include <compat/mach/mach_types.h>
#include <compat/mach/mach_message.h>
#include <compat/mach/mach_clock.h>
#include <compat/mach/mach_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const mach_syscallnames[] = {
	"#0 (unimplemented)",		/* 0 = unimplemented */
	"#1 (unimplemented)",		/* 1 = unimplemented */
	"#2 (unimplemented)",		/* 2 = unimplemented */
	"#3 (unimplemented)",		/* 3 = unimplemented */
	"#4 (unimplemented)",		/* 4 = unimplemented */
	"#5 (unimplemented)",		/* 5 = unimplemented */
	"#6 (unimplemented)",		/* 6 = unimplemented */
	"#7 (unimplemented)",		/* 7 = unimplemented */
	"#8 (unimplemented)",		/* 8 = unimplemented */
	"#9 (unimplemented)",		/* 9 = unimplemented */
	"#10 (unimplemented)",		/* 10 = unimplemented */
	"#11 (unimplemented)",		/* 11 = unimplemented */
	"#12 (unimplemented)",		/* 12 = unimplemented */
	"#13 (unimplemented)",		/* 13 = unimplemented */
	"#14 (unimplemented)",		/* 14 = unimplemented */
	"#15 (unimplemented)",		/* 15 = unimplemented */
	"#16 (unimplemented)",		/* 16 = unimplemented */
	"#17 (unimplemented)",		/* 17 = unimplemented */
	"#18 (unimplemented)",		/* 18 = unimplemented */
	"#19 (unimplemented)",		/* 19 = unimplemented */
	"#20 (unimplemented)",		/* 20 = unimplemented */
	"#21 (unimplemented)",		/* 21 = unimplemented */
	"#22 (unimplemented)",		/* 22 = unimplemented */
	"#23 (unimplemented)",		/* 23 = unimplemented */
	"#24 (unimplemented)",		/* 24 = unimplemented */
	"#25 (unimplemented)",		/* 25 = unimplemented */
	"reply_port",			/* 26 = reply_port */
	"thread_self_trap",			/* 27 = thread_self_trap */
	"task_self_trap",			/* 28 = task_self_trap */
	"host_self_trap",			/* 29 = host_self_trap */
	"#30 (unimplemented)",		/* 30 = unimplemented */
	"msg_trap",			/* 31 = msg_trap */
	"msg_overwrite_trap",			/* 32 = msg_overwrite_trap */
	"semaphore_signal_trap",			/* 33 = semaphore_signal_trap */
	"semaphore_signal_all_trap",			/* 34 = semaphore_signal_all_trap */
	"semaphore_signal_thread_trap",			/* 35 = semaphore_signal_thread_trap */
	"semaphore_wait_trap",			/* 36 = semaphore_wait_trap */
	"semaphore_wait_signal_trap",			/* 37 = semaphore_wait_signal_trap */
	"semaphore_timedwait_trap",			/* 38 = semaphore_timedwait_trap */
	"semaphore_timedwait_signal_trap",			/* 39 = semaphore_timedwait_signal_trap */
	"#40 (unimplemented)",		/* 40 = unimplemented */
	"init_process",			/* 41 = init_process */
	"#42 (unimplemented)",		/* 42 = unimplemented */
	"map_fd",			/* 43 = map_fd */
	"#44 (unimplemented)",		/* 44 = unimplemented */
	"task_for_pid",			/* 45 = task_for_pid */
	"pid_for_task",			/* 46 = pid_for_task */
	"#47 (unimplemented)",		/* 47 = unimplemented */
	"macx_swapon",			/* 48 = macx_swapon */
	"macx_swapoff",			/* 49 = macx_swapoff */
	"#50 (unimplemented)",		/* 50 = unimplemented */
	"macx_triggers",			/* 51 = macx_triggers */
	"#52 (unimplemented)",		/* 52 = unimplemented */
	"#53 (unimplemented)",		/* 53 = unimplemented */
	"#54 (unimplemented)",		/* 54 = unimplemented */
	"#55 (unimplemented)",		/* 55 = unimplemented */
	"#56 (unimplemented)",		/* 56 = unimplemented */
	"#57 (unimplemented)",		/* 57 = unimplemented */
	"#58 (unimplemented)",		/* 58 = unimplemented */
	"swtch_pri",			/* 59 = swtch_pri */
	"swtch",			/* 60 = swtch */
	"syscall_thread_switch",			/* 61 = syscall_thread_switch */
	"clock_sleep_trap",			/* 62 = clock_sleep_trap */
	"#63 (unimplemented)",		/* 63 = unimplemented */
	"#64 (unimplemented)",		/* 64 = unimplemented */
	"#65 (unimplemented)",		/* 65 = unimplemented */
	"#66 (unimplemented)",		/* 66 = unimplemented */
	"#67 (unimplemented)",		/* 67 = unimplemented */
	"#68 (unimplemented)",		/* 68 = unimplemented */
	"#69 (unimplemented)",		/* 69 = unimplemented */
	"#70 (unimplemented)",		/* 70 = unimplemented */
	"#71 (unimplemented)",		/* 71 = unimplemented */
	"#72 (unimplemented)",		/* 72 = unimplemented */
	"#73 (unimplemented)",		/* 73 = unimplemented */
	"#74 (unimplemented)",		/* 74 = unimplemented */
	"#75 (unimplemented)",		/* 75 = unimplemented */
	"#76 (unimplemented)",		/* 76 = unimplemented */
	"#77 (unimplemented)",		/* 77 = unimplemented */
	"#78 (unimplemented)",		/* 78 = unimplemented */
	"#79 (unimplemented)",		/* 79 = unimplemented */
	"#80 (unimplemented)",		/* 80 = unimplemented */
	"#81 (unimplemented)",		/* 81 = unimplemented */
	"#82 (unimplemented)",		/* 82 = unimplemented */
	"#83 (unimplemented)",		/* 83 = unimplemented */
	"#84 (unimplemented)",		/* 84 = unimplemented */
	"#85 (unimplemented)",		/* 85 = unimplemented */
	"#86 (unimplemented)",		/* 86 = unimplemented */
	"#87 (unimplemented)",		/* 87 = unimplemented */
	"#88 (unimplemented)",		/* 88 = unimplemented */
	"timebase_info",			/* 89 = timebase_info */
	"wait_until",			/* 90 = wait_until */
	"timer_create",			/* 91 = timer_create */
	"timer_destroy",			/* 92 = timer_destroy */
	"timer_arm",			/* 93 = timer_arm */
	"timer_cancel",			/* 94 = timer_cancel */
	"get_time_base_info",			/* 95 = get_time_base_info */
	"#96 (unimplemented)",		/* 96 = unimplemented */
	"#97 (unimplemented)",		/* 97 = unimplemented */
	"#98 (unimplemented)",		/* 98 = unimplemented */
	"#99 (unimplemented)",		/* 99 = unimplemented */
	"#100 (unimplemented)",		/* 100 = unimplemented */
	"#101 (unimplemented)",		/* 101 = unimplemented */
	"#102 (unimplemented)",		/* 102 = unimplemented */
	"#103 (unimplemented)",		/* 103 = unimplemented */
	"#104 (unimplemented)",		/* 104 = unimplemented */
	"#105 (unimplemented)",		/* 105 = unimplemented */
	"#106 (unimplemented)",		/* 106 = unimplemented */
	"#107 (unimplemented)",		/* 107 = unimplemented */
	"#108 (unimplemented)",		/* 108 = unimplemented */
	"#109 (unimplemented)",		/* 109 = unimplemented */
	"#110 (unimplemented)",		/* 110 = unimplemented */
	"#111 (unimplemented)",		/* 111 = unimplemented */
	"#112 (unimplemented)",		/* 112 = unimplemented */
	"#113 (unimplemented)",		/* 113 = unimplemented */
	"#114 (unimplemented)",		/* 114 = unimplemented */
	"#115 (unimplemented)",		/* 115 = unimplemented */
	"#116 (unimplemented)",		/* 116 = unimplemented */
	"#117 (unimplemented)",		/* 117 = unimplemented */
	"#118 (unimplemented)",		/* 118 = unimplemented */
	"#119 (unimplemented)",		/* 119 = unimplemented */
	"#120 (unimplemented)",		/* 120 = unimplemented */
	"#121 (unimplemented)",		/* 121 = unimplemented */
	"#122 (unimplemented)",		/* 122 = unimplemented */
	"#123 (unimplemented)",		/* 123 = unimplemented */
	"#124 (unimplemented)",		/* 124 = unimplemented */
	"#125 (unimplemented)",		/* 125 = unimplemented */
	"#126 (unimplemented)",		/* 126 = unimplemented */
	"#127 (unimplemented)",		/* 127 = unimplemented */
};
