typedef u_int8_t physaddr[4];
typedef u_int8_t physlen[4];
#define	ltophys	_lto4l
#define	phystol	_4ltol

/*
 * I/O port offsets
 */
#define	BHA_CTRL_PORT		0	/* control (wo) */
#define	BHA_STAT_PORT		0	/* status (ro) */
#define	BHA_CMD_PORT		1	/* command (wo) */
#define	BHA_DATA_PORT		1	/* data (ro) */
#define	BHA_INTR_PORT		2	/* interrupt status (ro) */

/*
 * BHA_CTRL bits
 */
#define BHA_CTRL_HRST		0x80	/* Hardware reset */
#define BHA_CTRL_SRST		0x40	/* Software reset */
#define BHA_CTRL_IRST		0x20	/* Interrupt reset */
#define BHA_CTRL_SCRST		0x10	/* SCSI bus reset */

/*
 * BHA_STAT bits
 */
#define BHA_STAT_STST		0x80	/* Self test in Progress */
#define BHA_STAT_DIAGF		0x40	/* Diagnostic Failure */
#define BHA_STAT_INIT		0x20	/* Mbx Init required */
#define BHA_STAT_IDLE		0x10	/* Host Adapter Idle */
#define BHA_STAT_CDF		0x08	/* cmd/data out port full */
#define BHA_STAT_DF		0x04	/* Data in port full */
#define BHA_STAT_INVDCMD	0x01	/* Invalid command */

/*
 * BHA_CMD opcodes
 */
#define	BHA_NOP			0x00	/* No operation */
#define BHA_MBX_INIT		0x01	/* Mbx initialization */
#define BHA_START_SCSI		0x02	/* start scsi command */
#define BHA_INQUIRE_REVISION	0x04	/* Adapter Inquiry */
#define BHA_MBO_INTR_EN		0x05	/* Enable MBO available interrupt */
#if 0
#define BHA_SEL_TIMEOUT_SET	0x06	/* set selection time-out */
#define BHA_BUS_ON_TIME_SET	0x07	/* set bus-on time */
#define BHA_BUS_OFF_TIME_SET	0x08	/* set bus-off time */
#define BHA_SPEED_SET		0x09	/* set transfer speed */
#endif
#define BHA_INQUIRE_DEVICES	0x0a	/* return installed devices 0-7 */
#define BHA_INQUIRE_CONFIG	0x0b	/* return configuration data */
#define BHA_TARGET_EN		0x0c	/* enable target mode */
#define BHA_INQUIRE_SETUP	0x0d	/* return setup data */
#define BHA_ECHO		0x1e	/* Echo command data */
#define BHA_INQUIRE_DEVICES_2	0x23	/* return installed devices 8-15 */
#define BHA_MBX_INIT_EXTENDED	0x81	/* Mbx initialization */
#define BHA_INQUIRE_REVISION_3	0x84	/* Get 3rd firmware version byte */
#define BHA_INQUIRE_REVISION_4	0x85	/* Get 4th firmware version byte */
#define BHA_INQUIRE_MODEL	0x8b	/* Get hardware ID and revision */
#define	BHA_INQUIRE_PERIOD	0x8c	/* Get synchronous period */
#define BHA_INQUIRE_EXTENDED	0x8d	/* Adapter Setup Inquiry */
#define	BHA_ROUND_ROBIN		0x8f	/* Enable/Disable(default) round robin */

/*
 * BHA_INTR bits
 */
#define BHA_INTR_ANYINTR	0x80	/* Any interrupt */
#define BHA_INTR_SCRD		0x08	/* SCSI reset detected */
#define BHA_INTR_HACC		0x04	/* Command complete */
#define BHA_INTR_MBOA		0x02	/* MBX out empty */
#define BHA_INTR_MBIF		0x01	/* MBX in full */

struct bha_mbx_out {
	physaddr ccb_addr;
	u_char dummy[3];
	u_char cmd;
};

struct bha_mbx_in {
	physaddr ccb_addr;
	u_char dummy[3];
	u_char stat;
};

/*
 * mbo.cmd values
 */
#define BHA_MBO_FREE	0x0	/* MBO entry is free */
#define BHA_MBO_START	0x1	/* MBO activate entry */
#define BHA_MBO_ABORT	0x2	/* MBO abort entry */

/*
 * mbi.stat values
 */
#define BHA_MBI_FREE	0x0	/* MBI entry is free */
#define BHA_MBI_OK	0x1	/* completed without error */
#define BHA_MBI_ABORT	0x2	/* aborted ccb */
#define BHA_MBI_UNKNOWN	0x3	/* Tried to abort invalid CCB */
#define BHA_MBI_ERROR	0x4	/* Completed with error */

#if	defined(BIG_DMA)
WARNING...THIS WON'T WORK(won't fit on 1 page)
#if 0
#define      BHA_NSEG 2048    /* Number of scatter gather segments - to much vm */
#endif
#define	BHA_NSEG	128
#else
#define	BHA_NSEG	33
#endif /* BIG_DMA */

struct bha_scat_gath {
	physlen seg_len;
	physaddr seg_addr;
};

struct bha_ccb {
	u_char opcode;
	u_char:3, data_in:1, data_out:1,:3;
	u_char scsi_cmd_length;
	u_char req_sense_length;
	/*------------------------------------longword boundary */
	physlen data_length;
	/*------------------------------------longword boundary */
	physaddr data_addr;
	/*------------------------------------longword boundary */
	u_char dummy1[2];
	u_char host_stat;
	u_char target_stat;
	/*------------------------------------longword boundary */
	u_char target;
	u_char lun;
	struct scsi_generic scsi_cmd;
	u_char dummy2[1];
	u_char link_id;
	/*------------------------------------longword boundary */
	physaddr link_addr;
	/*------------------------------------longword boundary */
	physaddr sense_ptr;
/*-----end of HW fields-----------------------longword boundary */
	struct scsi_sense_data scsi_sense;
	/*------------------------------------longword boundary */
	struct bha_scat_gath scat_gath[BHA_NSEG];
	/*------------------------------------longword boundary */
	TAILQ_ENTRY(bha_ccb) chain;
	struct bha_ccb *nexthash;
	long hashkey;
	struct scsi_xfer *xs;		/* the scsi_xfer for this cmd */
	int flags;
#define	CCB_ALLOC	0x01
#define	CCB_ABORT	0x02
#ifdef BHADIAG
#define	CCB_SENDING	0x04
#endif
	int timeout;
};

/*
 * opcode fields
 */
#define BHA_INITIATOR_CCB	0x00	/* SCSI Initiator CCB */
#define BHA_TARGET_CCB		0x01	/* SCSI Target CCB */
#define BHA_INIT_SCAT_GATH_CCB	0x02	/* SCSI Initiator with scattter gather */
#define BHA_RESET_CCB		0x81	/* SCSI Bus reset */

/*
 * bha_ccb.host_stat values
 */
#define BHA_OK		0x00	/* cmd ok */
#define BHA_LINK_OK	0x0a	/* Link cmd ok */
#define BHA_LINK_IT	0x0b	/* Link cmd ok + int */
#define BHA_SEL_TIMEOUT	0x11	/* Selection time out */
#define BHA_OVER_UNDER	0x12	/* Data over/under run */
#define BHA_BUS_FREE	0x13	/* Bus dropped at unexpected time */
#define BHA_INV_BUS	0x14	/* Invalid bus phase/sequence */
#define BHA_BAD_MBO	0x15	/* Incorrect MBO cmd */
#define BHA_BAD_CCB	0x16	/* Incorrect ccb opcode */
#define BHA_BAD_LINK	0x17	/* Not same values of LUN for links */
#define BHA_INV_TARGET	0x18	/* Invalid target direction */
#define BHA_CCB_DUP	0x19	/* Duplicate CCB received */
#define BHA_INV_CCB	0x1a	/* Invalid CCB or segment list */

struct bha_extended_inquire {
	struct {
		u_char	opcode;
		u_char	len;
	} cmd;
	struct {
		u_char	bus_type;	/* Type of bus connected to */
#define	BHA_BUS_TYPE_24BIT	'A'	/* ISA bus */
#define	BHA_BUS_TYPE_32BIT	'E'	/* EISA/VLB/PCI bus */
#define	BHA_BUS_TYPE_MCA	'M'	/* MicroChannel bus */
		u_char	bios_address;	/* Address of adapter BIOS */
		u_short	max_segment;	/* ? */
	} reply;
};

struct bha_config {
	struct {
		u_char	opcode;
	} cmd;
	struct {
		u_char  chan;
		u_char  intr;
		u_char  scsi_dev:3;
		u_char	:5;
	} reply;
};

struct bha_toggle {
	struct {
		u_char	opcode;
		u_char	enable;
	} cmd;
};

struct bha_mailbox {
	struct {
		u_char	opcode;
		u_char	nmbx;
		physaddr addr;
	} cmd;
};

struct bha_model {
	struct {
		u_char	opcode;
		u_char	len;
	} cmd;
	struct {
		u_char	id[4];		/* i.e bt742a -> '7','4','2','A' */
		u_char	version[2];	/* i.e Board Revision 'H' -> 'H', 0x00 */
	} reply;
};

struct bha_revision {
	struct {
		u_char	opcode;
	} cmd;
	struct {
		u_char  board_type;
		u_char  custom_feature;
		char    firm_revision;
		u_char  firm_version;
	} reply;
};

struct bha_digit {
	struct {
		u_char	opcode;
	} cmd;
	struct {
		u_char  digit;
	} reply;
};

struct bha_devices {
	struct {
		u_char	opcode;
	} cmd;
	struct {
		u_char	junk[8];
	} reply;
};

struct bha_setup {
	struct {
		u_char	opcode;
		u_char	len;
	} cmd;
	struct {
		u_char  sync_neg:1;
		u_char  parity:1;
		u_char	:6;
		u_char  speed;
		u_char  bus_on;
		u_char  bus_off;
		u_char  num_mbx;
		u_char  mbx[3];		/*XXX */
		/* doesn't make sense with 32bit addresses */
		struct {
			u_char  offset:4;
			u_char  period:3;
			u_char  valid:1;
		} sync[8];
		u_char  disc_sts;
	} reply;
};

struct bha_period {
	struct {
		u_char	opcode;
		u_char	len;
	} cmd;
	struct {
		u_char	period[8];
	} reply;
};

#define INT9	0x01
#define INT10	0x02
#define INT11	0x04
#define INT12	0x08
#define INT14	0x20
#define INT15	0x40

#define EISADMA	0x00
#define CHAN0	0x01
#define CHAN5	0x20
#define CHAN6	0x40
#define CHAN7	0x80
