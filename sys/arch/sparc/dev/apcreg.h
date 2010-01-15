/*	$NetBSD: apcreg.h,v 1.1 2010/01/15 20:57:12 bouyer Exp $	*/

/*
 * Copyright (c) 2010 Manuel Bouyer.
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

#define APC_IDLE_REG	0x00
#define APC_IDLE_REG_IDLE	0x01	/* suspend CPU */
#define APC_FANCTL_REG	0x20
#define APC_FANCTL_REG_HI	0x00	/* full speed */
#define APC_FANCTL_REG_LOW	0x01	/* low speed */
#define APC_CPOWER_REG	0x24
#define APC_CPOWER_REG_ON	0x00	/* Convenience power outlet on */
#define APC_CPOWER_REG_OFF	0x01	/* Convenience power outlet off */
#define APC_BPORT_REG	0x30

#define APC_REG_SIZE	0x34
