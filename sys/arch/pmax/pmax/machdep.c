/*	$NetBSD: machdep.c,v 1.35 1995/09/01 20:06:15 mycroft Exp $	*/

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department, The Mach Operating System project at
 * Carnegie-Mellon University and Ralph Campbell.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)machdep.c	8.3 (Berkeley) 1/12/94
 */

/* from: Utah Hdr: machdep.c 1.63 91/04/24 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signalvar.h>
#include <sys/kernel.h>
#include <sys/map.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/reboot.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/clist.h>
#include <sys/callout.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/msgbuf.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/user.h>
#include <sys/exec.h>
#include <sys/sysctl.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#ifdef SYSVSHM
#include <sys/shm.h>
#endif

#include <vm/vm_kern.h>

#include <machine/cpu.h>
#include <machine/reg.h>
#include <machine/psl.h>
#include <machine/pte.h>
#include <machine/dc7085cons.h>

#include <pmax/stand/dec_prom.h>

#include <pmax/dev/device.h>
#include <pmax/dev/sccreg.h>
#include <pmax/dev/ascreg.h>

#include <pmax/pmax/clockreg.h>
#include <pmax/pmax/kn01.h>
#include <pmax/pmax/kn02.h>
#include <pmax/pmax/kmin.h>
#include <pmax/pmax/maxine.h>
#include <pmax/pmax/kn03.h>
#include <pmax/pmax/asic.h>
#include <pmax/pmax/turbochannel.h>
#include <pmax/pmax/pmaxtype.h>
#include <pmax/pmax/cons.h>

#include <pm.h>
#include <cfb.h>
#include <mfb.h>
#include <xcfb.h>
#include <sfb.h>
#include <dc.h>
#include <dtop.h>
#include <scc.h>
#include <le.h>
#include <asc.h>

#include <pmax/dev/sccvar.h>
#include <pmax/dev/dcvar.h>

#if NDTOP > 0
extern int dtopKBDGetc();
#endif


extern int KBDGetc();
extern void fbPutc();
extern struct consdev cn_tab;

/* Will scan from max to min, inclusive */
static int tc_max_slot = KN02_TC_MAX;
static int tc_min_slot = KN02_TC_MIN;
static u_int tc_slot_phys_base [TC_MAX_SLOTS] = {
	/* use 3max for default values */
	KN02_PHYS_TC_0_START, KN02_PHYS_TC_1_START,
	KN02_PHYS_TC_2_START, KN02_PHYS_TC_3_START,
	KN02_PHYS_TC_4_START, KN02_PHYS_TC_5_START,
	KN02_PHYS_TC_6_START, KN02_PHYS_TC_7_START
};

/* the following is used externally (sysctl_hw) */
char	machine[] = "pmax";	/* cpu "architecture" */
char	cpu_model[30];

vm_map_t buffer_map;

/*
 * Declare these as initialized data so we can patch them.
 */
int	nswbuf = 0;
#ifdef	NBUF
int	nbuf = NBUF;
#else
int	nbuf = 0;
#endif
#ifdef	BUFPAGES
int	bufpages = BUFPAGES;
#else
int	bufpages = 0;
#endif
int	msgbufmapped = 0;	/* set when safe to use msgbuf */
int	maxmem;			/* max memory per process */
int	physmem;		/* max supported memory, changes to actual */
int	pmax_boardtype;		/* Mother board type */
u_long	le_iomem;		/* 128K for lance chip via. ASIC */
u_long	asc_iomem;		/* and 7 * 8K buffers for the scsi */
u_long	asic_base;		/* Base address of I/O asic */
const	struct callback *callv;	/* pointer to PROM entry points */

extern void	(*tc_enable_interrupt)  __P ((u_int slotno,
					      void (*handler)(int unit),
					      int unit, int onoff)); 
void	(*tc_enable_interrupt) __P ((u_int slotno, void (*handler)(int unit),
				     int unit, int onoff));
extern	int (*pmax_hardware_intr)();

int	kn02_intr(), kmin_intr(), xine_intr(), pmax_intr();
#ifdef DS5000_240
int	kn03_intr();
#endif
extern	int Mach_spl0(), Mach_spl1(), Mach_spl2(), Mach_spl3(), splhigh();
int	(*Mach_splbio)() = splhigh;
int	(*Mach_splnet)() = splhigh;
int	(*Mach_spltty)() = splhigh;
int	(*Mach_splimp)() = splhigh;
int	(*Mach_splclock)() = splhigh;
int	(*Mach_splstatclock)() = splhigh;
extern	volatile struct chiptime *Mach_clock_addr;
u_long	kmin_tc3_imask, xine_tc3_imask;
#ifdef DS5000_240
u_long	kn03_tc3_imask;
#endif
tc_option_t tc_slot_info[TC_MAX_LOGICAL_SLOTS];
static	void asic_init();
extern	void RemconsInit();

#ifdef DS5000

#if 1 /*def DS5000_200*/
void	kn02_enable_intr  __P ((u_int slotno, void (*handler)(),
				int unit, int onoff));
#endif /*def DS5000_200*/

#ifdef DS5000_100
void kmin_enable_intr  __P ((u_int slotno, void (*handler)(),
			     int unit, int onoff));
#endif /*DS5000_100*/

#ifdef DS5000_25
void xine_enable_intr __P ((u_int slotno, void (*handler)(),
			    int unit, int onoff));
#endif /*DS5000_25*/

#ifdef DS5000_240
void	kn03_enable_intr __P ((u_int slotno, void (*handler)(),
			       int unit, int onoff));
#endif /*DS5000_240*/

volatile u_int *Mach_reset_addr;
#endif /* DS5000 */


/*
 * safepri is a safe priority for sleep to set for a spin-wait
 * during autoconfiguration or after a panic.
 */
int	safepri = PSL_LOWIPL;

struct	user *proc0paddr;
struct	proc nullproc;		/* for use by swtch_exit() */

/*
 * Do all the stuff that locore normally does before calling main().
 * Process arguments passed to us by the prom monitor.
 * Return the first page address following the system.
 */
mach_init(argc, argv, code, cv)
	int argc;
	char *argv[];
	u_int code;
	const struct callback *cv;
{
	register char *cp;
	register int i;
	register unsigned firstaddr;
	register caddr_t v;
	caddr_t start;
	extern char edata[], end[];
	extern char MachUTLBMiss[], MachUTLBMissEnd[];
	extern char MachException[], MachExceptionEnd[];

	/* clear the BSS segment */
	v = (caddr_t)pmax_round_page(end);
	bzero(edata, v - edata);

	/* Initialize callv so we can do PROM output... */
	if (code == DEC_PROM_MAGIC) {
		callv = cv;
	} else {
		callv = &callvec;
	}

	/* check for direct boot from DS5000 PROM */
	if (argc > 0 && strcmp(argv[0], "boot") == 0) {
		argc--;
		argv++;
	}

	/* look at argv[0] and compute bootdev */
	makebootdev(argv[0]);

	/*
	 * Look at arguments passed to us and compute boothowto.
	 */
#ifdef GENERIC
	boothowto = RB_SINGLE | RB_ASKNAME;
#else
	boothowto = RB_SINGLE;
#endif
#ifdef KADB
	boothowto |= RB_KDB;
#endif
	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			for (cp = argv[i]; *cp; cp++) {
				switch (*cp) {
				case 'a': /* autoboot */
					boothowto &= ~RB_SINGLE;
					break;

				case 'd': /* use compiled in default root */
					boothowto |= RB_DFLTROOT;
					break;

				case 'm': /* mini root present in memory */
					boothowto |= RB_MINIROOT;
					break;

				case 'n': /* ask for names */
					boothowto |= RB_ASKNAME;
					break;

				case 'N': /* don't ask for names */
					boothowto &= ~RB_ASKNAME;
				}
			}
		}
	}

#ifdef MFS
	/*
	 * Check to see if a mini-root was loaded into memory. It resides
	 * at the start of the next page just after the end of BSS.
	 */
	if (boothowto & RB_MINIROOT) {
		boothowto |= RB_DFLTROOT;
		v += mfs_initminiroot(v);
	}
#endif

	/*
	 * Init mapping for u page(s) for proc[0], pm_tlbpid 1.
	 */
	start = v;
	curproc->p_addr = proc0paddr = (struct user *)v;
	curproc->p_md.md_regs = proc0paddr->u_pcb.pcb_regs;
	firstaddr = MACH_CACHED_TO_PHYS(v);
	for (i = 0; i < UPAGES; i++) {
		MachTLBWriteIndexed(i,
			(UADDR + (i << PGSHIFT)) | (1 << VMMACH_TLB_PID_SHIFT),
			curproc->p_md.md_upte[i] = firstaddr | PG_V | PG_M);
		firstaddr += NBPG;
	}
	v += UPAGES * NBPG;
	MachSetPID(1);

	/*
	 * init nullproc for swtch_exit().
	 * init mapping for u page(s), pm_tlbpid 0
	 * This could be used for an idle process.
	 */
	nullproc.p_addr = (struct user *)v;
	nullproc.p_md.md_regs = nullproc.p_addr->u_pcb.pcb_regs;
	bcopy("nullproc", nullproc.p_comm, sizeof("nullproc"));
	for (i = 0; i < UPAGES; i++) {
		nullproc.p_md.md_upte[i] = firstaddr | PG_V | PG_M;
		firstaddr += NBPG;
	}
	v += UPAGES * NBPG;

	/* clear pages for u areas */
	bzero(start, v - start);

	/*
	 * Copy down exception vector code.
	 */
	if (MachUTLBMissEnd - MachUTLBMiss > 0x80)
		panic("startup: UTLB code too large");
	bcopy(MachUTLBMiss, (char *)MACH_UTLB_MISS_EXC_VEC,
		MachUTLBMissEnd - MachUTLBMiss);
	bcopy(MachException, (char *)MACH_GEN_EXC_VEC,
		MachExceptionEnd - MachException);

	/*
	 * Clear out the I and D caches.
	 */
	MachConfigCache();
	MachFlushCache();

	/*
	 * Determine what model of computer we are running on.
	 */
	if (code == DEC_PROM_MAGIC) {
		i = (*cv->_getsysid)();
		cp = "";
	} else {
		if (cp = (*callv->_getenv)("systype"))
			i = atoi(cp);
		else {
			cp = "";
			i = 0;
		}
	}
	/* check for MIPS based platform */
	if (((i >> 24) & 0xFF) != 0x82) {
		printf("Unknown System type '%s' 0x%x\n", cp, i);
		boot(RB_HALT | RB_NOSYNC);
	}

	/* check what model platform we are running on */
	pmax_boardtype = ((i >> 16) & 0xff);

	switch (pmax_boardtype) {
	case DS_PMAX:	/* DS3100 Pmax */
		/*
		 * Set up interrupt handling and I/O addresses.
		 */
		pmax_hardware_intr = pmax_intr;
		Mach_splbio = Mach_spl0;
		Mach_splnet = Mach_spl1;
		Mach_spltty = Mach_spl2;
		Mach_splimp = Mach_spl2;
		Mach_splclock = Mach_spl3;
		Mach_splstatclock = Mach_spl3;
		Mach_clock_addr = (volatile struct chiptime *)
			MACH_PHYS_TO_UNCACHED(KN01_SYS_CLOCK);
		strcpy(cpu_model, "3100");
		break;

#ifdef DS5000
	case DS_3MAX:	/* DS5000/200 3max */
		{
		volatile int *csr_addr =
			(volatile int *)MACH_PHYS_TO_UNCACHED(KN02_SYS_CSR);

		Mach_reset_addr =
		    (unsigned *)MACH_PHYS_TO_UNCACHED(KN02_SYS_ERRADR);
		/* clear any memory errors from new-config probes */
		*Mach_reset_addr = 0;

		/*
		 * Enable ECC memory correction, turn off LEDs, and
		 * disable all TURBOchannel interrupts.
		 */
		i = *csr_addr;
		*csr_addr = (i & ~(KN02_CSR_WRESERVED | KN02_CSR_IOINTEN)) |
			KN02_CSR_CORRECT | 0xff;
		pmax_hardware_intr = kn02_intr;
		tc_enable_interrupt = kn02_enable_intr;
		Mach_splbio = Mach_spl0;
		Mach_splnet = Mach_spl0;
		Mach_spltty = Mach_spl0;
		Mach_splimp = Mach_spl0;
		Mach_splclock = Mach_spl1;
		Mach_splstatclock = Mach_spl1;
		Mach_clock_addr = (volatile struct chiptime *)
			MACH_PHYS_TO_UNCACHED(KN02_SYS_CLOCK);

		}
		strcpy(cpu_model, "5000/200");
		break;

#ifdef DS5000_100
	case DS_3MIN:	/* DS5000/1xx 3min */
		tc_max_slot = KMIN_TC_MAX;
		tc_min_slot = KMIN_TC_MIN;
		tc_slot_phys_base[0] = KMIN_PHYS_TC_0_START;
		tc_slot_phys_base[1] = KMIN_PHYS_TC_1_START;
		tc_slot_phys_base[2] = KMIN_PHYS_TC_2_START;
		asic_base = MACH_PHYS_TO_UNCACHED(KMIN_SYS_ASIC);
		pmax_hardware_intr = kmin_intr;
		tc_enable_interrupt = kmin_enable_intr;
		kmin_tc3_imask = (KMIN_INTR_CLOCK | KMIN_INTR_PSWARN |
			KMIN_INTR_TIMEOUT);

		/*
		 * Since all the motherboard interrupts come through the
		 * I/O ASIC, it has to be turned off for all the spls and
		 * since we don't know what kinds of devices are in the
		 * turbochannel option slots, just splhigh().
		 */
		Mach_splbio = splhigh;
		Mach_splnet = splhigh;
		Mach_spltty = splhigh;
		Mach_splimp = splhigh;
		Mach_splclock = splhigh;
		Mach_splstatclock = splhigh;
		Mach_clock_addr = (volatile struct chiptime *)
			MACH_PHYS_TO_UNCACHED(KMIN_SYS_CLOCK);


		/*
		 * Initialize interrupts.
		 */
		*(u_int *)ASIC_REG_IMSK(asic_base) = KMIN_IM0;
		*(u_int *)ASIC_REG_INTR(asic_base) = 0;

		/* clear any memory errors from probes */
		Mach_reset_addr =
		    (u_int*)MACH_PHYS_TO_UNCACHED(KMIN_REG_TIMEOUT);
		(*Mach_reset_addr) = 0;

		strcpy(cpu_model, "5000/1xx");
		break;

#endif /* ds5000_100 */

#ifdef DS5000_25
	case DS_MAXINE:	/* DS5000/xx maxine */
		tc_max_slot = XINE_TC_MAX;
		tc_min_slot = XINE_TC_MIN;
		tc_slot_phys_base[0] = XINE_PHYS_TC_0_START;
		tc_slot_phys_base[1] = XINE_PHYS_TC_1_START;
		asic_base = MACH_PHYS_TO_UNCACHED(XINE_SYS_ASIC);
		pmax_hardware_intr = xine_intr;
		tc_enable_interrupt = xine_enable_intr;
		Mach_splbio = Mach_spl3;
		Mach_splnet = Mach_spl3;
		Mach_spltty = Mach_spl3;
		Mach_splimp = Mach_spl3;
		Mach_splclock = Mach_spl1;
		Mach_splstatclock = Mach_spl1;
		Mach_clock_addr = (volatile struct chiptime *)
			MACH_PHYS_TO_UNCACHED(XINE_SYS_CLOCK);

		/*
		 * Initialize interrupts.
		 */
		*(u_int *)ASIC_REG_IMSK(asic_base) = XINE_IM0;
		*(u_int *)ASIC_REG_INTR(asic_base) = 0;
		/* clear any memory errors from probes */
		Mach_reset_addr =
		    (u_int*)MACH_PHYS_TO_UNCACHED(XINE_REG_TIMEOUT);
		(*Mach_reset_addr) = 0;
		strcpy(cpu_model, "5000/25");
		break;
#endif /*DS5000_25*/

#ifdef DS5000_240
	case DS_3MAXPLUS:	/* DS5000/240 3max+ */
		tc_max_slot = KN03_TC_MAX;
		tc_min_slot = KN03_TC_MIN;
		tc_slot_phys_base[0] = KN03_PHYS_TC_0_START;
		tc_slot_phys_base[1] = KN03_PHYS_TC_1_START;
		tc_slot_phys_base[2] = KN03_PHYS_TC_2_START;
		asic_base = MACH_PHYS_TO_UNCACHED(KN03_SYS_ASIC);
		pmax_hardware_intr = kn03_intr;
		tc_enable_interrupt = kn03_enable_intr;
		Mach_reset_addr =
		    (u_int *)MACH_PHYS_TO_UNCACHED(KN03_SYS_ERRADR);
		*Mach_reset_addr = 0;

		/*
		 * Reset interrupts, clear any errors from newconf probes
		 */

		Mach_splbio = Mach_spl0;
		Mach_splnet = Mach_spl0;
		Mach_spltty = Mach_spl0;
		Mach_splimp = Mach_spl0;
		Mach_splclock = Mach_spl1;
		Mach_splstatclock = Mach_spl1;
		Mach_clock_addr = (volatile struct chiptime *)
			MACH_PHYS_TO_UNCACHED(KN03_SYS_CLOCK);

		/*
		 * Initialize interrupts.
		 */
		kn03_tc3_imask = KN03_IM0 &
			~(KN03_INTR_TC_0|KN03_INTR_TC_1|KN03_INTR_TC_2);
		*(u_int *)ASIC_REG_IMSK(asic_base) = kn03_tc3_imask;
		*(u_int *)ASIC_REG_INTR(asic_base) = 0;

		/* clear any memory errors from probes */
		*Mach_reset_addr = 0;
		strcpy(cpu_model, "5000/240");
		break;
#endif /* DS5000_240 */
#endif /* DS5000 */

	default:
		printf("kernel not configured for systype 0x%x\n", i);
		boot(RB_HALT | RB_NOSYNC);
	}

	/*
	 * Find out how much memory is available.
	 * Be careful to save and restore the original contents for msgbuf.
	 */
	physmem = btoc((vm_offset_t)v - KERNBASE);
	cp = (char *)MACH_PHYS_TO_UNCACHED(physmem << PGSHIFT);
	while (cp < (char *)MACH_MAX_MEM_ADDR) {
		if (badaddr(cp, 4))
			break;
		i = *(int *)cp;
		*(int *)cp = 0xa5a5a5a5;
		/*
		 * Data will persist on the bus if we read it right away.
		 * Have to be tricky here.
		 */
		((int *)cp)[4] = 0x5a5a5a5a;
		MachEmptyWriteBuffer();
		if (*(int *)cp != 0xa5a5a5a5)
			break;
		*(int *)cp = i;
		cp += NBPG;
		physmem++;
	}

	maxmem = physmem;

#if NLE > 0
	/*
	 * Grab 128K at the top of physical memory for the lance chip
	 * on machines where it does dma through the I/O ASIC.
	 * It must be physically contiguous and aligned on a 128K boundary.
	 */
	if (pmax_boardtype == DS_3MIN || pmax_boardtype == DS_MAXINE ||
		pmax_boardtype == DS_3MAXPLUS) {
		maxmem -= btoc(128 * 1024);
		le_iomem = (maxmem << PGSHIFT);
	}
#endif /* NLE */
#if NASC > 0
	/*
	 * Ditto for the scsi chip. There is probably a way to make asc.c
	 * do dma without these buffers, but it would require major
	 * re-engineering of the asc driver.
	 * They must be 8K in size and page aligned.
	 */
	if (pmax_boardtype == DS_3MIN || pmax_boardtype == DS_MAXINE ||
		pmax_boardtype == DS_3MAXPLUS) {
		maxmem -= btoc(ASC_NCMD * 8192);
		asc_iomem = (maxmem << PGSHIFT);
	}
#endif /* NASC */

	/*
	 * Initialize error message buffer (at end of core).
	 */
	maxmem -= btoc(sizeof (struct msgbuf));
	msgbufp = (struct msgbuf *)(MACH_PHYS_TO_CACHED(maxmem << PGSHIFT));
	msgbufmapped = 1;

	/*
	 * Allocate space for system data structures.
	 * The first available kernel virtual address is in "v".
	 * As pages of kernel virtual memory are allocated, "v" is incremented.
	 *
	 * These data structures are allocated here instead of cpu_startup()
	 * because physical memory is directly addressable. We don't have
	 * to map these into virtual address space.
	 */
	start = v;

#define	valloc(name, type, num) \
	    (name) = (type *)v; v = (caddr_t)((name)+(num))
#define	valloclim(name, type, num, lim) \
	    (name) = (type *)v; v = (caddr_t)((lim) = ((name)+(num)))
#ifdef REAL_CLISTS
	valloc(cfree, struct cblock, nclist);
#endif
	valloc(callout, struct callout, ncallout);
	valloc(swapmap, struct map, nswapmap = maxproc * 2);
#ifdef SYSVSHM
	valloc(shmsegs, struct shmid_ds, shminfo.shmmni);
#endif

	/*
	 * Determine how many buffers to allocate.
	 * We allocate more buffer space than the BSD standard of
	 * using 10% of memory for the first 2 Meg, 5% of remaining.
	 * We just allocate a flat 10%.  Ensure a minimum of 16 buffers.
	 * We allocate 1/2 as many swap buffer headers as file i/o buffers.
	 */
	if (bufpages == 0)
		bufpages = physmem / 10 / CLSIZE;
	if (nbuf == 0) {
		nbuf = bufpages;
		if (nbuf < 16)
			nbuf = 16;
	}
	if (nswbuf == 0) {
		nswbuf = (nbuf / 2) &~ 1;	/* force even */
		if (nswbuf > 256)
			nswbuf = 256;		/* sanity */
	}
	valloc(swbuf, struct buf, nswbuf);
	valloc(buf, struct buf, nbuf);

	/*
	 * Clear allocated memory.
	 */
	bzero(start, v - start);

	/*
	 * Initialize the virtual memory system.
	 */
	pmap_bootstrap((vm_offset_t)v);

}

/*
 * Gross hack for identifying slots with potential
 * console devices.
 */

int framebuffer_in[15];

void
framebuffer_in_slot(slot)
	int slot;
{
	if (slot > 15) 
	    panic("Framebuffer in slot %d: impossible\n", slot);
	framebuffer_in[slot] = 1;
}

int kbd, crt;
char *oscon;

/*
 * Console initialization: called early on from main,
 * before vm init or startup.  Do enough configuration
 * to choose and initialize a console.
 */
consinit()
{
	/*
	 * First get the "osconsole" environment variable.
	 */
	oscon = (*callv->_getenv)("osconsole");
	crt = kbd = -1;
	if (oscon && *oscon >= '0' && *oscon <= '9') {
		kbd = *oscon - '0';
		cn_tab.cn_screen = 0;
		while (*++oscon) {
			if (*oscon == ',')
				cn_tab.cn_screen = 1;
			else if (cn_tab.cn_screen &&
			    *oscon >= '0' && *oscon <= '9') {
				crt = kbd;
				kbd = *oscon - '0';
				break;
			}
		}
	}
	/* we can't do anything until auto-configuration
	 * has run, and that requires kmalloc(), which
	 * hasn't been initialized yet.  Just keep using
	 * whatever the PROM vector gave us.
	 */
}

/*
 * Do console initialization
 */
xconsinit()
{

#ifdef DEBUG
/*XXX*/			printf("xconsinit: Looking for console device\n");
#endif

	if (pmax_boardtype == DS_PMAX && kbd == 1)
		cn_tab.cn_screen = 1;
	/*
	 * The boot program uses PMAX ROM entrypoints so the ROM sets
	 * osconsole to '1' like the PMAX.
	 */
	if (pmax_boardtype == DS_3MAX && crt == -1 && kbd == 1) {
		cn_tab.cn_screen = 1;
		crt = 0;
		kbd = 7;
	}

	/*
	 * First try the keyboard/crt cases then fall through to the
	 * remote serial lines.
	 */
	if (cn_tab.cn_screen) {
	    switch (pmax_boardtype) {
	    case DS_PMAX:
#if NDC > 0 && NPM > 0
		if (pminit()) {
			cn_tab.cn_dev = makedev(DCDEV, DCKBD_PORT);
			cn_tab.cn_getc = KBDGetc;
			cn_tab.cn_kbdgetc = dcGetc;
			cn_tab.cn_putc = fbPutc;
			cn_tab.cn_disabled = 0;
			return;
		}
#endif /* NDC and NPM */
		goto remcons;

	    case DS_MAXINE:
#if NDTOP > 0
		if (kbd == 3) {
			cn_tab.cn_dev = makedev(DTOPDEV, 0);
			cn_tab.cn_getc = dtopKBDGetc;
			cn_tab.cn_putc = fbPutc;
		} else
#endif /* NDTOP */
			goto remcons;
#if NXCFB > 0
		if (crt == 3 && xcfbinit()) {
			cn_tab.cn_disabled = 0;
			return;
		}
#endif /* XCFB */
		break;

	    case DS_3MAX:
#if NDC > 0
		if (kbd == 7) {
			cn_tab.cn_dev = makedev(DCDEV, DCKBD_PORT);
			cn_tab.cn_getc = KBDGetc;
			cn_tab.cn_kbdgetc = dcGetc;
			cn_tab.cn_putc = fbPutc;
		} else
#endif /* NDC */
			goto remcons;
		break;

	    case DS_3MIN:
	    case DS_3MAXPLUS:
#if NSCC > 0
		if (kbd == 3) {
			cn_tab.cn_dev = makedev(SCCDEV, SCCKBD_PORT);
			cn_tab.cn_getc = KBDGetc;
			cn_tab.cn_kbdgetc = sccGetc;
			cn_tab.cn_putc = fbPutc;
		} else
#endif /* NSCC */
			goto remcons;
		break;

	    default:
		goto remcons;
	    };

	    /*
	     * Check for a suitable turbochannel frame buffer.
	     */
	if (framebuffer_in[crt]) {
			cn_tab.cn_disabled = 0;
			return;
	    } else
		printf("No crt console device in slot %d\n", crt);
	}
remcons:
	/*
	 * Configure a serial port as a remote console.
	 */
	cn_tab.cn_screen = 0;
	switch (pmax_boardtype) {
	case DS_PMAX:
#if NDC > 0
		if (kbd == 4)
			cn_tab.cn_dev = makedev(DCDEV, DCCOMM_PORT);
		else
			cn_tab.cn_dev = makedev(DCDEV, DCPRINTER_PORT);
		cn_tab.cn_getc = dcGetc;
		cn_tab.cn_putc = dcPutc;
#endif /* NDC */
		break;

	case DS_3MAX:
#if NDC > 0
		cn_tab.cn_dev = makedev(DCDEV, DCPRINTER_PORT);
		cn_tab.cn_getc = dcGetc;
		cn_tab.cn_putc = dcPutc;
#endif /* NDC */
		break;

	case DS_3MIN:
	case DS_3MAXPLUS:
#if NSCC > 0
		cn_tab.cn_dev = makedev(SCCDEV, SCCCOMM3_PORT);
		cn_tab.cn_getc = sccGetc;
		cn_tab.cn_putc = sccPutc;
#endif /* NSCC */
		break;

	case DS_MAXINE:
#if NSCC > 0
		cn_tab.cn_dev = makedev(SCCDEV, SCCCOMM2_PORT);
		cn_tab.cn_getc = sccGetc;
		cn_tab.cn_putc = sccPutc;
#endif /* NSCC */
		break;
	};
	if (cn_tab.cn_dev == NODEV)
		printf("Can't configure console!\n");
}

/*
 * cpu_startup: allocate memory for variable-sized tables,
 * initialize cpu, and do autoconfiguration.
 */
cpu_startup()
{
	register unsigned i;
	register caddr_t v;
	int base, residual;
	vm_offset_t minaddr, maxaddr;
	vm_size_t size;
#ifdef DEBUG
	extern int pmapdebug;
	int opmapdebug = pmapdebug;

	pmapdebug = 0;
#endif

	/*
	 * Good {morning,afternoon,evening,night}.
	 */
	printf(version);
	printf("real mem = %d\n", ctob(physmem));

	/*
	 * Allocate virtual address space for file I/O buffers.
	 * Note they are different than the array of headers, 'buf',
	 * and usually occupy more virtual memory than physical.
	 */
	size = MAXBSIZE * nbuf;
	buffer_map = kmem_suballoc(kernel_map, (vm_offset_t *)&buffers,
				   &maxaddr, size, TRUE);
	minaddr = (vm_offset_t)buffers;
	if (vm_map_find(buffer_map, vm_object_allocate(size), (vm_offset_t)0,
			&minaddr, size, FALSE) != KERN_SUCCESS)
		panic("startup: cannot allocate buffers");
	base = bufpages / nbuf;
	residual = bufpages % nbuf;
	for (i = 0; i < nbuf; i++) {
		vm_size_t curbufsize;
		vm_offset_t curbuf;

		/*
		 * First <residual> buffers get (base+1) physical pages
		 * allocated for them.  The rest get (base) physical pages.
		 *
		 * The rest of each buffer occupies virtual space,
		 * but has no physical memory allocated for it.
		 */
		curbuf = (vm_offset_t)buffers + i * MAXBSIZE;
		curbufsize = CLBYTES * (i < residual ? base+1 : base);
		vm_map_pageable(buffer_map, curbuf, curbuf+curbufsize, FALSE);
		vm_map_simplify(buffer_map, curbuf);
	}
	/*
	 * Allocate a submap for exec arguments.  This map effectively
	 * limits the number of processes exec'ing at any time.
	 */
	exec_map = kmem_suballoc(kernel_map, &minaddr, &maxaddr,
				 16 * NCARGS, TRUE);
	/*
	 * Allocate a submap for physio
	 */
	phys_map = kmem_suballoc(kernel_map, &minaddr, &maxaddr,
				 VM_PHYS_SIZE, TRUE);

	/*
	 * Finally, allocate mbuf pool.  Since mclrefcnt is an off-size
	 * we use the more space efficient malloc in place of kmem_alloc.
	 */
	mclrefcnt = (char *)malloc(NMBCLUSTERS+CLBYTES/MCLBYTES,
				   M_MBUF, M_NOWAIT);
	bzero(mclrefcnt, NMBCLUSTERS+CLBYTES/MCLBYTES);
	mb_map = kmem_suballoc(kernel_map, (vm_offset_t *)&mbutl, &maxaddr,
			       VM_MBUF_SIZE, FALSE);
	/*
	 * Initialize callouts
	 */
	callfree = callout;
	for (i = 1; i < ncallout; i++)
		callout[i-1].c_next = &callout[i];
	callout[i-1].c_next = NULL;

#ifdef DEBUG
	pmapdebug = opmapdebug;
#endif
	printf("avail mem = %d\n", ptoa(cnt.v_free_count));
	printf("using %d buffers containing %d bytes of memory\n",
		nbuf, bufpages * CLBYTES);

	/*
	 * Set up buffers, so they can be used to read disk labels.
	 */
	bufinit();

	/*
	 * Set up CPU-specific registers, cache, etc.
	 */
	initcpu();

	/*
	 * Configure the system.
	 */
	configure();
}

/*
 * machine dependent system variables.
 */
cpu_sysctl(name, namelen, oldp, oldlenp, newp, newlen, p)
	int *name;
	u_int namelen;
	void *oldp;
	size_t *oldlenp;
	void *newp;
	size_t newlen;
	struct proc *p;
{

	/* all sysctl names at this level are terminal */
	if (namelen != 1)
		return (ENOTDIR);		/* overloaded */

	switch (name[0]) {
	case CPU_CONSDEV:
		return (sysctl_rdstruct(oldp, oldlenp, newp, &cn_tab.cn_dev,
		    sizeof cn_tab.cn_dev));
	default:
		return (EOPNOTSUPP);
	}
	/* NOTREACHED */
}

/*
 * Set registers on exec.
 * Clear all registers except sp, pc.
 */
void
setregs(p, pack, stack, retval)
	register struct proc *p;
	struct exec_package *pack;
	u_long stack;
	register_t *retval;
{
	extern struct proc *machFPCurProcPtr;

	bzero((caddr_t)p->p_md.md_regs, (FSR + 1) * sizeof(int));
	p->p_md.md_regs[SP] = stack;
	p->p_md.md_regs[PC] = pack->ep_entry & ~3;
	p->p_md.md_regs[PS] = PSL_USERSET;
	p->p_md.md_flags & ~MDP_FPUSED;
	if (machFPCurProcPtr == p)
		machFPCurProcPtr = (struct proc *)0;
}

/*
 * WARNING: code in locore.s assumes the layout shown for sf_signum
 * thru sf_handler so... don't screw with them!
 */
struct sigframe {
	int	sf_signum;		/* signo for handler */
	int	sf_code;		/* additional info for handler */
	struct	sigcontext *sf_scp;	/* context ptr for handler */
	sig_t	sf_handler;		/* handler addr for u_sigc */
	struct	sigcontext sf_sc;	/* actual context */
};

#ifdef DEBUG
int sigdebug = 0;
int sigpid = 0;
#define SDB_FOLLOW	0x01
#define SDB_KSTACK	0x02
#define SDB_FPSTATE	0x04
#endif

/*
 * Send an interrupt to process.
 */
void
sendsig(catcher, sig, mask, code)
	sig_t catcher;
	int sig, mask;
	u_long code;
{
	register struct proc *p = curproc;
	register struct sigframe *fp;
	register int *regs;
	register struct sigacts *psp = p->p_sigacts;
	int oonstack, fsize;
	struct sigcontext ksc;
	extern char sigcode[], esigcode[];

	regs = p->p_md.md_regs;
	oonstack = psp->ps_sigstk.ss_flags & SS_ONSTACK;
	/*
	 * Allocate and validate space for the signal handler
	 * context. Note that if the stack is in data space, the
	 * call to grow() is a nop, and the copyout()
	 * will fail if the process has not already allocated
	 * the space with a `brk'.
	 */
	fsize = sizeof(struct sigframe);
	if ((psp->ps_flags & SAS_ALTSTACK) &&
	    (psp->ps_sigstk.ss_flags & SS_ONSTACK) == 0 &&
	    (psp->ps_sigonstack & sigmask(sig))) {
		fp = (struct sigframe *)(psp->ps_sigstk.ss_base +
					 psp->ps_sigstk.ss_size - fsize);
		psp->ps_sigstk.ss_flags |= SS_ONSTACK;
	} else
		fp = (struct sigframe *)(regs[SP] - fsize);
	if ((unsigned)fp <= USRSTACK - ctob(p->p_vmspace->vm_ssize)) 
		(void)grow(p, (unsigned)fp);
#ifdef DEBUG
	if ((sigdebug & SDB_FOLLOW) ||
	    (sigdebug & SDB_KSTACK) && p->p_pid == sigpid)
		printf("sendsig(%d): sig %d ssp %x usp %x scp %x\n",
		       p->p_pid, sig, &oonstack, fp, &fp->sf_sc);
#endif
	/*
	 * Build the signal context to be used by sigreturn.
	 */
	ksc.sc_onstack = oonstack;
	ksc.sc_mask = mask;
	ksc.sc_pc = regs[PC];
	ksc.mullo = regs [MULLO];
	ksc.mulhi = regs [MULHI];
	ksc.sc_regs[ZERO] = 0xACEDBADE;		/* magic number */
	bcopy((caddr_t)&regs[1], (caddr_t)&ksc.sc_regs[1],
		sizeof(ksc.sc_regs) - sizeof(int));
	ksc.sc_fpused = p->p_md.md_flags & MDP_FPUSED;
	if (ksc.sc_fpused) {
		extern struct proc *machFPCurProcPtr;

		/* if FPU has current state, save it first */
		if (p == machFPCurProcPtr)
			MachSaveCurFPState(p);
		bcopy((caddr_t)&p->p_md.md_regs[F0], (caddr_t)ksc.sc_fpregs,
			sizeof(ksc.sc_fpregs));
	}
	if (copyout((caddr_t)&ksc, (caddr_t)&fp->sf_sc, sizeof(ksc))) {
		/*
		 * Process has trashed its stack; give it an illegal
		 * instruction to halt it in its tracks.
		 */
		SIGACTION(p, SIGILL) = SIG_DFL;
		sig = sigmask(SIGILL);
		p->p_sigignore &= ~sig;
		p->p_sigcatch &= ~sig;
		p->p_sigmask &= ~sig;
		psignal(p, SIGILL);
		return;
	}
	/* 
	 * Build the argument list for the signal handler.
	 */
	regs[A0] = sig;
	regs[A1] = code;
	regs[A2] = (int)&fp->sf_sc;
	regs[A3] = (int)catcher;

	regs[PC] = (int)catcher;
	regs[SP] = (int)fp;
	/*
	 * Signal trampoline code is at base of user stack.
	 */
	regs[RA] = (int)PS_STRINGS - (esigcode - sigcode);
#ifdef DEBUG
	if ((sigdebug & SDB_FOLLOW) ||
	    (sigdebug & SDB_KSTACK) && p->p_pid == sigpid)
		printf("sendsig(%d): sig %d returns\n",
		       p->p_pid, sig);
#endif
}

/*
 * System call to cleanup state after a signal
 * has been taken.  Reset signal mask and
 * stack state from context left by sendsig (above).
 * Return to previous pc and psl as specified by
 * context left by sendsig. Check carefully to
 * make sure that the user has not modified the
 * psl to gain improper priviledges or to cause
 * a machine fault.
 */
/* ARGSUSED */
sigreturn(p, uap, retval)
	struct proc *p;
	struct sigreturn_args /* {
		syscallarg(struct sigcontext *) sigcntxp;
	} */ *uap;
	register_t *retval;
{
	register struct sigcontext *scp;
	register int *regs;
	struct sigcontext ksc;
	int error;

	scp = SCARG(uap, sigcntxp);
#ifdef DEBUG
	if (sigdebug & SDB_FOLLOW)
		printf("sigreturn: pid %d, scp %x\n", p->p_pid, scp);
#endif
	regs = p->p_md.md_regs;
	/*
	 * Test and fetch the context structure.
	 * We grab it all at once for speed.
	 */
	error = copyin((caddr_t)scp, (caddr_t)&ksc, sizeof(ksc));
	if (error || ksc.sc_regs[ZERO] != 0xACEDBADE) {
#ifdef DEBUG
		if (!(sigdebug & SDB_FOLLOW))
			printf("sigreturn: pid %d, scp %x\n", p->p_pid, scp);
		printf("  old sp %x ra %x pc %x\n",
			regs[SP], regs[RA], regs[PC]);
		printf("  new sp %x ra %x pc %x err %d z %x\n",
			ksc.sc_regs[SP], ksc.sc_regs[RA], ksc.sc_regs[PC],
			error, ksc.sc_regs[ZERO]);
#endif
		return (EINVAL);
	}
	scp = &ksc;
	/*
	 * Restore the user supplied information
	 */
	if (scp->sc_onstack & 01)
		p->p_sigacts->ps_sigstk.ss_flags |= SS_ONSTACK;
	else
		p->p_sigacts->ps_sigstk.ss_flags &= ~SS_ONSTACK;
	p->p_sigmask = scp->sc_mask &~ sigcantmask;
	regs[PC] = scp->sc_pc;
	regs[MULLO] = scp->mullo;
	regs[MULHI] = scp->mulhi;
	bcopy((caddr_t)&scp->sc_regs[1], (caddr_t)&regs[1],
		sizeof(scp->sc_regs) - sizeof(int));
	if (scp->sc_fpused)
		bcopy((caddr_t)scp->sc_fpregs, (caddr_t)&p->p_md.md_regs[F0],
			sizeof(scp->sc_fpregs));
	return (EJUSTRETURN);
}

int	waittime = -1;

boot(howto)
	register int howto;
{

	/* take a snap shot before clobbering any registers */
	if (curproc)
		savectx(curproc->p_addr, 0);

#ifdef DEBUG
	if (panicstr)
		stacktrace();
#endif

	boothowto = howto;
	if ((howto & RB_NOSYNC) == 0 && waittime < 0) {
		/*
		 * Synchronize the disks....
		 */
		waittime = 0;
		vfs_shutdown ();

		/*
		 * If we've been adjusting the clock, the todr
		 * will be out of synch; adjust it now.
		 */
		resettodr();
	}
	(void) splhigh();		/* extreme priority */
	if (callv != &callvec) {
		if (howto & RB_HALT)
			(*callv->_rex)('h');
		else {
			if (howto & RB_DUMP)
				dumpsys();
			(*callv->_rex)('b');
		}
	} else if (howto & RB_HALT) {
		volatile void (*f)() = (volatile void (*)())DEC_PROM_REINIT;

		(*f)();	/* jump back to prom monitor */
	} else {
		volatile void (*f)() = (volatile void (*)())DEC_PROM_AUTOBOOT;

		if (howto & RB_DUMP)
			dumpsys();
		(*f)();	/* jump back to prom monitor and do 'auto' cmd */
	}
	/*NOTREACHED*/
}

int	dumpmag = (int)0x8fca0101;	/* magic number for savecore */
int	dumpsize = 0;		/* also for savecore */
long	dumplo = 0;

dumpconf()
{
	int nblks;

	dumpsize = physmem;
	if (dumpdev != NODEV && bdevsw[major(dumpdev)].d_psize) {
		nblks = (*bdevsw[major(dumpdev)].d_psize)(dumpdev);
		if (dumpsize > btoc(dbtob(nblks - dumplo)))
			dumpsize = btoc(dbtob(nblks - dumplo));
		else if (dumplo == 0)
			dumplo = nblks - btodb(ctob(physmem));
	}
	/*
	 * Don't dump on the first CLBYTES (why CLBYTES?)
	 * in case the dump device includes a disk label.
	 */
	if (dumplo < btodb(CLBYTES))
		dumplo = btodb(CLBYTES);
}

/*
 * Doadump comes here after turning off memory management and
 * getting on the dump stack, either when called above, or by
 * the auto-restart code.
 */
dumpsys()
{
	int error;

	msgbufmapped = 0;
	if (dumpdev == NODEV)
		return;
	/*
	 * For dumps during autoconfiguration,
	 * if dump device has already configured...
	 */
	if (dumpsize == 0)
		dumpconf();
	if (dumplo < 0)
		return;
	printf("\ndumping to dev %x, offset %d\n", dumpdev, dumplo);
	printf("dump ");
	switch (error = (*bdevsw[major(dumpdev)].d_dump)(dumpdev)) {

	case ENXIO:
		printf("device bad\n");
		break;

	case EFAULT:
		printf("device not ready\n");
		break;

	case EINVAL:
		printf("area improper\n");
		break;

	case EIO:
		printf("i/o error\n");
		break;

	default:
		printf("error %d\n", error);
		break;

	case 0:
		printf("succeeded\n");
	}
}

/*
 * Return the best possible estimate of the time in the timeval
 * to which tvp points.  Unfortunately, we can't read the hardware registers.
 * We guarantee that the time will be greater than the value obtained by a
 * previous call.
 */
void
microtime(tvp)
	register struct timeval *tvp;
{
	int s = splclock();
	static struct timeval lasttime;

	*tvp = time;
#ifdef notdef
	tvp->tv_usec += clkread();
	while (tvp->tv_usec > 1000000) {
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}
#endif
	if (tvp->tv_sec == lasttime.tv_sec &&
	    tvp->tv_usec <= lasttime.tv_usec &&
	    (tvp->tv_usec = lasttime.tv_usec + 1) > 1000000) {
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}
	lasttime = *tvp;
	splx(s);
}

initcpu()
{
	register volatile struct chiptime *c;
	int i;

	/* Reset after bus errors during probe */
	if (Mach_reset_addr) {
		*Mach_reset_addr = 0;
		MachEmptyWriteBuffer();
	}

	/* clear any pending interrupts */
	switch (pmax_boardtype) {
	case DS_PMAX:
		break;	/* nothing to  do for KN01. */
	case DS_3MAXPLUS:
	case DS_3MIN:
	case DS_MAXINE:
		*(u_int *)ASIC_REG_INTR(asic_base) = 0;
		break;
	case DS_3MAX:
		*(u_int *)MACH_PHYS_TO_UNCACHED(KN02_SYS_CHKSYN) = 0;
		MachEmptyWriteBuffer();
		break;
	default:
		printf("Unknown system type in initcpu()\n");
		break;
	}

	/*
	 * With newconf, this should be  done elswhere, but without it
	 * we hang (?)
	 */
#if 1 /*XXX*/
	/* disable clock interrupts (until startrtclock()) */
	if (Mach_clock_addr) {
	c = Mach_clock_addr;
	c->regb = REGB_DATA_MODE | REGB_HOURS_FORMAT;
	i = c->regc;
	}
	return (i);
#endif
}

/*
 * Convert an ASCII string into an integer.
 */
int
atoi(s)
	char *s;
{
	int c;
	unsigned base = 10, d;
	int neg = 0, val = 0;

	if (s == 0 || (c = *s++) == 0)
		goto out;

	/* skip spaces if any */
	while (c == ' ' || c == '\t')
		c = *s++;

	/* parse sign, allow more than one (compat) */
	while (c == '-') {
		neg = !neg;
		c = *s++;
	}

	/* parse base specification, if any */
	if (c == '0') {
		c = *s++;
		switch (c) {
		case 'X':
		case 'x':
			base = 16;
			break;
		case 'B':
		case 'b':
			base = 2;
			break;
		default:
			base = 8;
		}
	}

	/* parse number proper */
	for (;;) {
		if (c >= '0' && c <= '9')
			d = c - '0';
		else if (c >= 'a' && c <= 'z')
			d = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			d = c - 'A' + 10;
		else
			break;
		val *= base;
		val += d;
		c = *s++;
	}
	if (neg)
		val = -val;
out:
	return val;	
}


#ifdef DS5000
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */


/*
 * Driver map: associates a device driver to an option type.
 * Drivers name are (arbitrarily) defined in each driver and
 * used in the various config tables.
 */
struct drivers_map {
	char	module_name[TC_ROM_LLEN];	/* from ROM, literally! */
	char	*driver_name;			/* in bus_??_init[] tables */
} tc_drivers_map[] = {
	{ "KN02    ",	"dc"},		/* (*) 3max system board (with DC) */
	{ "PMAD-AA ",	"le"},		/* Ether */
	{ "PMAZ-AA ",	"asc"},		/* SCSI */
	{ "PMAG-AA ",	"mfb"},		/* Mono Frame Buffer */
	{ "PMAG-BA ",	"cfb"},		/* Color Frame Buffer */
	{ "PMAGB-BA",	"sfb"},		/* Smart Frame Buffer */
	{ "PMAG-CA ",	"ga"},		/* 2D graphic board */
	{ "PMAG-DA ",	"gq"},		/* 3D graphic board (LM) */
	{ "PMAG-FA ",	"gq"},		/* 3D graphic board (HE) */
	{ "PMAG-DV ",	"xcfb"},	/* (*) maxine Color Frame Buffer */
	{ "Z8530   ",	"scc"},		/* (*) 3min/maxine serial lines */
	{ "ASIC    ",	"asic"},	/* (*) 3min/maxine DMA controller */
	{ "XINE-FDC",	"fdc"},		/* (*) maxine floppy controller */
	{ "DTOP    ",	"dtop"},	/* (*) maxine desktop bus */
	{ "AMD79c30",	"isdn"},	/* (*) maxine ISDN chip */
	{ "XINE-FRC",	"frc"},		/* (*) maxine free-running counter */
	{ "PMAF-AA ",   "fza"},		/* slow FDDI */
	{ "T3PKT   ",   "tt"},		/* DECWRL turbochannel T3 */
	{ "T1D4PKT ",   "ds"},		/* DECWRL turbochannel T1 */
	{ "FORE_ATM",	"fa"},		/* Fore ATM */
	{ "", 0}			/* list end */
};

/*
 * Identify an option on the TC.  Looks at the mandatory
 * info in the option's ROM and checks it.
 */
#ifdef DEBUG
int tc_verbose = 0;
#endif

static int
tc_identify_option(addr, slot, complain)
	tc_rommap_t	*addr;
	tc_option_t	*slot;
	int		complain;
{
	register int	i;
	unsigned char   width;
	char            firmwr[TC_ROM_LLEN+1], vendor[TC_ROM_LLEN+1],
			module[TC_ROM_LLEN+1], host_type[TC_ROM_SLEN+1];

	/*
	 * We do not really use the 'width' info, but take advantage
	 * of the restriction that the spec impose on the portion
	 * of the ROM that maps between +0x3e0 and +0x470, which
	 * is the only piece we need to look at.
	 */
	width = addr->rom_width.value;
	switch (width) {
	case 1:
	case 2:
	case 4:
		break;

	default:
#ifdef DEBUG
		if (tc_verbose && complain)
			printf("%s (x%x) at x%x\n", "Invalid ROM width",
			       width, addr);
#endif
		return (0);
	}

	if (addr->rom_stride.value != 4) {
#ifdef DEBUG
		if (tc_verbose && complain)
			printf("%s (x%x) at x%x\n", "Invalid ROM stride",
			       addr->rom_stride.value, addr);
#endif
		return (0);
	}

	if ((addr->test_data[0] != 0x55) ||
	    (addr->test_data[4] != 0x00) ||
	    (addr->test_data[8] != 0xaa) ||
	    (addr->test_data[12] != 0xff)) {
#ifdef DEBUG
		if (tc_verbose && complain)
			printf("%s x%x\n", "Test pattern failed, option at",
			       addr);
#endif
		return (0);
	}

	for (i = 0; i < TC_ROM_LLEN; i++) {
		firmwr[i] = addr->firmware_rev[i].value;
		vendor[i] = addr->vendor_name[i].value;
		module[i] = addr->module_name[i].value;
		if (i >= TC_ROM_SLEN)
			continue;
		host_type[i] = addr->host_firmware_type[i].value;
	}
	firmwr[TC_ROM_LLEN] = vendor[TC_ROM_LLEN] =
	module[TC_ROM_LLEN] = host_type[TC_ROM_SLEN] = '\0';

#ifdef DEBUG
	if (tc_verbose)
		printf("%s %s '%s' at 0x%x\n %s %s %s '%s'\n %s %d %s %d %s\n",
		"Found a", vendor, module, addr,
		"Firmware rev.", firmwr,
		"diagnostics for a", host_type,
		"ROM size is", addr->rom_size.value << 3,
		"Kbytes, uses", addr->slot_size.value, "TC slot(s)");
#endif

	bcopy(module, slot->module_name, TC_ROM_LLEN);
	bcopy(vendor, slot->module_id, TC_ROM_LLEN);
	bcopy(firmwr, &slot->module_id[TC_ROM_LLEN], TC_ROM_LLEN);
	slot->slot_size = addr->slot_size.value;
	slot->rom_width = width;

	return (1);
}


/*
 * Probe a slot in the TURBOchannel. Return TRUE if a valid option
 * is present, FALSE otherwise. A side-effect is to fill the slot
 * descriptor with the size of the option, whether it is
 * recognized or not.
 */
int
tc_probe_slot(addr, slot)
	caddr_t addr;
	tc_option_t *slot;
{
	int i;
	static unsigned tc_offset_rom[] = {
		TC_OFF_PROTO_ROM, TC_OFF_ROM
	};
#define TC_N_OFFSETS	sizeof(tc_offset_rom)/sizeof(unsigned)

	slot->slot_size = 1;

	for (i = 0; i < TC_N_OFFSETS; i++) {
		if (badaddr(addr + tc_offset_rom[i], 4))
			continue;
		/* complain only on last chance */
		if (tc_identify_option((tc_rommap_t *)(addr + tc_offset_rom[i]),
		    slot, i == (TC_N_OFFSETS-1)))
			return (1);
	}
	return (0);
#undef TC_N_OFFSETS
}

/*
 * Enable/Disable interrupts for a TURBOchannel slot.
 */
void
kn02_enable_intr(slotno, handler, unit, on)
	register u_int slotno;
	void (*handler)();
	int unit, on;
{
	register volatile int *p_csr =
		(volatile int *)MACH_PHYS_TO_UNCACHED(KN02_SYS_CSR);
	int csr;
	int s;

#if 0
	printf("3MAX enable_intr: imask %x, %sabling slot %d, unit %d\n",
	       kn03_tc3_imask, (on? "en" : "dis"), slotno, unit);
#endif

	if (slotno > TC_MAX_LOGICAL_SLOTS)
		panic("kn02_enable_intr: bogus slot %d\n", slotno);

	if (on)  {
		/*printf("kn02: slot %d handler 0x%x\n", slotno, handler);*/
		tc_slot_info[slotno].intr = handler;
		tc_slot_info[slotno].unit = unit;
	} else {
		tc_slot_info[slotno].intr = 0;
		tc_slot_info[slotno].unit = 0;
	}

	slotno = 1 << (slotno + KN02_CSR_IOINTEN_SHIFT);
	s = Mach_spl0();
	csr = *p_csr & ~(KN02_CSR_WRESERVED | 0xFF);
	if (on)
		*p_csr = csr | slotno;
	else
		*p_csr = csr & ~slotno;
	splx(s);
}

/*
 *	Object:
 *		kmin_enable_intr		EXPORTED function
 *
 *	Enable/Disable interrupts from a TURBOchannel slot.
 *
 *	We pretend we actually have 8 slots even if we really have
 *	only 4: TCslots 0-2 maps to slots 0-2, TCslot3 maps to
 *	slots 3-7 (see pmax/tc/ds-asic-conf.c).
 */
void
kmin_enable_intr(slotno, handler, unit, on)
	register unsigned int slotno;
	void (*handler)();
	int unit, on;
{
	register unsigned mask;

	switch (slotno) {
	case 0:
	case 1:
	case 2:
		return;
	case KMIN_SCSI_SLOT:
		mask = (KMIN_INTR_SCSI | KMIN_INTR_SCSI_PTR_LOAD |
			KMIN_INTR_SCSI_OVRUN | KMIN_INTR_SCSI_READ_E);
		break;
	case KMIN_LANCE_SLOT:
		mask = KMIN_INTR_LANCE;
		break;
	case KMIN_SCC0_SLOT:
		mask = KMIN_INTR_SCC_0;
		break;
	case KMIN_SCC1_SLOT:
		mask = KMIN_INTR_SCC_1;
		break;
	case KMIN_ASIC_SLOT:
		mask = KMIN_INTR_ASIC;
		break;
	default:
		return;
	}
	if (on)
		kmin_tc3_imask |= mask;
	else
		kmin_tc3_imask &= ~mask;
}

/*
 *	Object:
 *		xine_enable_intr		EXPORTED function
 *
 *	Enable/Disable interrupts from a TURBOchannel slot.
 *
 *	We pretend we actually have 11 slots even if we really have
 *	only 3: TCslots 0-1 maps to slots 0-1, TCslot 2 is used for
 *	the system (TCslot3), TCslot3 maps to slots 3-10
 *	 (see pmax/tc/ds-asic-conf.c).
 *	Note that all these interrupts come in via the IMR.
 */
void
xine_enable_intr(slotno, handler, unit, on)
	register unsigned int slotno;
	void (*handler)();
	int unit, on;
{
	register unsigned mask;

	switch (slotno) {
	case 0:			/* a real slot, but  */
		mask = XINE_INTR_TC_0;
		break;
	case 1:			/* a real slot, but */
		mask = XINE_INTR_TC_1;
		break;
	case XINE_FLOPPY_SLOT:
		mask = XINE_INTR_FLOPPY;
		break;
	case XINE_SCSI_SLOT:
		mask = (XINE_INTR_SCSI | XINE_INTR_SCSI_PTR_LOAD |
			XINE_INTR_SCSI_OVRUN | XINE_INTR_SCSI_READ_E);
		break;
	case XINE_LANCE_SLOT:
		mask = XINE_INTR_LANCE;
		break;
	case XINE_SCC0_SLOT:
		mask = XINE_INTR_SCC_0;
		break;
	case XINE_DTOP_SLOT:
		mask = XINE_INTR_DTOP_RX;
		break;
	case XINE_ISDN_SLOT:
		mask = XINE_INTR_ISDN;
		break;
	case XINE_ASIC_SLOT:
		mask = XINE_INTR_ASIC;
		break;
	default:
		return;/* ignore */
	}
	if (on)
		xine_tc3_imask |= mask;
	else
		xine_tc3_imask &= ~mask;
	*(u_int *)ASIC_REG_IMSK(asic_base) = xine_tc3_imask;
}

#ifdef DS5000_240
void
kn03_tc_reset()
{
/*
	 * Reset interrupts, clear any errors from newconf probes
	 */
	*(u_int *)ASIC_REG_INTR(asic_base) = 0;
	*(unsigned *)MACH_PHYS_TO_UNCACHED(KN03_SYS_ERRADR) = 0;
}


/*
 *	Object:
 *		kn03_enable_intr		EXPORTED function
 *
 *	Enable/Disable interrupts from a TURBOchannel slot.
 *
 *	We pretend we actually have 8 slots even if we really have
 *	only 4: TCslots 0-2 maps to slots 0-2, TCslot3 maps to
 *	slots 3-7 (see pmax/tc/ds-asic-conf.c).
 */
void
kn03_enable_intr(slotno, handler, unit, on)
	register unsigned int slotno;
	void (*handler)();
	int unit, on;
{
	register unsigned mask;

#if 0
	printf("3MAXPLUS: imask %x, %sabling slot %d, unit %d addr 0x%x\n",
	       kn03_tc3_imask, (on? "en" : "dis"), slotno, unit, handler);
#endif

	switch (slotno) {
	case 0:
		mask = KN03_INTR_TC_0;
		break;
	case 1:
		mask = KN03_INTR_TC_1;
		break;
	case 2:
		mask = KN03_INTR_TC_2;
		break;
	case KN03_SCSI_SLOT:
		mask = (KN03_INTR_SCSI | KN03_INTR_SCSI_PTR_LOAD |
			KN03_INTR_SCSI_OVRUN | KN03_INTR_SCSI_READ_E);
		break;
	case KN03_LANCE_SLOT:
		mask = KN03_INTR_LANCE;
		break;
	case KN03_SCC0_SLOT:
		mask = KN03_INTR_SCC_0;
		break;
	case KN03_SCC1_SLOT:
		mask = KN03_INTR_SCC_1;
		break;
	case KN03_ASIC_SLOT:
		mask = KN03_INTR_ASIC;
		break;
	default:
#ifdef DIAGNOSTIC
		printf("warning: enabling unknown intr %x\n", slotno);
#endif
		goto done;
	}
	if (on) {
		kn03_tc3_imask |= mask;
		tc_slot_info[slotno].intr = handler;
		tc_slot_info[slotno].unit = unit;

	} else {
		kn03_tc3_imask &= ~mask;
		tc_slot_info[slotno].intr = 0;
	}
done:
	*(u_int *)ASIC_REG_IMSK(asic_base) = kn03_tc3_imask;
}
#endif /* DS5000_240 */


/*
 * Initialize the I/O asic
 */
static void
asic_init(isa_maxine)
	int isa_maxine;
{
	volatile u_int *decoder;

	/* These are common between 3min and maxine */
	decoder = (volatile u_int *)ASIC_REG_LANCE_DECODE(asic_base);
	*decoder = KMIN_LANCE_CONFIG;
}
#endif /* DS5000 */
