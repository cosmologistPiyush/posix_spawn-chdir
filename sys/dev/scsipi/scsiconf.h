/*	$NetBSD: scsiconf.h,v 1.15 1994/11/03 22:05:10 mycroft Exp $	*/

/*
 * Copyright (c) 1993, 1994 Charles Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 * Originally written by Julian Elischer (julian@tfs.com)
 * for TRW Financial Systems for use under the MACH(2.5) operating system.
 *
 * TRW Financial Systems, in accordance with their agreement with Carnegie
 * Mellon University, makes this software available to CMU to distribute
 * or use in any manner that they see fit as long as this message is kept with
 * the software. For this reason TFS also grants any other persons or
 * organisations permission to use or modify this software.
 *
 * TFS supplies this software to be publicly redistributed
 * on the understanding that TFS is not responsible for the correct
 * functioning of this software in any circumstances.
 *
 * Ported to run under 386BSD by Julian Elischer (julian@tfs.com) Sept 1992
 */

#ifndef	SCSI_SCSICONF_H
#define SCSI_SCSICONF_H 1
typedef	int			boolean;
typedef	long int		int32;
typedef	short int		int16;
typedef	char 			int8;
typedef	unsigned long int	u_int32;
typedef	unsigned short int	u_int16;
typedef	unsigned char 		u_int8;

#include <sys/queue.h>
#include <machine/cpu.h>
#include <scsi/scsi_debug.h>

/*
 * The following documentation tries to describe the relationship between the
 * various structures defined in this file:
 *
 * each adapter type has a scsi_adapter struct. This describes the adapter and
 *    identifies routines that can be called to use the adapter.
 * each device type has a scsi_device struct. This describes the device and
 *    identifies routines that can be called to use the device.
 * each existing device position (scsibus + target + lun)
 *    can be described by a scsi_link struct.
 *    Only scsi positions that actually have devices, have a scsi_link
 *    structure assigned. so in effect each device has scsi_link struct.
 *    The scsi_link structure contains information identifying both the
 *    device driver and the adapter driver for that position on that scsi bus,
 *    and can be said to 'link' the two.
 * each individual scsi bus has an array that points to all the scsi_link
 *    structs associated with that scsi bus. Slots with no device have
 *    a NULL pointer.
 * each individual device also knows the address of it's own scsi_link
 *    structure.
 *
 *				-------------
 *
 * The key to all this is the scsi_link structure which associates all the 
 * other structures with each other in the correct configuration.  The
 * scsi_link is the connecting information that allows each part of the 
 * scsi system to find the associated other parts.
 */


/*
 * These entrypoints are called by the high-end drivers to get services from
 * whatever low-end drivers they are attached to each adapter type has one of
 * these statically allocated.
 */
struct scsi_adapter {
/* 4*/	int		(*scsi_cmd)();
/* 8*/	void		(*scsi_minphys)();
/*12*/	int32		(*open_target_lu)();
/*16*/	int32		(*close_target_lu)();
/*20*/	u_int		(*adapter_info)(); /* see definitions below */
/*24*/	char		*name; /* name of scsi bus controller */
/*32*/	u_long	spare[2];
};

/*
 * return values for scsi_cmd()
 */
#define SUCCESSFULLY_QUEUED	0
#define TRY_AGAIN_LATER		1
#define	COMPLETE		2
#define	HAD_ERROR		3 /* do not use this, use COMPLETE */
#define	ESCAPE_NOT_SUPPORTED	4

/*
 * Format of adapter_info() response data
 * e.g. maximum number of entries queuable to a device by the adapter
 */
#define	AD_INF_MAX_CMDS		0x000000FF
/* 24 bits of other adapter characteristics go here */

/*
 * These entry points are called by the low-end drivers to get services from
 * whatever high-end drivers they are attached to.  Each device type has one
 * of these statically allocated.
 */
struct scsi_device {
/*  4*/	int	(*err_handler)(); /* returns -1 to say err processing complete */
/*  8*/	void	(*start)();
/* 12*/	int32	(*async)();
/* 16*/	int32	(*done)();	/* returns -1 to say done processing complete */
/* 20*/	char	*name;		/* name of device type */
/* 24*/	int	flags;		/* device type dependent flags */
/* 32*/	int32	spare[2];
};

/*
 * This structure describes the connection between an adapter driver and
 * a device driver, and is used by each to call services provided by
 * the other, and to allow generic scsi glue code to call these services
 * as well.
 */
struct scsi_link {
/*  1*/	u_int8	scsibus;		/* the Nth scsibus */
/*  2*/	u_int8	target;			/* targ of this dev */
/*  3*/	u_int8	lun;			/* lun of this dev */
/*  4*/	u_int8	adapter_targ;		/* what are we on the scsi bus */
/*  5*/	u_int8	dev_unit;		/* e.g. the 0 in sd0 */
/*  6*/	u_int8	opennings;		/* available operations */
/*  7*/	u_int8	active;			/* operations in progress */
/*  8*/ u_int8	sparea[1];
/* 12*/	int	flags;			/* flags that all devices have */
/* 16*/	struct	scsi_adapter *adapter;	/* adapter entry points etc. */
/* 20*/	struct	scsi_device *device;	/* device entry points etc. */
/* 24*/	void	*adapter_softc;		/* needed for call to foo_scsi_cmd */
/* 28*/	void	*fordriver;		/* for private use by the driver */
};
#define	SDEV_MEDIA_LOADED 	0x01	/* device figures are still valid */
#define	SDEV_WAITING	 	0x02	/* a process is waiting for this */
#define	SDEV_OPEN	 	0x04	/* at least 1 open session */
#define	SDEV_DBX		0xF0	/* debuging flags (scsi_debug.h) */	

/*
 * One of these is allocated and filled in for each scsi bus.
 * it holds pointers to allow the scsi bus to get to the driver
 * That is running each LUN on the bus
 * it also has a template entry which is the prototype struct
 * supplied by the adapter driver, this is used to initialise
 * the others, before they have the rest of the fields filled in
 */
struct scsibus_data {
	struct device sc_dev;
	struct scsi_link *adapter_link;		/* prototype supplied by adapter */
	struct scsi_link *sc_link[8][8];
};

/*
 * Each scsi transaction is fully described by one of these structures
 * It includes information about the source of the command and also the
 * device and adapter for which the command is destined.
 * (via the scsi_link structure)
 */
struct scsi_xfer {
/* 4*/	LIST_ENTRY(scsi_xfer) free_list;
/*12*/	int	flags;
/*16*/	struct	scsi_link *sc_link;	/* all about our device and adapter */
/*20*/	int	retries;		/* the number of times to retry */
/*24*/	int	timeout;		/* in milliseconds */
/*28*/	struct	scsi_generic *cmd;	/* The scsi command to execute */
/*32*/	int32	cmdlen;			/* how long it is */
/*36*/	u_char	*data;			/* dma address OR a uio address */
/*40*/	int32	datalen;		/* data len (blank if uio)    */
/*44*/	int32	resid;			/* how much buffer was not touched */
/*48*/	int	error;			/* an error value	*/
/*52*/	struct	buf *bp;		/* If we need to associate with a buf */
/*84*/	struct	scsi_sense_data	sense; /* 32 bytes*/
	/*
	 * Believe it or not, Some targets fall on the ground with
	 * anything but a certain sense length.
	 */
/*88*/	int32 req_sense_length;		/* Explicit request sense length */
/*92*/	int32 status;			/* SCSI status */
/*104*/	struct	scsi_generic cmdstore;	/* stash the command in here */
};

/*
 * Per-request Flag values
 */
#define	SCSI_NOSLEEP	0x01	/* Not a user... don't sleep		*/
#define	SCSI_NOMASK	0x02	/* dont allow interrupts.. booting	*/
#define	SCSI_NOSTART	0x04	/* left over from ancient history	*/
#define	SCSI_USER	0x08	/* Is a user cmd, call scsi_user_done	*/
#define	ITSDONE		0x10	/* the transfer is as done as it gets	*/
#define	INUSE		0x20	/* The scsi_xfer block is in use	*/
#define	SCSI_SILENT	0x40	/* Don't report errors to console	*/
#define SCSI_ERR_OK	0x80	/* An error on this operation is OK.	*/
#define	SCSI_RESET	0x100	/* Reset the device in question		*/
#define	SCSI_DATA_UIO	0x200	/* The data address refers to a UIO	*/
#define	SCSI_DATA_IN	0x400	/* expect data to come INTO memory	*/
#define	SCSI_DATA_OUT	0x800	/* expect data to flow OUT of memory	*/
#define	SCSI_TARGET	0x1000	/* This defines a TARGET mode op.	*/
#define	SCSI_ESCAPE	0x2000	/* Escape operation			*/

/*
 * Escape op codes.  This provides an extensible setup for operations
 * that are not scsi commands.  They are intended for modal operations.
 */

#define SCSI_OP_TARGET	0x0001
#define	SCSI_OP_RESET	0x0002
#define	SCSI_OP_BDINFO	0x0003

/*
 * Error values an adapter driver may return
 */
#define XS_NOERROR	0x0	/* there is no error, (sense is invalid)  */
#define XS_SENSE	0x1	/* Check the returned sense for the error */
#define	XS_DRIVER_STUFFUP 0x2	/* Driver failed to perform operation	  */
#define XS_TIMEOUT	0x03	/* The device timed out.. turned off?	  */
#define XS_SWTIMEOUT	0x04	/* The Timeout reported was caught by SW  */
#define XS_BUSY		0x08	/* The device busy, try again later?	  */

int scsi_targmatch __P((struct device *, void *, void *));

struct scsi_xfer *get_xs __P((struct scsi_link *, int));
void free_xs __P((struct scsi_xfer *, struct scsi_link *, int));
void sc_print_addr __P((struct scsi_link *sc_link));
u_int32 scsi_size __P((struct scsi_link *, int));
int scsi_test_unit_ready __P((struct scsi_link *, int));
int scsi_change_def __P((struct scsi_link *, int));
int scsi_inquire __P((struct scsi_link *, struct scsi_inquiry_data *, int));
int scsi_prevent __P((struct scsi_link *, int, int));
int scsi_start __P((struct scsi_link *, int, int));
void scsi_done __P((struct scsi_xfer *));
int scsi_scsi_cmd __P((struct scsi_link *, struct scsi_generic *,
			u_int32 cmdlen, u_char *data_addr,
			u_int32 datalen, int retries,
			int timeout, struct buf *bp,
			int flags));
int scsi_do_ioctl __P((struct scsi_link *, dev_t, u_long, caddr_t, int));

void show_scsi_xs __P((struct scsi_xfer *));
void show_scsi_cmd __P((struct scsi_xfer *));
void show_mem __P((unsigned char *, u_int32));

void	lto3b __P((int val, u_char *bytes));
int	_3btol __P((u_char *bytes));

#endif /*SCSI_SCSICONF_H*/
/* END OF FILE */
