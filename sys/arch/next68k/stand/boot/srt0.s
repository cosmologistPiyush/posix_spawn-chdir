/*	$NetBSD: srt0.s,v 1.1.1.1 1998/06/09 07:53:06 dbj Exp $	*/
/*
 * Copyright (c) 1994 Rolf Grossmann
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
 *      This product includes software developed by Rolf Grossmann.
 * 4. The name of the author may not be used to endorse or promote products
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
 * Startup code for standalone system
 */

    .globl _mg			| struct prominfo
    .globl _end
    .globl _edata
    .globl _main

    .text
    .globl start

start:	
    || clear bss (this shouldn't hurt us i.e. cause an exception)
    movel #_edata,a2		| start of BSS
    movel #_end,a3		| end
Lclr:
    clrb a2@+			| clear BSS
    cmpl a2,a3			| done?
    bne Lclr			| no, keep going

    movl	#0x0808,d0
    movc	d0,cacr         | clear and disable on-chip cache(s)

    || catch exceptions myself
    movec vbr, a3
    movel a3, save_vbr		| save register for restoration
    lea vectbl, a4
    movel a3@(4),a4@(4)		| copy mg, just for sure
    movel a3@(180),a4@(180)	| copy vector for trap #13
    movel a3@(124),a4@(124)	| copy vector for int 7
    movec a4,vbr		| use the new table

    || save mg as pi
    movel a3@(4),_mg

    || make sure we disallow interrupts
    movew #0x2600,sr

    || let's go
    movel sp@(4),sp@-		| copy the argument we got
    jsr _main			| call C
    addql #4,sp

    || restore prom vectors
    movel save_vbr, a0
    movec a0,vbr
    
    || return kernel start address (still in d0)
    rts

    .globl __halt
__halt:
    movel save_vbr, a0
    movec a0,vbr		| restore prom vbr
hloop:
    movel #halt, d0
    trap #13			| halt the system
    bra hloop			| and don't allow continuation
    
    .globl _trap
trap:
    moveml d0-d7/a0-a7,sp@-	| save all registers

    movel sp,sp@-		| push pointer to registers
    jsr _trap			| call C to handle things (dump regs)
    addql   #4,sp
    tstl    d0
    jeq     Lstop
    moveml  sp@+,d0-d7/a0-a7
    rte
Lstop:
    bra Lstop			| don't move
    
    .data
save_vbr:
    .long 0
halt:
    .asciz "-h"
vectbl:				| copy of vector table (can't use .fill)
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
    .long trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap,trap
