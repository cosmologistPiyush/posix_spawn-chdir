#include <sys/param.h>
#include "stand.h"
#include "samachdep.h"

rdopen(f, ctlr, unit, part)
	struct open_file *f;
	int ctlr, unit, part;
{
	f->f_devdata = (void *) 0x200000;
	return(0);
}

int
rdstrategy(ss, func, dblk, size, buf, rsize)
	void *ss;
	int func;
	daddr_t dblk;		/* block number */
	u_int size;		/* request size in bytes */
	void *buf;
	u_int *rsize;		/* out: bytes transferred */
{
	memcpy(buf, ss + (dblk << DEV_BSHIFT), size);
	*rsize = size;
	return(0);
}
