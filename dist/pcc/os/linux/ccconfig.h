/*	$Id: ccconfig.h,v 1.1.1.3 2008/02/10 20:05:06 ragge Exp $	*/

/*
 * Copyright (c) 2004 Anders Magnusson (ragge@ludd.luth.se).
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

/*
 * Various settings that controls how the C compiler works.
 */

#ifndef LIBDIR
#define LIBDIR "/usr/lib/"
#endif

/* XXX maybe configure could detect these? Or we should build our own? */
#if defined(mach_i386)
#define GCCLIBDIR LIBDIR "gcc/i586-suse-linux/4.1.0/"
#elif defined(mach_powerpc)
#define GCCLIBDIR LIBDIR "gcc/powerpc-unknown-linux/4.1.2/"
#endif

/* common cpp predefines */
#define	CPPADD	{ "-D__linux__", "-D__ELF__", "-I" INCLUDEDIR "/pcc", NULL, }

#define	DYNLINKER { "-dynamic-linker", "/lib/ld-linux.so.2", NULL }

#define CRT0FILE LIBDIR "crt1.o"
#define	LIBCLIBS { "-lc", "-lgcc_s", NULL }

#define STARTFILES { LIBDIR "crti.o", GCCLIBDIR "crtbegin.o", NULL }
#define	ENDFILES { GCCLIBDIR "crtend.o", LIBDIR "crtn.o", NULL }

#define STARTLABEL "_start"

#if defined(mach_i386)
#define	CPPMDADD { "-D__i386__", NULL, }
#elif defined(mach_powerpc)
#define	CPPMDADD { "-D__ppc__", NULL, }
#else
#error defines for arch missing
#endif

#define	STABS
#define ELFABI
