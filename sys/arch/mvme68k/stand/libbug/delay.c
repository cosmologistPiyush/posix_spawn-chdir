/*	$NetBSD: delay.c,v 1.1 1996/05/17 19:31:49 chuck Exp $	*/

/*
 * bug routines -- assumes that the necessary sections of memory
 * are preserved.
 */
#include <sys/types.h>
#include <machine/prom.h>

/* BUG - timing routine */
void
mvmeprom_delay(msec)
	int msec;
{
	MVMEPROM_ARG1(msec);
	MVMEPROM_CALL(MVMEPROM_DELAY);
}
