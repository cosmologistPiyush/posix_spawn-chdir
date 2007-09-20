/*	$Id: table.c,v 1.1.1.1 2007/09/20 13:08:47 abs Exp $	*/
/*
 * Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code and documentation must retain the above
 * copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditionsand the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 * 	This product includes software developed or owned by Caldera
 *	International, Inc.
 * Neither the name of Caldera International, Inc. nor the names of other
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE LIABLE
 * FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OFLIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

# include "mfile2"

# define WPTR TPTRTO|TINT|TLONG|TFLOAT|TDOUBLE|TPOINT|TUNSIGNED|TULONG
# define AWD SNAME|SOREG|SCON|STARNM|STARREG
/* tbl */
# define ANYSIGNED TPOINT|TINT|TLONG|TSHORT|TCHAR
# define ANYUSIGNED TUNSIGNED|TULONG|TUSHORT|TUCHAR
# define ANYFIXED ANYSIGNED|ANYUSIGNED
# define TWORD TINT|TUNSIGNED|TPOINT|TLONG|TULONG
/* tbl */

struct optab  table[] = {

PCONV,	INAREG|INTAREG,
	SAREG|AWD,	TCHAR|TSHORT,
	SANY,	TPOINT,
		NAREG|NASL,	RESC1,
		"	cvtZLl	AL,A1\n",

PCONV,	INAREG|INTAREG,
	SAREG|AWD,	TUCHAR|TUSHORT,
	SANY,	TPOINT,
		NAREG|NASL,	RESC1,
		"	movzZLl	AL,A1\n",

SCONV,	INTAREG|FORCC,
	SAREG,	TDOUBLE,
	SANY,	TDOUBLE,
		0,	RLEFT,
		"",

SCONV,	INTAREG|FORCC,
	SAREG|AWD,	TANY,
	SANY,	TFLOAT|TDOUBLE,
		NAREG|NASL,	RESC1|RESCC,
		"	cvtZLd	AL,A1\n",

SCONV,	INTAREG|FORCC,
	SAREG|AWD,	TFLOAT|TDOUBLE,
	SANY,	ANYFIXED,
		NAREG|NASL,	RESC1|RESCC,
		"	cvtZLZF	AL,A1\n",

SCONV,	INTAREG|FORCC,
	SAREG|SNAME|SCON|STARNM,	TANY,
	SANY,	ANYUSIGNED,
		NAREG|NASL,	RESC1|RESCC,
		"	movzZRl	AL,A1\n",

SCONV,	INTAREG|FORCC,
	SSOREG,	TANY,
	SANY,	ANYUSIGNED,
		NAREG|NASL,	RESC1|RESCC,
		"	movzZRl	AL,A1\n",

SCONV,	INTAREG|FORCC,
	SAREG|SNAME|SCON|STARNM,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1|RESCC,
		"	cvtZRl	AL,A1\n",

SCONV,	INTAREG|FORCC,
	SSOREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1|RESCC,
		"	cvtZRl	AL,A1\n",


INIT,	FOREFF,
	SCON,	TANY,
	SANY,	TWORD,
		0,	RNOP,
		"	.long	CL\n",

INIT,	FOREFF,
	SCON,	TANY,
	SANY,	TSHORT|TUSHORT,
		0,	RNOP,
		"	.word	CL\n",

INIT,	FOREFF,
	SCON,	TANY,
	SANY,	TCHAR|TUCHAR,
		0,	RNOP,
		"	.byte	CL\n",

	/* for the use of fortran only */

GOTO,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jbr	CL\n",

GOTO,	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp	(AL)\n",

STARG,	INTEMP,
	SCON|SAREG,	TANY,
	SANY,	TANY,
		NTEMP+2*NAREG,	RESC3,
		"ZS",

STASG,	FORARG,
	SNAME|SOREG,	TANY,
	SCON|SAREG,	TANY,
		0,	RNULL,
		"	subl2	ZT,sp\nZS",

STASG,	FOREFF,
	SNAME|SOREG,	TANY,
	SCON|SAREG,	TANY,
		0,	RNOP,
		"ZS",

STASG,	INAREG,
	SNAME|SOREG,	TANY,
	SCON,	TANY,
		NAREG,	RESC1,
		"ZS	movl	AR,A1\n",

STASG,	INAREG,
	SNAME|SOREG,	TANY,
	SAREG,	TANY,
		0,	RRIGHT,
		"	pushl	AR\nZS	movl	(sp)+,AR\n",

FLD,	INAREG|INTAREG,
	SANY,	TANY,
	SFLD,	ANYSIGNED,
		NAREG|NASR,	RESC1,
		"	extv	H,S,AR,A1\n",

FLD,	INAREG|INTAREG,
	SANY,	TANY,
	SFLD,	ANYUSIGNED,
		NAREG|NASR,	RESC1,
		"	extzv	H,S,AR,A1\n",

FLD,	FORARG,
	SANY,	TANY,
	SFLD,	ANYSIGNED,
		0,	RNULL,
		"	extv	H,S,AR,-(sp)\n",

FLD,	FORARG,
	SANY,	TANY,
	SFLD,	ANYUSIGNED,
		0,	RNULL,
		"	extzv	H,S,AR,-(sp)\n",

OPLOG,	FORCC,
	SAREG|AWD,	TWORD,
	SAREG|AWD,	TWORD,
		0,	RESCC,
		"	cmpl	AL,AR\nZP",

OPLOG,	FORCC,
	SAREG|AWD,	TSHORT|TUSHORT,
	SAREG|AWD,	TSHORT|TUSHORT,
		0,	RESCC,
		"	cmpw	AL,AR\nZP",

OPLOG,	FORCC,
	SAREG|AWD,	TCHAR|TUCHAR,
	SAREG|AWD,	TCHAR|TUCHAR,
		0,	RESCC,
		"	cmpb	AL,AR\nZP",

OPLOG,	FORCC,
	SAREG|AWD,	TSHORT|TUSHORT,
	SSCON,	TANY,
		0,	RESCC,
		"	cmpw	AL,AR\nZP",

OPLOG,	FORCC,
	SAREG|AWD,	TCHAR|TUCHAR,
	SCCON,	TANY,
		0,	RESCC,
		"	cmpb	AL,AR\nZP",

OPLOG,	FORCC,
	SAREG|AWD,	TDOUBLE,
	SAREG|AWD,	TDOUBLE,
		0,	RESCC,
		"	cmpd	AL,AR\nZP",

OPLOG,	FORCC,
	SAREG|AWD,	TFLOAT|TDOUBLE,
	SAREG|AWD,	TFLOAT|TDOUBLE,
		0,	RESCC,
		"	cmpf	AL,AR\nZP",

CCODES,	INAREG|INTAREG,
	SANY,	TANY,
	SANY,	TANY,
		NAREG,	RESC1,
		"	movl	$1,A1\nZN",

UNARY CALL,	INTAREG,
	SCON,	TANY,
	SANY,	TWORD|TCHAR|TUCHAR|TSHORT|TUSHORT|TFLOAT|TDOUBLE,
		NAREG|NASL,	RESC1, /* should be register 0 */
		"	calls	ZC,CL\n",

UNARY CALL,	INTAREG,
	SAREG,	TANY,
	SANY,	TWORD|TCHAR|TUCHAR|TSHORT|TUSHORT|TFLOAT|TDOUBLE,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"	calls	ZC,(AL)\n",

UNARY CALL,	INAREG|INTAREG,
	SNAME,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* really reg 0 */
		"	calls	ZC,*AL\n",

UNARY CALL,	INAREG|INTAREG,
	SSOREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* really reg 0 */
		"	calls	ZC,*AL\n",

ASG RS,	INAREG|FOREFF|FORCC,
	SAREG,	TWORD,
	SCON,	TINT,
		0,	RLEFT|RESCC,
		"	extzv	AR,ZU,AL,AL\n",

ASG RS,	INAREG|FOREFF|FORCC,
	SAREG,	TWORD,
	SAREG,	ANYFIXED,
		NAREG,	RLEFT|RESCC,
		"	subl3	AR,$32,A1\n	extzv	AR,A1,AL,AL\n",

ASG RS,	INAREG|FOREFF|FORCC,
	SAREG,	TWORD,
	SAREG|AWD,	TWORD,
		NAREG,	RLEFT|RESCC,
		"	subl3	AR,$32,A1\n	extzv	AR,A1,AL,AL\n",

RS,	INAREG|INTAREG|FORCC,
	SAREG,	TWORD,
	SCON,	TINT,
		NAREG|NASL,	RESC1|RESCC,
		"	extzv	AR,ZU,AL,A1\n",

ASG LS,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TWORD,
	SAREG|AWD,	ANYSIGNED|ANYUSIGNED,
		0,	RLEFT|RESCC,
		"	ashl	AR,AL,AL\n",

LS,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TWORD,
	SAREG|AWD,	ANYSIGNED|ANYUSIGNED,
		NAREG|NASL|NASR,	RESC1|RESCC,
		"	ashl	AR,AL,A1\n",

INCR,	FOREFF,
	SAREG|AWD,	TANY,
	SANY,	TANY,
		0,	RLEFT,
		"	ZE\n",

DECR,	FOREFF,
	SAREG|AWD,	TANY,
	SCON,	TANY,
		0,	RLEFT,
		"	ZE\n",

INCR,	INAREG|INTAREG,
	SAREG|AWD,	TANY,
	SCON,	TANY,
		NAREG,	RESC1,
		"	ZD\n",

DECR,	INAREG|INTAREG,
	SAREG|AWD,	TANY,
	SCON,	TANY,
		NAREG,	RESC1,
		"	ZD\n",

ASSIGN,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TANY,
	SAREG|AWD,	TANY,
		0,	RLEFT|RRIGHT|RESCC,
		"	ZA\n",

ASSIGN,	INAREG|FOREFF|FORCC,
	SFLD,	TANY,
	SAREG|AWD,	TWORD,
		0,	RRIGHT|RESCC,
		"	insv	AR,H,S,AL\n",

ASSIGN,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TWORD,
	SFLD,	ANYSIGNED,
		0,	RLEFT|RESCC,
		"	extv	H,S,AR,AL\n",

ASSIGN,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TWORD,
	SFLD,	ANYUSIGNED,
		0,	RLEFT|RESCC,
		"	extzv	H,S,AR,AL\n",

/* dummy UNARY MUL entry to get U* to possibly match OPLTYPE */
UNARY MUL,	FOREFF,
	SCC,	TANY,
	SCC,	TANY,
		0,	RNULL,
		"	HELP HELP HELP\n",

REG,	FORARG,
	SANY,	TANY,
	SAREG,	TDOUBLE|TFLOAT,
		0,	RNULL,
		"	movZR	AR,-(sp)\n",

REG,	INTEMP,
	SANY,	TANY,
	SAREG,	TDOUBLE,
		2*NTEMP,	RESC1,
		"	movd	AR,A1\n",

REG,	INTEMP,
	SANY,	TANY,
	SAREG,	TANY,
		NTEMP,	RESC1,
		"	movZF	AR,A1\n",

OPLEAF,	FOREFF,
	SANY,	TANY,
	SAREG|AWD,	TANY,
		0,	RLEFT,
		"",

OPLTYPE,	INAREG|INTAREG,
	SANY,	TANY,
	SANY,	TFLOAT|TDOUBLE,
		2*NAREG|NASR,	RESC1,
		"	ZA\n",

OPLTYPE,	INAREG|INTAREG,
	SANY,	TANY,
	SANY,	TANY,
		NAREG|NASR,	RESC1,
		"	ZA\n",

OPLTYPE,	FORCC,
	SANY,	TANY,
	SANY,	TANY,
		0,	RESCC,
		"	tstZR	AR\n",

OPLTYPE,	FORARG,
	SANY,	TANY,
	SANY,	TWORD,
		0,	RNULL,
		"	pushl	AR\n",

OPLTYPE,	FORARG,
	SANY,	TANY,
	SANY,	TCHAR|TSHORT,
		0,	RNULL,
		"	cvtZRl	AR,-(sp)\n",

OPLTYPE,	FORARG,
	SANY,	TANY,
	SANY,	TUCHAR|TUSHORT,
		0,	RNULL,
		"	movzZRl	AR,-(sp)\n",

OPLTYPE,	FORARG,
	SANY,	TANY,
	SANY,	TDOUBLE,
		0,	RNULL,
		"	movd	AR,-(sp)\n",

OPLTYPE,	FORARG,
	SANY,	TANY,
	SANY,	TFLOAT,
		0,	RNULL,
		"	cvtfd	AR,-(sp)\n",

UNARY MINUS,	INTAREG|FORCC,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG|TDOUBLE,
	SANY,	TANY,
		NAREG|NASL,	RESC1|RESCC,
		"	mnegZL	AL,A1\n",

COMPL,	INTAREG|FORCC,
	SAREG|AWD,	TINT|TUNSIGNED,
	SANY,	TANY,
		NAREG|NASL,	RESC1|RESCC,
		"	mcomZL	AL,A1\n",

COMPL,	INTAREG|FORCC,
	SAREG|AWD,	ANYSIGNED|ANYUSIGNED,
	SANY,	TANY,
		NAREG|NASL,	RESC1|RESCC,
		"	cvtZLl	AL,A1\n	mcoml	A1,A1\n",

AND,	FORCC,
	SAREG|AWD,	TWORD,
	SCON,	TWORD,
		0,	RESCC,
		"	bitl	ZZ,AL\n",

AND,	FORCC,
	SAREG|AWD,	TSHORT|TUSHORT,
	SSCON,	TWORD,
		0,	RESCC,
		"	bitw	ZZ,AL\n",

AND,	FORCC,
	SAREG|AWD,	TCHAR|TUCHAR,
	SCCON,	TWORD,
		0,	RESCC,
		"	bitb	ZZ,AL\n",

ASG AND,	INAREG|FOREFF|FORCC,
	SAREG,	ANYFIXED,
	SCON,	TWORD,
		0,	RLEFT|RESCC,
		"	bicl2	AR,AL\n",

ASG OPMUL,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
		0,	RLEFT|RESCC,
		"	OL2	AR,AL\n",

OPMUL,	INAREG|INTAREG|FORCC,
	STAREG,	TINT|TUNSIGNED|TLONG|TULONG,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
		0,	RLEFT|RESCC,
		"	OL2	AR,AL\n",

OPMUL,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
		NAREG|NASL|NASR,	RESC1|RESCC,
		"	OL3	AR,AL,A1\n",

ASG MOD,	INAREG|INTAREG|FOREFF|FORCC,
	SAREG,	TINT|TUNSIGNED|TLONG|TULONG,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
		NAREG,	RLEFT|RESCC,
		"	divl3	AR,AL,A1\n	mull2	AR,A1\n	subl2	A1,AL\n",

MOD,	INAREG|INTAREG,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
	SAREG|AWD,	TINT|TUNSIGNED|TLONG|TULONG,
		NAREG,	RESC1,
		"	divl3	AR,AL,A1\n	mull2	AR,A1\n	subl3	A1,AL,A1\n",

ASG PLUS,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	ANYSIGNED|ANYUSIGNED,
	SONE,	TINT|TLONG,
		0,	RLEFT|RESCC,
		"	incZL	AL\n",

ASG MINUS,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	ANYSIGNED|ANYUSIGNED,
	SONE,	TINT|TLONG,
		0,	RLEFT|RESCC,
		"	decZL	AL\n",

PLUS,	INAREG|INTAREG|FORCC,
	STAREG,	ANYFIXED,
	SONE,	TWORD,
		0,	RLEFT|RESCC,
		"	incZL	AL\n",

MINUS,	INAREG|INTAREG|FORCC,
	STAREG,	ANYFIXED,
	SONE,	TWORD,
		0,	RLEFT|RESCC,
		"	decZL	AL\n",

ASG OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TWORD,
	SAREG|AWD,	TWORD,
		0,	RLEFT|RESCC,
		"	OL2	AR,AL\n",

ASG OPSIMP,	INAREG|FOREFF|FORCC,
	AWD,	TSHORT|TUSHORT,
	SAREG|AWD,	TSHORT|TUSHORT,
		0,	RLEFT|RESCC,
		"	OW2	AR,AL\n",

ASG OPSIMP,	INAREG|FOREFF|FORCC,
	AWD,	TSHORT|TUSHORT,
	SSCON,	TWORD,
		0,	RLEFT|RESCC,
		"	OW2	AR,AL\n",

ASG OPSIMP,	INAREG|FOREFF|FORCC,
	AWD,	TCHAR|TUCHAR,
	SAREG|AWD,	TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"	OB2	AR,AL\n",

ASG OPSIMP,	INAREG|FOREFF|FORCC,
	AWD,	TCHAR|TUCHAR,
	SCCON,	TWORD,
		0,	RLEFT|RESCC,
		"	OB2	AR,AL\n",

OPSIMP,	INAREG|INTAREG|FORCC,
	STAREG,	ANYFIXED,
	SAREG|AWD,	TWORD,
		0,	RLEFT|RESCC,
		"	OL2	AR,AL\n",

OPSIMP,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TWORD,
	SAREG|AWD,	TWORD,
		NAREG|NASL|NASR,	RESC1|RESCC,
		"	OL3	AR,AL,A1\n",

ASG OPFLOAT,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TDOUBLE,
	SAREG|AWD,	TDOUBLE,
		0,	RLEFT|RESCC,
		"	OD2	AR,AL\n",

ASG OPFLOAT,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TFLOAT,
	SAREG|AWD,	TFLOAT,
		0,	RLEFT|RESCC,
		"	OF2	AR,AL\n",

ASG OPFLOAT,	INAREG|FOREFF|FORCC,
	SAREG|AWD,	TDOUBLE,
	SAREG|AWD,	TFLOAT,
		NAREG|NASR,	RLEFT|RESCC,
		"	cvtfd	AR,A1\n	OD2	A1,AL\n",

ASG OPFLOAT,	INAREG|INTAREG|FOREFF|FORCC,
	SAREG|AWD,	TFLOAT,
	SAREG|AWD,	TDOUBLE,
		NAREG,	RLEFT|RESC1|RESCC,
		"	cvtfd	AL,A1\n	OD2	AR,A1\n	cvtdf	A1,AL\n",

OPFLOAT,	INAREG|INTAREG|FORCC,
	STAREG,	TDOUBLE,
	SAREG|AWD,	TDOUBLE,
		0,	RLEFT|RESCC,
		"	OD2	AR,AL\n",

OPFLOAT,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TDOUBLE,
	SAREG|AWD,	TDOUBLE,
		NAREG|NASL|NASR,	RESC1|RESCC,
		"	OD3	AR,AL,A1\n",

OPFLOAT,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TFLOAT,
	SAREG|AWD,	TDOUBLE,
		NAREG|NASL,	RESC1|RESCC,
		"	cvtfd	AL,A1\n	OD2	AR,A1\n",

OPFLOAT,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TDOUBLE,
	SAREG|AWD,	TFLOAT,
		NAREG|NASR,	RESC1|RESCC,
		"	cvtfd	AR,A1\n	OD3	A1,AL,A1\n",

OPFLOAT,	INAREG|INTAREG|FORCC,
	SAREG|AWD,	TFLOAT,
	SAREG|AWD,	TFLOAT,
		NAREG|NASL|NASR,	RESC1|RESCC,
		"	OF3	AR,AL,A1\n	cvtfd	A1,A1\n",

	/* Default actions for hard trees ... */

# define DF(x) FORREW,SANY,TANY,SANY,TANY,REWRITE,x,""

UNARY MUL, DF( UNARY MUL ),

INCR, DF(INCR),

DECR, DF(INCR),

ASSIGN, DF(ASSIGN),

STASG, DF(STASG),

OPLEAF, DF(NAME),

OPLOG,	FORCC,
	SANY,	TANY,
	SANY,	TANY,
		REWRITE,	BITYPE,
		"",

OPLOG,	DF(NOT),

COMOP, DF(COMOP),

INIT, DF(INIT),

OPUNARY, DF(UNARY MINUS),


ASG OPANY, DF(ASG PLUS),

OPANY, DF(BITYPE),

FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	"help; I'm in trouble\n" };
