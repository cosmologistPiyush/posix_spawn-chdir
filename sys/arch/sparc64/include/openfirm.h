/*	$NetBSD: openfirm.h,v 1.5 1998/09/02 05:51:38 eeh Exp $	*/

/*
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Prototypes for additional OpenFirmware Interface Routines
 */

#include <dev/ofw/openfirm.h>

/* All cells are 8 byte slots */
typedef u_int64_t cell_t;
#ifdef _LP64
#define HDL2CELL(x)	(cell_t)(u_int)(int)(x)
#define ADR2CELL(x)	(cell_t)(x)
#else
#define HDL2CELL(x)	(cell_t)(u_int)(int)(x)
#define ADR2CELL(x)	(cell_t)(u_int)(int)(x)
#endif

int OF_test __P((char* service));
int OF_test_method __P((int handle, char* method));
int OF_getproplen __P((int handle, char* prop));

int OF_stdin __P((void));
int OF_stdout __P((void));
void OF_poweroff __P((void)) __attribute__((__noreturn__));
void OF_set_symbol_lookup __P((void (*s2v)(void *), void (*v2s)(void *)));
void OF_sym2val __P((void *));
void OF_val2sym __P((void *));
