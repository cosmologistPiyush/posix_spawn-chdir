/*	$NetBSD: cpu.h,v 1.3 2002/10/01 11:02:27 bsh Exp $	*/

#if defined(_KERNEL) && !defined(_LKM)

#include "opt_evbarm_boardtype.h"

#define EVBARM_BOARDTYPE_INTEGRATOR	1
#define EVBARM_BOARDTYPE_IQ80310	2
#define EVBARM_BOARDTYPE_I80321		3
#define EVBARM_BOARDTYPE_IXM1200	4
#define EVBARM_BOARDTYPE_PXA2X0	  	5   /* PXA2X0 based boards */
#define EVBARM_BOARDTYPE_S3C2800	6   /* S3C2800 based boards */

#endif

#include <arm/cpu.h>
