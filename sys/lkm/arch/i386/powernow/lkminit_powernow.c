/*	$NetBSD: lkminit_powernow.c,v 1.1 2006/01/11 00:18:29 xtraeme Exp $	*/

/*
 * Derived from:
 *
 * Copyright (c) 1993 Terrence R. Lambert.
 * Copyright (c) 2005 Olaf 'Rhialto' Seibert.
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
 *      This product includes software developed by Terrence R. Lambert.
 * 4. The name Terrence R. Lambert may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TERRENCE R. LAMBERT ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: lkminit_powernow.c,v 1.1 2006/01/11 00:18:29 xtraeme Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/lkm.h>
#include <sys/errno.h>
#include <machine/cpu.h>

int powernow_lkmentry(struct lkm_table *, int, int);
static int powernow_mod_handle(struct lkm_table *, int);

MOD_MISC("powernow");

void pnowk7_destroy(void);

/*
 * This function is called each time the module is loaded or unloaded.
 * Since we are a miscellaneous module, we have to provide whatever
 * code is necessary to patch ourselves into the area we are being
 * loaded to change.
 *
 * The stat information is basically common to all modules, so there
 * is no real issue involved with stat; we will leave it lkm_nofunc(),
 * since we don't have to do anything about it.
 */
static int
powernow_mod_handle(struct lkm_table *lkmtp, int cmd)
{
	int		err = 0;	/* default = success*/
	extern int sys_lkmnosys(struct lwp *, void *, register_t *);
	struct cpu_info *ci;
	uint32_t lfunc;
	uint32_t descs[4];

	switch (cmd) {
	case LKM_E_LOAD:
		/*
		 * Don't load twice! (lkmexists() is exported by kern_lkm.c)
		 */
		if (lkmexists(lkmtp))
			return EEXIST;

		/*
		 * Partial copy from /sys/arch/i386/i386/identcpu.c
		 * amd_family6_probe().
		 * This is only a partial protection against the wrong
		 * cpu.
		 */
		CPUID(0x80000000, lfunc, descs[1], descs[2], descs[3]);

		ci = curcpu();
		if (lfunc >= 0x80000007) {
		    CPUID(0x80000007, descs[0], descs[1], descs[2], descs[3]);
		    if ((descs[3] & 0x06)) {
			if ((ci->ci_signature & 0xF00) == 0x600)
			    pnowk7_init(ci);
		    }
		}
		break;		/* Success */

	case LKM_E_UNLOAD:
		pnowk7_destroy();
		break;		/* Success */

	default:	/* we only understand load/unload*/
		err = EINVAL;
		break;
	}

	return err;
}


/*
 * External entry point; should generally match name of .o file + '_lkmentry'.
 * The arguments are always the same for all loaded modules.  The "load",
 * "unload", and "stat" functions in "DISPATCH" will be called under
 * their respective circumstances.  If no function is desired, lkm_nofunc()
 * should be supplied.  They are called with the same arguments (cmd is
 * included to allow the use of a single function, ver is included for
 * version matching between modules and the kernel loader for the modules).
 *
 * Since we expect to link in the kernel and add external symbols to
 * the kernel symbol name space in a future version, generally all
 * functions used in the implementation of a particular module should
 * be static unless they are expected to be seen in other modules or
 * to resolve unresolved symbols alread existing in the kernel (the
 * second case is not likely to ever occur).
 *
 * The entry point should return 0 unless it is refusing load (in which
 * case it should return an errno from errno.h).
 */
int
powernow_lkmentry(struct lkm_table *lkmtp, int cmd, int ver)
{
	DISPATCH(lkmtp, cmd, ver, powernow_mod_handle,
	    powernow_mod_handle, lkm_nofunc)
}
