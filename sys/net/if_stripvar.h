/*	$NetBSD: if_stripvar.h,v 1.7 2000/03/23 07:03:26 thorpej Exp $	*/

#ifndef _NET_IF_STRIPVAR_H_
#define _NET_IF_STRIPVAR_H_

/*
 * Definitions for STRIP interface data structures
 * 
 */
struct strip_softc {
	struct	ifnet sc_if;		/* network-visible interface */
	int	sc_unit;		/* XXX unit number */
	struct	ifqueue sc_fastq;	/* interactive output queue */
	struct	tty *sc_ttyp;		/* pointer to tty structure */
	struct	callout sc_timo_ch;	/* timeout callout */
	u_char	*sc_mp;			/* pointer to next available buf char */
	u_char	*sc_ep;			/* pointer to last available buf char */
	u_char	*sc_buf;		/* input buffer */
	u_char	*sc_rxbuf;		/* input destuffing buffer */
	u_char	*sc_txbuf;		/* output stuffing buffer */
	u_char	*sc_xxx;		/* XXX don't ask... */
	u_int	sc_flags;		/* see below */
	long	sc_oqlen;		/* previous output queue size */
	long	sc_otimeout;		/* number of times output's stalled */
#ifdef __NetBSD__
	int	sc_oldbufsize;		/* previous output buffer size */
	int	sc_oldbufquot;		/* previous output buffer quoting */
#endif
#ifdef INET				/* XXX */
	struct	slcompress sc_comp;	/* tcp compression data */
#endif

	int sc_state;			/* Radio reset state-machine */
#define ST_ALIVE	0x0		/*    answered  probe */
#define ST_PROBE_SENT	0x1		/*    probe sent, answer pending */
#define ST_DEAD		0x2		/*    no answer to probe; do reset */

	long sc_statetimo;		/* When (secs) current state ends */

	caddr_t	sc_bpf;			/* BPF data */
};


/* Internal flags */
#define	SC_ERROR	0x0001		/* Incurred error reading current pkt*/

#define SC_TIMEOUT	0x00000400	/* timeout is currently pending */

/* visible flags */
#define	SC_COMPRESS	IFF_LINK0	/* compress TCP traffic */
#define	SC_NOICMP	IFF_LINK1	/* supress ICMP traffic */
#define	SC_AUTOCOMP	IFF_LINK2	/* auto-enable TCP compression */

#ifdef _KERNEL
void	stripattach __P((int n));
void	stripclose __P((struct tty *));
void	stripinput __P((int, struct tty *));
int	stripioctl __P((struct ifnet *, u_long, caddr_t));
int	stripopen __P((dev_t, struct tty *));
int	stripoutput __P((struct ifnet *,
	    struct mbuf *, struct sockaddr *, struct rtentry *));
void	stripstart __P((struct tty *));
int	striptioctl __P((struct tty *, u_long, caddr_t, int));
#endif /* _KERNEL */

#endif /* _NET_IF_STRIPVAR_H_ */
