/* $NetBSD: xcfb.c,v 1.24 2001/08/22 02:24:29 nisimura Exp $ */

/*
 * Copyright (c) 1998, 1999 Tohru Nishimura.  All rights reserved.
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
 *      This product includes software developed by Tohru Nishimura
 *	for the NetBSD Project.
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

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: xcfb.c,v 1.24 2001/08/22 02:24:29 nisimura Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/buf.h>
#include <sys/ioctl.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/wscons/wsconsio.h>
#include <dev/wscons/wsdisplayvar.h>

#include <dev/rasops/rasops.h>
#include <dev/wsfont/wsfont.h>

#include <dev/tc/tcvar.h>
#include <dev/tc/ioasicreg.h>
#include <dev/ic/ims332reg.h>
#include <pmax/pmax/maxine.h>

#include <uvm/uvm_extern.h>

struct hwcmap256 {
#define	CMAP_SIZE	256	/* 256 R/G/B entries */
	u_int8_t r[CMAP_SIZE];
	u_int8_t g[CMAP_SIZE];
	u_int8_t b[CMAP_SIZE];
};

struct hwcursor64 {
	struct wsdisplay_curpos cc_pos;
	struct wsdisplay_curpos cc_hot;
	struct wsdisplay_curpos cc_size;
	struct wsdisplay_curpos cc_magic;	/* not used by PMAG-DV */
#define	CURSOR_MAX_SIZE	64
	u_int8_t cc_color[6];
	u_int64_t cc_image[64 + 64];
};

#define	XCFB_FB_BASE	(XINE_PHYS_CFB_START + 0x2000000)
#define	XCFB_FB_SIZE	0x100000

#define	IMS332_HIGH	(IOASIC_SLOT_5_START)
#define	IMS332_RLOW	(IOASIC_SLOT_7_START)
#define	IMS332_WLOW	(IOASIC_SLOT_7_START + 0x20000)

struct xcfb_softc {
	struct device sc_dev;
	vaddr_t sc_vaddr;
	size_t sc_size;
	struct rasops_info *sc_ri;
	struct hwcmap256 sc_cmap;	/* software copy of colormap */
	struct hwcursor64 sc_cursor;	/* software copy of cursor */
	int sc_blanked;
	/* XXX MAXINE can take PMAG-DV vertical retrace interrupt XXX */
	int nscreens;
	/* cursor coordinate is located at upper-left corner */
	int sc_csr;			/* software copy of IMS332 CSR A */
};

static int  xcfbmatch __P((struct device *, struct cfdata *, void *));
static void xcfbattach __P((struct device *, struct device *, void *));

const struct cfattach xcfb_ca = {
	sizeof(struct xcfb_softc), xcfbmatch, xcfbattach,
};

static tc_addr_t xcfb_consaddr;
static struct rasops_info xcfb_console_ri;
static void xcfb_common_init __P((struct rasops_info *));
static void xcfbhwinit __P((caddr_t));
int xcfb_cnattach __P((void));

struct wsscreen_descr xcfb_stdscreen = {
	"std", 0, 0,
	0, /* textops */
	0, 0,
	WSSCREEN_REVERSE
};

static const struct wsscreen_descr *_xcfb_scrlist[] = {
	&xcfb_stdscreen,
};

static const struct wsscreen_list xcfb_screenlist = {
	sizeof(_xcfb_scrlist) / sizeof(struct wsscreen_descr *), _xcfb_scrlist
};

static int	xcfbioctl __P((void *, u_long, caddr_t, int, struct proc *));
static paddr_t	xcfbmmap __P((void *, off_t, int));

static int	xcfb_alloc_screen __P((void *, const struct wsscreen_descr *,
				       void **, int *, int *, long *));
static void	xcfb_free_screen __P((void *, void *));
static int	xcfb_show_screen __P((void *, void *, int,
				      void (*) (void *, int, int), void *));

static const struct wsdisplay_accessops xcfb_accessops = {
	xcfbioctl,
	xcfbmmap,
	xcfb_alloc_screen,
	xcfb_free_screen,
	xcfb_show_screen,
	0 /* load_font */
};

static int  xcfbintr __P((void *));
static void xcfb_screenblank __P((struct xcfb_softc *));
static int  set_cmap __P((struct xcfb_softc *, struct wsdisplay_cmap *));
static int  get_cmap __P((struct xcfb_softc *, struct wsdisplay_cmap *));
static int  set_cursor __P((struct xcfb_softc *, struct wsdisplay_cursor *));
static int  get_cursor __P((struct xcfb_softc *, struct wsdisplay_cursor *));
static void set_curpos __P((struct xcfb_softc *, struct wsdisplay_curpos *));
static void ims332_loadcmap __P((struct hwcmap256 *));
static void ims332_set_curpos __P((struct xcfb_softc *));
static void ims332_load_curcmap __P((struct xcfb_softc *));
static void ims332_load_curshape __P((struct xcfb_softc *));
static void ims332_write_reg __P((int, u_int32_t));
#if 0
static u_int32_t ims332_read_reg __P((int));
#endif

extern long ioasic_base;	/* XXX */

/*
 * Compose 2 bit/pixel cursor image.  
 *   M M M M I I I I		M I M I M I M I
 *	[ before ]		   [ after ]
 *   3 2 1 0 3 2 1 0		3 3 2 2 1 1 0 0
 *   7 6 5 4 7 6 5 4		7 7 6 6 5 5 4 4
 */
static const u_int8_t shuffle[256] = {
	0x00, 0x01, 0x04, 0x05, 0x10, 0x11, 0x14, 0x15,
	0x40, 0x41, 0x44, 0x45, 0x50, 0x51, 0x54, 0x55,
	0x02, 0x03, 0x06, 0x07, 0x12, 0x13, 0x16, 0x17,
	0x42, 0x43, 0x46, 0x47, 0x52, 0x53, 0x56, 0x57,
	0x08, 0x09, 0x0c, 0x0d, 0x18, 0x19, 0x1c, 0x1d,
	0x48, 0x49, 0x4c, 0x4d, 0x58, 0x59, 0x5c, 0x5d,
	0x0a, 0x0b, 0x0e, 0x0f, 0x1a, 0x1b, 0x1e, 0x1f,
	0x4a, 0x4b, 0x4e, 0x4f, 0x5a, 0x5b, 0x5e, 0x5f,
	0x20, 0x21, 0x24, 0x25, 0x30, 0x31, 0x34, 0x35,
	0x60, 0x61, 0x64, 0x65, 0x70, 0x71, 0x74, 0x75,
	0x22, 0x23, 0x26, 0x27, 0x32, 0x33, 0x36, 0x37,
	0x62, 0x63, 0x66, 0x67, 0x72, 0x73, 0x76, 0x77,
	0x28, 0x29, 0x2c, 0x2d, 0x38, 0x39, 0x3c, 0x3d,
	0x68, 0x69, 0x6c, 0x6d, 0x78, 0x79, 0x7c, 0x7d,
	0x2a, 0x2b, 0x2e, 0x2f, 0x3a, 0x3b, 0x3e, 0x3f,
	0x6a, 0x6b, 0x6e, 0x6f, 0x7a, 0x7b, 0x7e, 0x7f,
	0x80, 0x81, 0x84, 0x85, 0x90, 0x91, 0x94, 0x95,
	0xc0, 0xc1, 0xc4, 0xc5, 0xd0, 0xd1, 0xd4, 0xd5,
	0x82, 0x83, 0x86, 0x87, 0x92, 0x93, 0x96, 0x97,
	0xc2, 0xc3, 0xc6, 0xc7, 0xd2, 0xd3, 0xd6, 0xd7,
	0x88, 0x89, 0x8c, 0x8d, 0x98, 0x99, 0x9c, 0x9d,
	0xc8, 0xc9, 0xcc, 0xcd, 0xd8, 0xd9, 0xdc, 0xdd,
	0x8a, 0x8b, 0x8e, 0x8f, 0x9a, 0x9b, 0x9e, 0x9f,
	0xca, 0xcb, 0xce, 0xcf, 0xda, 0xdb, 0xde, 0xdf,
	0xa0, 0xa1, 0xa4, 0xa5, 0xb0, 0xb1, 0xb4, 0xb5,
	0xe0, 0xe1, 0xe4, 0xe5, 0xf0, 0xf1, 0xf4, 0xf5,
	0xa2, 0xa3, 0xa6, 0xa7, 0xb2, 0xb3, 0xb6, 0xb7,
	0xe2, 0xe3, 0xe6, 0xe7, 0xf2, 0xf3, 0xf6, 0xf7,
	0xa8, 0xa9, 0xac, 0xad, 0xb8, 0xb9, 0xbc, 0xbd,
	0xe8, 0xe9, 0xec, 0xed, 0xf8, 0xf9, 0xfc, 0xfd,
	0xaa, 0xab, 0xae, 0xaf, 0xba, 0xbb, 0xbe, 0xbf,
	0xea, 0xeb, 0xee, 0xef, 0xfa, 0xfb, 0xfe, 0xff,
};

static int
xcfbmatch(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct tc_attach_args *ta = aux;

	if (strncmp("PMAG-DV ", ta->ta_modname, TC_ROM_LLEN) != 0)
		return (0);

	return (1);
}

static void
xcfbattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct xcfb_softc *sc = (struct xcfb_softc *)self;
	struct tc_attach_args *ta = aux;
	struct rasops_info *ri;
	struct wsemuldisplaydev_attach_args waa;
	struct hwcmap256 *cm;
	const u_int8_t *p;
	int console, index;

	console = (ta->ta_addr == xcfb_consaddr);
	if (console) {
		sc->sc_ri = ri = &xcfb_console_ri;
		sc->nscreens = 1;
	}
	else {
		MALLOC(ri, struct rasops_info *, sizeof(struct rasops_info),
			M_DEVBUF, M_NOWAIT);
		if (ri == NULL) {
			printf(": can't alloc memory\n");
			return;
		}
		memset(ri, 0, sizeof(struct rasops_info));

		ri->ri_hw = (void *)ioasic_base;
		xcfb_common_init(ri);
		sc->sc_ri = ri;
	}
	printf(": %dx%d, %dbpp\n", ri->ri_width, ri->ri_height, ri->ri_depth);

	cm = &sc->sc_cmap;
	p = rasops_cmap;
	for (index = 0; index < CMAP_SIZE; index++, p += 3) {
		cm->r[index] = p[0];
		cm->g[index] = p[1];
		cm->b[index] = p[2];
	}

	sc->sc_vaddr = ta->ta_addr;
	sc->sc_blanked = 0;
	sc->sc_csr = IMS332_BPP_8 | IMS332_CSR_A_VTG_ENABLE;

        tc_intr_establish(parent, ta->ta_cookie, IPL_TTY, xcfbintr, sc);

	waa.console = console;
	waa.scrdata = &xcfb_screenlist;
	waa.accessops = &xcfb_accessops;
	waa.accesscookie = sc;

	config_found(self, &waa, wsemuldisplaydevprint);
}

static void
xcfb_common_init(ri)
	struct rasops_info *ri;
{
	int cookie;

	/* initialize colormap and cursor hardware */
	xcfbhwinit((caddr_t)ri->ri_hw);

	ri->ri_flg = RI_CENTER;
	ri->ri_depth = 8;
	ri->ri_width = 1024;
	ri->ri_height = 768;
	ri->ri_stride = 1024;
	ri->ri_bits = (caddr_t)MIPS_PHYS_TO_KSEG1(XCFB_FB_BASE);

	/* clear the screen */
	memset(ri->ri_bits, 0, ri->ri_stride * ri->ri_height);

	wsfont_init();
	/* prefer 12 pixel wide font */
	if ((cookie = wsfont_find(NULL, 12, 0, 0)) <= 0)
		cookie = wsfont_find(NULL, 0, 0, 0);
	if (cookie <= 0) {
		printf("xcfb: font table is empty\n");
		return;
	}

	if (wsfont_lock(cookie, &ri->ri_font,
	    WSDISPLAY_FONTORDER_L2R, WSDISPLAY_FONTORDER_L2R) <= 0) {
		printf("xcfb: couldn't lock font\n");
		return;
	}
	ri->ri_wsfcookie = cookie;

	rasops_init(ri, 34, 80);

	/* XXX shouldn't be global */
	xcfb_stdscreen.nrows = ri->ri_rows;
	xcfb_stdscreen.ncols = ri->ri_cols;
	xcfb_stdscreen.textops = &ri->ri_ops;
	xcfb_stdscreen.capabilities = ri->ri_caps;
}

int
xcfb_cnattach()
{
	struct rasops_info *ri;
	long defattr;

	ri = &xcfb_console_ri;
	ri->ri_hw = (void *)ioasic_base;
	xcfb_common_init(ri);
	(*ri->ri_ops.alloc_attr)(ri, 0, 0, 0, &defattr);
	wsdisplay_cnattach(&xcfb_stdscreen, ri, 0, 0, defattr);
	xcfb_consaddr = MIPS_PHYS_TO_KSEG1(XINE_PHYS_CFB_START);
	return (0);
}

static void
xcfbhwinit(base)
	caddr_t base;
{
	u_int32_t *csr, i;
	const u_int8_t *p;

	csr = (u_int32_t *)(base + IOASIC_CSR);
	i = *csr;
	i &= ~XINE_CSR_VDAC_ENABLE;
	*csr = i;
	DELAY(50);
	i |= XINE_CSR_VDAC_ENABLE;
	*csr = i;
	DELAY(50);
	ims332_write_reg(IMS332_REG_BOOT, 0x2c);
	ims332_write_reg(IMS332_REG_CSR_A,
		IMS332_BPP_8|IMS332_CSR_A_DISABLE_CURSOR);
	ims332_write_reg(IMS332_REG_HALF_SYNCH, 0x10);
	ims332_write_reg(IMS332_REG_BACK_PORCH, 0x21);
	ims332_write_reg(IMS332_REG_DISPLAY, 0x100);
	ims332_write_reg(IMS332_REG_SHORT_DIS, 0x5d);
	ims332_write_reg(IMS332_REG_BROAD_PULSE, 0x9f);
	ims332_write_reg(IMS332_REG_LINE_TIME, 0x146);
	ims332_write_reg(IMS332_REG_V_SYNC, 0x0c);
	ims332_write_reg(IMS332_REG_V_PRE_EQUALIZE, 0x02);
	ims332_write_reg(IMS332_REG_V_POST_EQUALIZE, 0x02);
	ims332_write_reg(IMS332_REG_V_BLANK, 0x2a);
	ims332_write_reg(IMS332_REG_V_DISPLAY, 0x600);
	ims332_write_reg(IMS332_REG_LINE_START, 0x10);
	ims332_write_reg(IMS332_REG_MEM_INIT, 0x0a);
	ims332_write_reg(IMS332_REG_COLOR_MASK, 0xffffff);
	ims332_write_reg(IMS332_REG_CSR_A,
		IMS332_BPP_8|IMS332_CSR_A_VTG_ENABLE);

	/* build sane colormap */
	p = rasops_cmap;
	for (i = 0; i < CMAP_SIZE; i++, p += 3) {
		u_int32_t bgr;

		bgr = p[2] << 16 | p[1] << 8 | p[0];
		ims332_write_reg(IMS332_REG_LUT_BASE + i, bgr);
	}

	/* clear out cursor image */
	for (i = 0; i < 512; i++)
		ims332_write_reg(IMS332_REG_CURSOR_RAM + i, 0);

	/*
	 * 2 bit/pixel cursor.  Assign MSB for cursor mask and LSB for
	 * cursor image.  LUT_1 for mask color, while LUT_2 for
	 * image color.  LUT_0 will be never used.
	 */
	ims332_write_reg(IMS332_REG_CURSOR_LUT_0, 0);
	ims332_write_reg(IMS332_REG_CURSOR_LUT_1, 0xffffff);
	ims332_write_reg(IMS332_REG_CURSOR_LUT_2, 0xffffff);
}

static int
xcfbioctl(v, cmd, data, flag, p)
	void *v;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	struct xcfb_softc *sc = v;
	struct rasops_info *ri = sc->sc_ri;
	int turnoff, error;

	switch (cmd) {
	case WSDISPLAYIO_GTYPE:
		*(u_int *)data = WSDISPLAY_TYPE_XCFB;
		return (0);

	case WSDISPLAYIO_GINFO:
#define	wsd_fbip ((struct wsdisplay_fbinfo *)data)
		wsd_fbip->height = ri->ri_height;
		wsd_fbip->width = ri->ri_width;
		wsd_fbip->depth = ri->ri_depth;
		wsd_fbip->cmsize = CMAP_SIZE;
#undef fbt
		return (0);

	case WSDISPLAYIO_GETCMAP:
		return get_cmap(sc, (struct wsdisplay_cmap *)data);

	case WSDISPLAYIO_PUTCMAP:
		error = set_cmap(sc, (struct wsdisplay_cmap *)data);
		if (error == 0)
			ims332_loadcmap(&sc->sc_cmap);
		return (error);

	case WSDISPLAYIO_SVIDEO:
		turnoff = *(int *)data == WSDISPLAYIO_VIDEO_OFF;
		if ((sc->sc_blanked == 0) ^ turnoff) {
			sc->sc_blanked = turnoff;
			xcfb_screenblank(sc);
		}
		return (0);

	case WSDISPLAYIO_GVIDEO:
		*(u_int *)data = sc->sc_blanked ?
		    WSDISPLAYIO_VIDEO_OFF : WSDISPLAYIO_VIDEO_ON;
		return (0);

	case WSDISPLAYIO_GCURPOS:
		*(struct wsdisplay_curpos *)data = sc->sc_cursor.cc_pos;
		return (0);

	case WSDISPLAYIO_SCURPOS:
		set_curpos(sc, (struct wsdisplay_curpos *)data);
		ims332_set_curpos(sc);
		return (0);

	case WSDISPLAYIO_GCURMAX:
		((struct wsdisplay_curpos *)data)->x =
		((struct wsdisplay_curpos *)data)->y = CURSOR_MAX_SIZE;
		return (0);

	case WSDISPLAYIO_GCURSOR:
		return get_cursor(sc, (struct wsdisplay_cursor *)data);

	case WSDISPLAYIO_SCURSOR:
		return set_cursor(sc, (struct wsdisplay_cursor *)data);
	}
	return (ENOTTY);
}

static paddr_t
xcfbmmap(v, offset, prot)
	void *v;
	off_t offset;
	int prot;
{

	if (offset >= XCFB_FB_SIZE || offset < 0)
		return (-1);
	return mips_btop(MIPS_KSEG1_TO_PHYS(XCFB_FB_BASE + offset));
}

static int
xcfb_alloc_screen(v, type, cookiep, curxp, curyp, attrp)
	void *v;
	const struct wsscreen_descr *type;
	void **cookiep;
	int *curxp, *curyp;
	long *attrp;
{
	struct xcfb_softc *sc = v;
	struct rasops_info *ri = sc->sc_ri;
	long defattr;

	if (sc->nscreens > 0)
		return (ENOMEM);

	*cookiep = ri; 		/* one and only for now */
	*curxp = 0;
	*curyp = 0;
	(*ri->ri_ops.alloc_attr)(ri, 0, 0, 0, &defattr);
	*attrp = defattr;
	sc->nscreens++;
	return (0);
}

static void
xcfb_free_screen(v, cookie)
	void *v;
	void *cookie;
{
	struct xcfb_softc *sc = v;

	if (sc->sc_ri == &xcfb_console_ri)
		panic("xcfb_free_screen: console");

	sc->nscreens--;
}

static int
xcfb_show_screen(v, cookie, waitok, cb, cbarg)
	void *v;
	void *cookie;
	int waitok;
	void (*cb) __P((void *, int, int));
	void *cbarg;
{

	return (0);
}

static int
xcfbintr(v)
	void *v;
{
	struct xcfb_softc *sc = v;
	u_int32_t *intr, i;

	intr = (u_int32_t *)((caddr_t)sc->sc_ri->ri_hw + IOASIC_INTR);
	i = *intr;
	i &= ~XINE_INTR_VINT;
	*intr = i;
	return (1);
}

static void
xcfb_screenblank(sc)
	struct xcfb_softc *sc;
{
	if (sc->sc_blanked)
		sc->sc_csr |= IMS332_CSR_A_FORCE_BLANK;
	else
		sc->sc_csr &= ~IMS332_CSR_A_FORCE_BLANK;
	ims332_write_reg(IMS332_REG_CSR_A, sc->sc_csr);
}

static int
get_cmap(sc, p)
	struct xcfb_softc *sc;
	struct wsdisplay_cmap *p;
{
	u_int index = p->index, count = p->count;

	if (index >= CMAP_SIZE || (index + count) > CMAP_SIZE)
		return (EINVAL);

	if (!uvm_useracc(p->red, count, B_WRITE) ||
	    !uvm_useracc(p->green, count, B_WRITE) ||
	    !uvm_useracc(p->blue, count, B_WRITE))
		return (EFAULT);

	copyout(&sc->sc_cmap.r[index], p->red, count);
	copyout(&sc->sc_cmap.g[index], p->green, count);
	copyout(&sc->sc_cmap.b[index], p->blue, count);

	return (0);
}

static int
set_cmap(sc, p)
	struct xcfb_softc *sc;
	struct wsdisplay_cmap *p;
{
	u_int index = p->index, count = p->count;

	if (index >= CMAP_SIZE || (index + count) > CMAP_SIZE)
		return (EINVAL);

	if (!uvm_useracc(p->red, count, B_READ) ||
	    !uvm_useracc(p->green, count, B_READ) ||
	    !uvm_useracc(p->blue, count, B_READ))
		return (EFAULT);

	copyin(p->red, &sc->sc_cmap.r[index], count);
	copyin(p->green, &sc->sc_cmap.g[index], count);
	copyin(p->blue, &sc->sc_cmap.b[index], count);

	return (0);
}

static int
set_cursor(sc, p)
	struct xcfb_softc *sc;
	struct wsdisplay_cursor *p;
{
#define	cc (&sc->sc_cursor)
	u_int v, index, count;

	v = p->which;
	if (v & WSDISPLAY_CURSOR_DOCMAP) {
		index = p->cmap.index;
		count = p->cmap.count;

		if (index >= 2 || index + count > 2)
			return (EINVAL);
		if (!uvm_useracc(p->cmap.red, count, B_READ) ||
		    !uvm_useracc(p->cmap.green, count, B_READ) ||
		    !uvm_useracc(p->cmap.blue, count, B_READ))
			return (EFAULT);

		copyin(p->cmap.red, &cc->cc_color[index], count);
		copyin(p->cmap.green, &cc->cc_color[index + 2], count);
		copyin(p->cmap.blue, &cc->cc_color[index + 4], count);
		ims332_load_curcmap(sc);
	}
	if (v & WSDISPLAY_CURSOR_DOSHAPE) {
		if (p->size.x > CURSOR_MAX_SIZE || p->size.y > CURSOR_MAX_SIZE)
			return (EINVAL);
		count = ((p->size.x < 33) ? 4 : 8) * p->size.y;
		if (!uvm_useracc(p->image, count, B_READ) ||
		    !uvm_useracc(p->mask, count, B_READ))
			return (EFAULT);
		cc->cc_size = p->size;
		memset(cc->cc_image, 0, sizeof cc->cc_image);
		copyin(p->image, cc->cc_image, count);
		copyin(p->mask, cc->cc_image+CURSOR_MAX_SIZE, count);
		ims332_load_curshape(sc);
	}
	if (v & WSDISPLAY_CURSOR_DOCUR) {
		cc->cc_hot = p->hot;
		if (p->enable)
			sc->sc_csr &= ~IMS332_CSR_A_DISABLE_CURSOR;
		else
			sc->sc_csr |= IMS332_CSR_A_DISABLE_CURSOR;
		ims332_write_reg(IMS332_REG_CSR_A, sc->sc_csr);
	}
	if (v & WSDISPLAY_CURSOR_DOPOS) {
		set_curpos(sc, &p->pos);
		ims332_set_curpos(sc);
	}

	return (0);
#undef cc
}

static int
get_cursor(sc, p)
	struct xcfb_softc *sc;
	struct wsdisplay_cursor *p;
{
	return (ENOTTY); /* XXX */
}

static void
set_curpos(sc, curpos)
	struct xcfb_softc *sc;
	struct wsdisplay_curpos *curpos;
{
	struct rasops_info *ri = sc->sc_ri;
	int x = curpos->x, y = curpos->y;

	if (y < 0)
		y = 0;
	else if (y > ri->ri_height)
		y = ri->ri_height;
	if (x < 0)
		x = 0;
	else if (x > ri->ri_width)
		x = ri->ri_width;
	sc->sc_cursor.cc_pos.x = x;
	sc->sc_cursor.cc_pos.y = y;
}

static void
ims332_loadcmap(cm)
	struct hwcmap256 *cm;
{
	int i;
	u_int32_t rgb;
	
	for (i = 0; i < CMAP_SIZE; i++) {
		rgb = cm->b[i] << 16 | cm->g[i] << 8 | cm->r[i];
		ims332_write_reg(IMS332_REG_LUT_BASE + i, rgb);
	}
}

static void
ims332_set_curpos(sc)
	struct xcfb_softc *sc;
{
	struct wsdisplay_curpos *curpos = &sc->sc_cursor.cc_pos;
	u_int32_t pos;
	int s;

	s = spltty();
	pos = (curpos->x & 0xfff) << 12 | (curpos->y & 0xfff);
	ims332_write_reg(IMS332_REG_CURSOR_LOC, pos);
	splx(s);
}

static void
ims332_load_curcmap(sc)
	struct xcfb_softc *sc;
{
	u_int8_t *cp = sc->sc_cursor.cc_color;
	u_int32_t rgb;

	/* cursor background */
	rgb = cp[5] << 16 | cp[3] << 8 | cp[1];
	ims332_write_reg(IMS332_REG_CURSOR_LUT_1, rgb);

	/* cursor foreground */
	rgb = cp[4] << 16 | cp[2] << 8 | cp[0];
	ims332_write_reg(IMS332_REG_CURSOR_LUT_2, rgb);
}

static void
ims332_load_curshape(sc)
	struct xcfb_softc *sc;
{
	unsigned i, img, msk, bits;
	u_int8_t u, *ip, *mp;

	ip = (u_int8_t *)sc->sc_cursor.cc_image;
	mp = (u_int8_t *)(sc->sc_cursor.cc_image+CURSOR_MAX_SIZE);

	i = 0;
	/* 64 pixel scan line is consisted with 8 halfword cursor ram */
	while (i < sc->sc_cursor.cc_size.y * 8) {
		/* pad right half 32 pixel when smaller than 33 */
		if ((i & 0x4) && sc->sc_cursor.cc_size.x < 33)
			bits = 0;
		else {
			img = *ip++;
			msk = *mp++;
			img &= msk;	/* cookie off image */
			u = (msk & 0x0f) << 4 | (img & 0x0f);
			bits = shuffle[u];
			u = (msk & 0xf0) | (img & 0xf0) >> 4;
			bits = (shuffle[u] << 8) | bits;
		}
		ims332_write_reg(IMS332_REG_CURSOR_RAM + i, bits);
		i += 1;
	}
	/* pad unoccupied scan lines */
	while (i < CURSOR_MAX_SIZE * 8) {
		ims332_write_reg(IMS332_REG_CURSOR_RAM + i, 0);
		i += 1;
	}
}

static void
ims332_write_reg(regno, val)
	int regno;
	u_int32_t val;
{
	caddr_t high8 = (caddr_t)(ioasic_base + IMS332_HIGH);
	caddr_t low16 = (caddr_t)(ioasic_base + IMS332_WLOW) + (regno << 4);

	*(volatile u_int16_t *)high8 = (val & 0xff0000) >> 8;
	*(volatile u_int16_t *)low16 = val;
}

#if 0
static u_int32_t
ims332_read_reg(regno)
	int regno;
{
	caddr_t high8 = (caddr_t)(ioasic_base + IMS332_HIGH);
	caddr_t low16 = (caddr_t)(ioasic_base + IMS332_RLOW) + (regno << 4);
	u_int v0, v1;

	v1 = *(volatile u_int16_t *)high8;
	v0 = *(volatile u_int16_t *)low16;
	return (v1 & 0xff00) << 8 | v0;
}
#endif
