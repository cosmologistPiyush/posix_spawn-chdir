/*	$NetBSD: bus.h,v 1.5 1996/10/22 18:06:54 cgd Exp $	*/

/*
 * Copyright (c) 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#ifndef _ALPHA_BUS_H_
#define	_ALPHA_BUS_H_

/*
 * Addresses (in bus space).
 */
typedef u_long bus_addr_t;
typedef u_long bus_size_t;

/*
 * Access methods for bus space.
 */
typedef struct alpha_bus_space *bus_space_tag_t;
typedef u_long bus_space_handle_t;

struct alpha_bus_space {
	/* cookie */
	void		*bs_cookie;

	/* mapping/unmapping */
	int		(*bs_map) __P((void *, bus_addr_t, bus_size_t,
			    int, bus_space_handle_t *));
	void		(*bs_unmap) __P((void *, bus_space_handle_t,
			    bus_size_t));
	int		(*bs_subregion) __P((void *, bus_space_handle_t,
			    bus_size_t, bus_size_t, bus_space_handle_t *));

	/* allocation/deallocation */
	int		(*bs_alloc) __P((void *, bus_addr_t, bus_addr_t,
			    bus_size_t, bus_size_t, bus_addr_t, int,
			    bus_addr_t *, bus_space_handle_t *));
	void		(*bs_free) __P((void *, bus_space_handle_t,
			    bus_size_t));

	/* read (single) */
	u_int8_t	(*bs_r_1) __P((void *, bus_space_handle_t,
			    bus_size_t));
	u_int16_t	(*bs_r_2) __P((void *, bus_space_handle_t,
			    bus_size_t));
	u_int32_t	(*bs_r_4) __P((void *, bus_space_handle_t,
			    bus_size_t));
	u_int64_t	(*bs_r_8) __P((void *, bus_space_handle_t,
			    bus_size_t));

	/* read multi */
	void		(*bs_rm_1) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int8_t *, size_t));
	void		(*bs_rm_2) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int16_t *, size_t));
	void		(*bs_rm_4) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int32_t *, size_t));
	void		(*bs_rm_8) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int64_t *, size_t));
					
	/* read region */
	void		(*bs_rr_1) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int8_t *, size_t));
	void		(*bs_rr_2) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int16_t *, size_t));
	void		(*bs_rr_4) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int32_t *, size_t));
	void		(*bs_rr_8) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int64_t *, size_t));
					
	/* write (single) */
	void		(*bs_w_1) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int8_t));
	void		(*bs_w_2) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int16_t));
	void		(*bs_w_4) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int32_t));
	void		(*bs_w_8) __P((void *, bus_space_handle_t, bus_size_t,
			    u_int64_t));

	/* write multi */
	void		(*bs_wm_1) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int8_t *, size_t));
	void		(*bs_wm_2) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int16_t *, size_t));
	void		(*bs_wm_4) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int32_t *, size_t));
	void		(*bs_wm_8) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int64_t *, size_t));
					
	/* write region */
	void		(*bs_wr_1) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int8_t *, size_t));
	void		(*bs_wr_2) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int16_t *, size_t));
	void		(*bs_wr_4) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int32_t *, size_t));
	void		(*bs_wr_8) __P((void *, bus_space_handle_t, bus_size_t,
			    const u_int64_t *, size_t));

	/* set multi */
	/* XXX IMPLEMENT */

	/* set region */
	/* XXX IMPLEMENT */

	/* copy */
	/* XXX IMPLEMENT */

	/* barrier */
	void		(*bs_barrier) __P((void *, bus_space_handle_t,
			    bus_size_t, bus_size_t, int));
};


/*
 * Utility macros; INTERNAL USE ONLY.
 */
#define	__bs_c(a,b)		__CONCAT(a,b)
#define	__bs_opname(op,size)	__bs_c(__bs_c(__bs_c(bs_,op),_),size)

#define	__bs_rs(sz, t, h, o)						\
	(*(t)->__bs_opname(r,sz))((t)->bs_cookie, h, o)
#define	__bs_ws(sz, t, h, o, v)						\
	(*(t)->__bs_opname(w,sz))((t)->bs_cookie, h, o, v)
#define	__bs_nonsingle(type, sz, t, h, o, a, c)				\
	(*(t)->__bs_opname(type,sz))((t)->bs_cookie, h, o, a, c)


/*
 * Mapping and unmapping operations.
 */
#define	bus_space_map(t, a, s, c, hp)					\
	(*(t)->bs_map)((t)->bs_cookie, (a), (s), (c), (hp))
#define	bus_space_unmap(t, h, s)					\
	(*(t)->bs_unmap)((t)->bs_cookie, (h), (s))
#define	bus_space_subregion(t, h, o, s, hp)				\
	(*(t)->bs_subregion)((t)->bs_cookie, (h), (o), (s), (hp))


/*
 * Allocation and deallocation operations.
 */
#define	bus_space_alloc(t, rs, re, s, a, b, c, ap, hp)			\
	(*(t)->bs_alloc)((t)->bs_cookie, (rs), (re), (s), (a), (b),	\
	    (c), (ap), (hp))
#define	bus_space_free(t, h, s)						\
	(*(t)->bs_free)((t)->bs_cookie, (h), (s))


/*
 * Bus read (single) operations.
 */
#define	bus_space_read_1(t, h, o)	__bs_rs(1,(t),(h),(o))
#define	bus_space_read_2(t, h, o)	__bs_rs(2,(t),(h),(o))
#define	bus_space_read_4(t, h, o)	__bs_rs(4,(t),(h),(o))
#define	bus_space_read_8(t, h, o)	__bs_rs(8,(t),(h),(o))


/*
 * Bus read multiple operations.
 */
#define	bus_space_read_multi_1(t, h, o, a, c)				\
	__bs_nonsingle(rm,1,(t),(h),(o),(a),(c))
#define	bus_space_read_multi_2(t, h, o, a, c)				\
	__bs_nonsingle(rm,2,(t),(h),(o),(a),(c))
#define	bus_space_read_multi_4(t, h, o, a, c)				\
	__bs_nonsingle(rm,4,(t),(h),(o),(a),(c))
#define	bus_space_read_multi_8(t, h, o, a, c)				\
	__bs_nonsingle(rm,8,(t),(h),(o),(a),(c))


/*
 * Bus read region operations.
 */
#define	bus_space_read_region_1(t, h, o, a, c)				\
	__bs_nonsingle(rr,1,(t),(h),(o),(a),(c))
#define	bus_space_read_region_2(t, h, o, a, c)				\
	__bs_nonsingle(rr,2,(t),(h),(o),(a),(c))
#define	bus_space_read_region_4(t, h, o, a, c)				\
	__bs_nonsingle(rr,4,(t),(h),(o),(a),(c))
#define	bus_space_read_region_8(t, h, o, a, c)				\
	__bs_nonsingle(rr,8,(t),(h),(o),(a),(c))


/*
 * Bus write (single) operations.
 */
#define	bus_space_write_1(t, h, o, v)	__bs_ws(1,(t),(h),(o),(v))
#define	bus_space_write_2(t, h, o, v)	__bs_ws(2,(t),(h),(o),(v))
#define	bus_space_write_4(t, h, o, v)	__bs_ws(4,(t),(h),(o),(v))
#define	bus_space_write_8(t, h, o, v)	__bs_ws(8,(t),(h),(o),(v))


/*
 * Bus write multiple operations.
 */
#define	bus_space_write_multi_1(t, h, o, a, c)				\
	__bs_nonsingle(wm,1,(t),(h),(o),(a),(c))
#define	bus_space_write_multi_2(t, h, o, a, c)				\
	__bs_nonsingle(wm,2,(t),(h),(o),(a),(c))
#define	bus_space_write_multi_4(t, h, o, a, c)				\
	__bs_nonsingle(wm,4,(t),(h),(o),(a),(c))
#define	bus_space_write_multi_8(t, h, o, a, c)				\
	__bs_nonsingle(wm,8,(t),(h),(o),(a),(c))


/*
 * Bus write region operations.
 */
#define	bus_space_write_region_1(t, h, o, a, c)				\
	__bs_nonsingle(wr,1,(t),(h),(o),(a),(c))
#define	bus_space_write_region_2(t, h, o, a, c)				\
	__bs_nonsingle(wr,2,(t),(h),(o),(a),(c))
#define	bus_space_write_region_4(t, h, o, a, c)				\
	__bs_nonsingle(wr,4,(t),(h),(o),(a),(c))
#define	bus_space_write_region_8(t, h, o, a, c)				\
	__bs_nonsingle(wr,8,(t),(h),(o),(a),(c))


/*
 * Set multiple operations.
 */
/* XXX IMPLEMENT */


/*
 * Set region operations.
 */
/* XXX IMPLEMENT */


/*
 * Copy operations.
 */
/* XXX IMPLEMENT */


/*
 * Bus barrier operations.
 */
#define	bus_space_barrier(t, h, o, l, f)				\
	(*(t)->bs_barrier)((t)->bs_cookie, (h), (o), (l), (f))

#define	BUS_BARRIER_READ	0x01
#define	BUS_BARRIER_WRITE	0x02

#endif /* _ALPHA_BUS_H_ */
