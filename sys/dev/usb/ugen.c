/*	$NetBSD: ugen.c,v 1.4 1998/12/12 11:59:28 augustss Exp $	*/

/*
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (augustss@carlstedt.se) at
 * Carlstedt Research & Technology.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/device.h>
#include <sys/poll.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdi_util.h>

#ifdef USB_DEBUG
#define DPRINTF(x)	if (ugendebug) printf x
#define DPRINTFN(n,x)	if (ugendebug>(n)) printf x
int	ugendebug = 0;
#else
#define DPRINTF(x)
#define DPRINTFN(n,x)
#endif

struct ugen_endpoint {
	struct ugen_softc *sc;
	usb_endpoint_descriptor_t *edesc;
	usbd_interface_handle iface;
	int state;
#define UGEN_OPEN	0x01	/* device is open */
#define	UGEN_ASLP	0x02	/* waiting for data */
	usbd_pipe_handle pipeh;
	struct clist q;
	struct selinfo rsel;
	void *ibuf;
};

#define	UGEN_CHUNK	128	/* chunk size for read */
#define	UGEN_IBSIZE	1020	/* buffer size */
#define	UGEN_BBSIZE	1024

struct ugen_softc {
	struct device sc_dev;		/* base device */
	struct usbd_device *sc_udev;

	struct ugen_endpoint sc_endpoints[USB_MAX_ENDPOINTS][2];
#define OUT 0			/* index order is important, from UE_OUT */
#define IN  1			/* from UE_IN */

	int sc_disconnected;		/* device is gone */
};

int ugen_match __P((struct device *, struct cfdata *, void *));
void ugen_attach __P((struct device *, struct device *, void *));

int ugenopen __P((dev_t, int, int, struct proc *));
int ugenclose __P((dev_t, int, int, struct proc *p));
int ugenread __P((dev_t, struct uio *uio, int));
int ugenwrite __P((dev_t, struct uio *uio, int));
int ugenioctl __P((dev_t, u_long, caddr_t, int, struct proc *));
int ugenpoll __P((dev_t, int, struct proc *));
void ugenintr __P((usbd_request_handle reqh, usbd_private_handle addr, 
		   usbd_status status));
void ugen_disco __P((void *));

int ugen_set_config __P((struct ugen_softc *sc, int configno));
usb_config_descriptor_t *ugen_get_cdesc __P((struct ugen_softc *sc, int index,
					     int *lenp));
usbd_status ugen_set_interface __P((struct ugen_softc *, int, int));
int ugen_get_alt_index __P((struct ugen_softc *sc, int ifaceidx));

#define UGENUNIT(n) (((n) >> 4) & 0xf)
#define UGENENDPOINT(n) ((n) & 0xf)

extern struct cfdriver ugen_cd;

struct cfattach ugen_ca = {
	sizeof(struct ugen_softc), ugen_match, ugen_attach
};

int
ugen_match(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct usb_attach_arg *uaa = (struct usb_attach_arg *)aux;

	if (uaa->usegeneric)
		return (UMATCH_GENERIC);
	else
		return (UMATCH_NONE);
}

void
ugen_attach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
	struct ugen_softc *sc = (struct ugen_softc *)self;
	struct usb_attach_arg *uaa = (struct usb_attach_arg *)aux;
	char devinfo[1024];
	usbd_status r;
	
	usbd_devinfo(uaa->device, 0, devinfo);
	printf(": %s\n", devinfo);

	sc->sc_udev = uaa->device;
	r = ugen_set_config(sc, 1);
	if (r != USBD_NORMAL_COMPLETION) {
		printf("%s: setting configuration 0 failed\n", 
		       sc->sc_dev.dv_xname);
		sc->sc_disconnected = 1;
		return;
	}
}

int
ugen_set_config(sc, configno)
	struct ugen_softc *sc;
	int configno;
{
	usbd_device_handle dev = sc->sc_udev;
	usbd_interface_handle iface;
	usb_endpoint_descriptor_t *ed;
	struct ugen_endpoint *sce;
	u_int8_t niface, nendpt;
	int ifaceno, endptno, endpt;
	usbd_status r;

	DPRINTFN(1,("ugen_set_config: %s to configno %d, sc=%p\n",
		    sc->sc_dev.dv_xname, configno, sc));
	r = usbd_set_config_no(dev, configno, 0);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);

	r = usbd_interface_count(dev, &niface);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	memset(sc->sc_endpoints, 0, sizeof sc->sc_endpoints);
	for (ifaceno = 0; ifaceno < niface; ifaceno++) {
		DPRINTFN(1,("ugen_set_config: ifaceno %d\n", ifaceno));
		r = usbd_device2interface_handle(dev, ifaceno, &iface);
		if (r != USBD_NORMAL_COMPLETION)
			return (r);
		r = usbd_endpoint_count(iface, &nendpt);
		if (r != USBD_NORMAL_COMPLETION)
			return (r);
		for (endptno = 0; endptno < nendpt; endptno++) {
			ed = usbd_interface2endpoint_descriptor(iface,endptno);
			endpt = ed->bEndpointAddress;
			sce = &sc->sc_endpoints[UE_GET_ADDR(endpt)]
				               [UE_GET_IN(endpt)];
			DPRINTFN(1,("ugen_set_config: endptno %d, endpt=0x%02x"
				    "(%d,%d), sce=%p\n", 
				    endptno, endpt, UE_GET_ADDR(endpt),
				    UE_GET_IN(endpt), sce));
			sce->sc = sc;
			sce->edesc = ed;
			sce->iface = iface;
		}
	}
	return (USBD_NORMAL_COMPLETION);
}

void
ugen_disco(p)
	void *p;
{
	struct ugen_softc *sc = p;
	sc->sc_disconnected = 1;
}

int
ugenopen(dev, flag, mode, p)
	dev_t dev;
	int flag;
	int mode;
	struct proc *p;
{
	int unit = UGENUNIT(dev);
	int endpt = UGENENDPOINT(dev);
	usb_endpoint_descriptor_t *edesc;
	struct ugen_softc *sc;
	struct ugen_endpoint *sce;
	int dir, isize;
	usbd_status r;

	DPRINTFN(5, ("ugenopen: flag=%d, unit=%d endpt=%d\n", 
		     flag, unit, endpt));
	if (unit >= ugen_cd.cd_ndevs)
		return (ENXIO);
	sc = ugen_cd.cd_devs[unit];
	if (!sc)
		return (ENXIO);

	if (sc->sc_disconnected)
		return (EIO);

	if (endpt == USB_CONTROL_ENDPOINT) {
		/*if ((flag & (FWRITE|FREAD)) != (FWRITE|FREAD))
		  return (EACCES);*/
		sce = &sc->sc_endpoints[USB_CONTROL_ENDPOINT][OUT];
		if (sce->state & UGEN_OPEN)
			return (EBUSY);
	} else {
		switch (flag & (FWRITE|FREAD)) {
		case FWRITE:
			dir = OUT;
			break;
		case FREAD:
			dir = IN;
			break;
		default:
			return (EACCES);
		}
		sce = &sc->sc_endpoints[endpt][dir];
		DPRINTFN(5, ("ugenopen: sc=%p, endpt=%d, dir=%d, sce=%p\n", 
			     sc, endpt, dir, sce));
		if (sce->state & UGEN_OPEN)
			return (EBUSY);
		edesc = sce->edesc;
		if (!edesc)
			return (ENXIO);
		switch (edesc->bmAttributes & UE_XFERTYPE) {
		case UE_INTERRUPT:
			isize = UGETW(edesc->wMaxPacketSize);
			if (isize == 0)	/* shouldn't happen */
				return (EINVAL);
			sce->ibuf = malloc(isize, M_USB, M_NOWAIT);
			DPRINTFN(5, ("ugenopen: intr endpt=%d,isize=%d\n", 
				     endpt, isize));
			if (clalloc(&sce->q, UGEN_IBSIZE, 0) == -1)
				return (ENOMEM);
			r = usbd_open_pipe_intr(sce->iface, 
				edesc->bEndpointAddress, 
				USBD_SHORT_XFER_OK, &sce->pipeh, sce, 
				sce->ibuf, isize, ugenintr);
			if (r != USBD_NORMAL_COMPLETION) {
				free(sce->ibuf, M_USB);
				clfree(&sce->q);
				return (EIO);
			}
			usbd_set_disco(sce->pipeh, ugen_disco, sc);
			DPRINTFN(5, ("ugenopen: interrupt open done\n"));
			break;
		case UE_BULK:
			r = usbd_open_pipe(sce->iface, 
					   edesc->bEndpointAddress, 0, 
					   &sce->pipeh);
			if (r != USBD_NORMAL_COMPLETION)
				return (EIO);
			break;
		case UE_CONTROL:
		case UE_ISOCHRONOUS:
			return (EINVAL);
		}
	}
	sce->state |= UGEN_OPEN;
	return (0);
}

int
ugenclose(dev, flag, mode, p)
	dev_t dev;
	int flag;
	int mode;
	struct proc *p;
{
	struct ugen_softc *sc = ugen_cd.cd_devs[UGENUNIT(dev)];
	int endpt = UGENENDPOINT(dev);
	struct ugen_endpoint *sce;
	int dir;

	DPRINTFN(5, ("ugenclose\n"));
	if (sc->sc_disconnected)
		return (EIO);

	if (endpt == USB_CONTROL_ENDPOINT) {
		sc->sc_endpoints[endpt][OUT].state &= ~UGEN_OPEN;
		return (0);
	}
	dir = flag & FWRITE ? OUT : IN;
	sce = &sc->sc_endpoints[endpt][dir];
	sce->state &= ~UGEN_OPEN;

	usbd_abort_pipe(sce->pipeh);
	usbd_close_pipe(sce->pipeh);

	if (sce->ibuf) {
		free(sce->ibuf, M_USB);
		sce->ibuf = 0;
	}

	return (0);
}

int
ugenread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	struct ugen_softc *sc = ugen_cd.cd_devs[UGENUNIT(dev)];
	int endpt = UGENENDPOINT(dev);
	struct ugen_endpoint *sce = &sc->sc_endpoints[endpt][IN];
	size_t n;
	char buf[UGEN_BBSIZE];
	usbd_request_handle reqh;
	usbd_status r;
	int s;
	int error = 0;
	u_char buffer[UGEN_CHUNK];

	DPRINTFN(5, ("ugenread: %d:%d\n", UGENUNIT(dev), UGENENDPOINT(dev)));
	if (sc->sc_disconnected)
		return (EIO);

#ifdef DIAGNOSTIC
	if (!sce->edesc) {
		printf("ugenread: no edesc\n");
		return (EIO);
	}
#endif

	switch (sce->edesc->bmAttributes & UE_XFERTYPE) {
	case UE_INTERRUPT:
		/* Block until activity occured. */
		s = splusb();
		while (sce->q.c_cc == 0) {
			if (flag & IO_NDELAY) {
				splx(s);
				return EWOULDBLOCK;
			}
			sce->state |= UGEN_ASLP;
			DPRINTFN(5, ("ugenread: sleep on %p\n", sc));
			error = tsleep((caddr_t)sce, PZERO | PCATCH, 
				       "ugenrea", 0);
			DPRINTFN(5, ("ugenread: woke, error=%d\n", error));
			if (error) {
				sce->state &= ~UGEN_ASLP;
				splx(s);
				return (error);
			}
		}
		splx(s);

		/* Transfer as many chunks as possible. */
		while (sce->q.c_cc > 0 && uio->uio_resid > 0) {
			n = min(sce->q.c_cc, uio->uio_resid);
			if (n > sizeof(buffer))
				n = sizeof(buffer);

			/* Remove a small chunk from the input queue. */
			q_to_b(&sce->q, buffer, n);
			DPRINTFN(5, ("ugenread: got %d chars\n", n));

			/* Copy the data to the user process. */
			error = uiomove(buffer, n, uio);
			if (error)
				break;
		}
		break;
	case UE_BULK:
		reqh = usbd_alloc_request();
		if (reqh == 0)
			return (ENOMEM);
		while ((n = min(UGEN_BBSIZE, uio->uio_resid)) != 0) {
			/* XXX use callback to enable interrupt? */
			r = usbd_setup_request(reqh, sce->pipeh, 0, buf, n,
					       0, USBD_NO_TIMEOUT, 0);
			if (r != USBD_NORMAL_COMPLETION) {
				error = EIO;
				break;
			}
			DPRINTFN(1, ("ugenread: transfer %d bytes\n", n));
			r = usbd_sync_transfer(reqh);
			if (r != USBD_NORMAL_COMPLETION) {
				DPRINTF(("ugenread: error=%d\n", r));
				usbd_clear_endpoint_stall(sce->pipeh);
				error = EIO;
				break;
			}
			error = uiomove(buf, n, uio);
			if (error)
				break;
		}
		usbd_free_request(reqh);
		break;
	default:
		return (ENXIO);
	}

	return (error);
}

int
ugenwrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	struct ugen_softc *sc = ugen_cd.cd_devs[UGENUNIT(dev)];
	int endpt = UGENENDPOINT(dev);
	struct ugen_endpoint *sce = &sc->sc_endpoints[endpt][IN];
	size_t n;
	int error = 0;
	char buf[UGEN_BBSIZE];
	usbd_request_handle reqh;
	usbd_status r;

	if (sc->sc_disconnected)
		return (EIO);

#ifdef DIAGNOSTIC
	if (!sce->edesc) {
		printf("ugenwrite: no edesc\n");
		return (EIO);
	}
#endif

	DPRINTF(("ugenwrite\n"));
	switch (sce->edesc->bmAttributes & UE_XFERTYPE) {
	case UE_BULK:
		reqh = usbd_alloc_request();
		if (reqh == 0)
			return (EIO);
		while ((n = min(UGEN_BBSIZE, uio->uio_resid)) != 0) {
			error = uiomove(buf, n, uio);
			if (error)
				break;
			/* XXX use callback to enable interrupt? */
			r = usbd_setup_request(reqh, sce->pipeh, 0, buf, n,
					       0, USBD_NO_TIMEOUT, 0);
			if (r != USBD_NORMAL_COMPLETION) {
				error = EIO;
				break;
			}
			DPRINTFN(1, ("ugenwrite: transfer %d bytes\n", n));
			r = usbd_sync_transfer(reqh);
			if (r != USBD_NORMAL_COMPLETION) {
				DPRINTF(("ugenwrite: error=%d\n", r));
				usbd_clear_endpoint_stall(sce->pipeh);
				error = EIO;
				break;
			}
		}
		usbd_free_request(reqh);
		break;
	default:
		return (ENXIO);
	}
	return (error);
}


void
ugenintr(reqh, addr, status)
	usbd_request_handle reqh;
	usbd_private_handle addr;
	usbd_status status;
{
	struct ugen_endpoint *sce = addr;
	/*struct ugen_softc *sc = sce->sc;*/
	u_char *ibuf;
	int isize;

	if (status == USBD_CANCELLED)
		return;

	if (status != USBD_NORMAL_COMPLETION) {
		DPRINTF(("ugenintr: status=%d\n", status));
		usbd_clear_endpoint_stall_async(sce->pipeh);
		return;
	}

	ibuf = sce->ibuf;
	isize = UGETW(sce->edesc->wMaxPacketSize);

	/*DPRINTFN(5, ("ugenintr: addr=%d endpt=%d\n", 
		     addr, endpt, isize));
	DPRINTFN(5, ("          data = %02x %02x %02x\n",
	ibuf[0], ibuf[1], ibuf[2]));*/

	(void)b_to_q(ibuf, isize, &sce->q);
		
	if (sce->state & UGEN_ASLP) {
		sce->state &= ~UGEN_ASLP;
		DPRINTFN(5, ("ugen_intr: waking %p\n", sce));
		wakeup((caddr_t)sce);
	}
	selwakeup(&sce->rsel);
}

usbd_status
ugen_set_interface(sc, ifaceidx, altno)
	struct ugen_softc *sc;
	int ifaceidx, altno;
{
	usbd_interface_handle iface;
	usb_endpoint_descriptor_t *ed;
	usbd_status r;
	struct ugen_endpoint *sce;
	u_int8_t niface, nendpt, endptno, endpt;

	DPRINTFN(15, ("ugen_set_interface %d %d\n", ifaceidx, altno));

	r = usbd_interface_count(sc->sc_udev, &niface);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	if (ifaceidx < 0 || ifaceidx >= niface)
		return (USBD_INVAL);
	
	r = usbd_device2interface_handle(sc->sc_udev, ifaceidx, &iface);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	r = usbd_endpoint_count(iface, &nendpt);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	for (endptno = 0; endptno < nendpt; endptno++) {
		ed = usbd_interface2endpoint_descriptor(iface,endptno);
		endpt = ed->bEndpointAddress;
		sce = &sc->sc_endpoints[UE_GET_ADDR(endpt)][UE_GET_IN(endpt)];
		sce->sc = 0;
		sce->edesc = 0;
		sce->iface = 0;
	}

	/* change setting */
	r = usbd_set_interface(iface, altno);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);

	r = usbd_endpoint_count(iface, &nendpt);
	if (r != USBD_NORMAL_COMPLETION)
		return (r);
	for (endptno = 0; endptno < nendpt; endptno++) {
		ed = usbd_interface2endpoint_descriptor(iface,endptno);
		endpt = ed->bEndpointAddress;
		sce = &sc->sc_endpoints[UE_GET_ADDR(endpt)][UE_GET_IN(endpt)];
		sce->sc = sc;
		sce->edesc = ed;
		sce->iface = iface;
	}
	return (0);
}

/* Retrieve a complete descriptor for a certain device and index. */
usb_config_descriptor_t *
ugen_get_cdesc(sc, index, lenp)
	struct ugen_softc *sc;
	int index;
	int *lenp;
{
	usb_config_descriptor_t *cdesc, *tdesc, cdescr;
	int len;
	usbd_status r;

	if (index == USB_CURRENT_CONFIG_INDEX) {
		tdesc = usbd_get_config_descriptor(sc->sc_udev);
		len = UGETW(tdesc->wTotalLength);
		if (lenp)
			*lenp = len;
		cdesc = malloc(len, M_TEMP, M_WAITOK);
		memcpy(cdesc, tdesc, len);
		DPRINTFN(5,("ugen_get_cdesc: current, len=%d\n", len));
	} else {
		r = usbd_get_config_desc(sc->sc_udev, index, &cdescr);
		if (r != USBD_NORMAL_COMPLETION)
			return (0);
		len = UGETW(cdescr.wTotalLength);
		DPRINTFN(5,("ugen_get_cdesc: index=%d, len=%d\n", index, len));
		if (lenp)
			*lenp = len;
		cdesc = malloc(len, M_TEMP, M_WAITOK);
		r = usbd_get_config_desc_full(sc->sc_udev, index, cdesc, len);
		if (r != USBD_NORMAL_COMPLETION) {
			free(cdesc, M_TEMP);
			return (0);
		}
	}
	return (cdesc);
}

int
ugen_get_alt_index(sc, ifaceidx)
	struct ugen_softc *sc;
	int ifaceidx;
{
	usbd_interface_handle iface;
	usbd_status r;

	r = usbd_device2interface_handle(sc->sc_udev, ifaceidx, &iface);
	if (r != USBD_NORMAL_COMPLETION)
			return (-1);
	return (usbd_get_interface_altindex(iface));
}

int
ugenioctl(dev, cmd, addr, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t addr; 
	int flag;
	struct proc *p;
{
	struct ugen_softc *sc = ugen_cd.cd_devs[UGENUNIT(dev)];
	int endpt = UGENENDPOINT(dev);
	usbd_status r;
	usbd_interface_handle iface;
	struct usb_config_desc *cd;
	usb_config_descriptor_t *cdesc;
	struct usb_interface_desc *id;
	usb_interface_descriptor_t *idesc;
	struct usb_endpoint_desc *ed;
	usb_endpoint_descriptor_t *edesc;
	struct usb_alt_interface *ai;
	struct usb_string_desc *si;
	u_int8_t conf, alt;

	DPRINTFN(5, ("ugenioctl: cmd=%08lx\n", cmd));
	if (sc->sc_disconnected)
		return (EIO);

	switch (cmd) {
	case FIONBIO:
		/* All handled in the upper FS layer. */
		return (0);
	default:
		break;
	}

	if (endpt != USB_CONTROL_ENDPOINT)
		return (EINVAL);

	switch (cmd) {
	case USB_GET_CONFIG:
		r = usbd_get_config(sc->sc_udev, &conf);
		if (r != USBD_NORMAL_COMPLETION)
			return (EIO);
		*(int *)addr = conf;
		break;
	case USB_SET_CONFIG:
		if (!(flag & FWRITE))
			return (EPERM);
		r = usbd_set_config_no(sc->sc_udev, *(int *)addr, 0);
		if (r != USBD_NORMAL_COMPLETION)
			return (EIO);
		break;
	case USB_GET_ALTINTERFACE:
		ai = (struct usb_alt_interface *)addr;
		r = usbd_device2interface_handle(sc->sc_udev, 
						 ai->interface_index, &iface);
		if (r != USBD_NORMAL_COMPLETION)
			return (EINVAL);
		idesc = usbd_get_interface_descriptor(iface);
		if (!idesc)
			return (EIO);
		ai->alt_no = idesc->bAlternateSetting;
		break;
	case USB_SET_ALTINTERFACE:
		if (!(flag & FWRITE))
			return (EPERM);
		ai = (struct usb_alt_interface *)addr;
		r = usbd_device2interface_handle(sc->sc_udev, 
						 ai->interface_index, &iface);
		if (r != USBD_NORMAL_COMPLETION)
			return (EINVAL);
		r = ugen_set_interface(sc, ai->interface_index, ai->alt_no);
		if (r != USBD_NORMAL_COMPLETION)
			return (EINVAL);
		break;
	case USB_GET_NO_ALT:
		ai = (struct usb_alt_interface *)addr;
		cdesc = ugen_get_cdesc(sc, ai->config_index, 0);
		if (!cdesc)
			return (EINVAL);
		idesc = usbd_find_idesc(cdesc, ai->interface_index, 0);
		if (!idesc)
			return (EINVAL);
		ai->alt_no = usbd_get_no_alts(cdesc, idesc->bInterfaceNumber);
		break;
	case USB_GET_DEVICE_DESC:
		*(usb_device_descriptor_t *)addr =
			*usbd_get_device_descriptor(sc->sc_udev);
		break;
	case USB_GET_CONFIG_DESC:
		cd = (struct usb_config_desc *)addr;
		cdesc = ugen_get_cdesc(sc, cd->config_index, 0);
		if (!cdesc)
			return (EINVAL);
		cd->desc = *cdesc;
		free(cdesc, M_TEMP);
		break;
	case USB_GET_INTERFACE_DESC:
		id = (struct usb_interface_desc *)addr;
		cdesc = ugen_get_cdesc(sc, id->config_index, 0);
		if (!cdesc)
			return (EINVAL);
		if (id->config_index == USB_CURRENT_CONFIG_INDEX &&
		    id->alt_index == USB_CURRENT_ALT_INDEX)
			alt = ugen_get_alt_index(sc, id->interface_index);
		else
			alt = id->alt_index;
		idesc = usbd_find_idesc(cdesc, id->interface_index, alt);
		if (!idesc) {
			free(cdesc, M_TEMP);
			return (EINVAL);
		}
		id->desc = *idesc;
		free(cdesc, M_TEMP);
		break;
	case USB_GET_ENDPOINT_DESC:
		ed = (struct usb_endpoint_desc *)addr;
		cdesc = ugen_get_cdesc(sc, ed->config_index, 0);
		if (!cdesc)
			return (EINVAL);
		if (ed->config_index == USB_CURRENT_CONFIG_INDEX &&
		    ed->alt_index == USB_CURRENT_ALT_INDEX)
			alt = ugen_get_alt_index(sc, ed->interface_index);
		else
			alt = ed->alt_index;
		edesc = usbd_find_edesc(cdesc, ed->interface_index, 
					alt, ed->endpoint_index);
		if (!edesc) {
			free(cdesc, M_TEMP);
			return (EINVAL);
		}
		ed->desc = *edesc;
		free(cdesc, M_TEMP);
		break;
	case USB_GET_FULL_DESC:
	{
		int len;
		struct iovec iov;
		struct uio uio;
		struct usb_full_desc *fd = (struct usb_full_desc *)addr;
		int error;

		cdesc = ugen_get_cdesc(sc, fd->config_index, &len);
		if (len > fd->size)
			len = fd->size;
		iov.iov_base = (caddr_t)fd->data;
		iov.iov_len = len;
		uio.uio_iov = &iov;
		uio.uio_iovcnt = 1;
		uio.uio_resid = len;
		uio.uio_offset = 0;
		uio.uio_segflg = UIO_USERSPACE;
		uio.uio_rw = UIO_READ;
		uio.uio_procp = p;
		error = uiomove(cdesc, len, &uio);
		free(cdesc, M_TEMP);
		return (error);
	}
	case USB_GET_STRING_DESC:
		si = (struct usb_string_desc *)addr;
		r = usbd_get_string_desc(sc->sc_udev, si->string_index, 
					 si->language_id, &si->desc);
		if (r != USBD_NORMAL_COMPLETION)
			return (EINVAL);
		break;
	case USB_DO_REQUEST:
	{
		struct usb_ctl_request *ur = (void *)addr;
		int len = UGETW(ur->request.wLength);
		struct iovec iov;
		struct uio uio;
		void *ptr = 0;
		usbd_status r;
		int error = 0;

		if (!(flag & FWRITE))
			return (EPERM);
		/* Avoid requests that would damage the bus integrity. */
		if ((ur->request.bmRequestType == UT_WRITE_DEVICE &&
		     ur->request.bRequest == UR_SET_ADDRESS) ||
		    (ur->request.bmRequestType == UT_WRITE_DEVICE &&
		     ur->request.bRequest == UR_SET_CONFIG) ||
		    (ur->request.bmRequestType == UT_WRITE_INTERFACE &&
		     ur->request.bRequest == UR_SET_INTERFACE))
			return (EINVAL);

		if (len < 0 || len > 32767)
			return (EINVAL);
		if (len != 0) {
			iov.iov_base = (caddr_t)ur->data;
			iov.iov_len = len;
			uio.uio_iov = &iov;
			uio.uio_iovcnt = 1;
			uio.uio_resid = len;
			uio.uio_offset = 0;
			uio.uio_segflg = UIO_USERSPACE;
			uio.uio_rw =
				ur->request.bmRequestType & UT_READ ? 
				UIO_READ : UIO_WRITE;
			uio.uio_procp = p;
			ptr = malloc(len, M_TEMP, M_WAITOK);
			if (uio.uio_rw == UIO_WRITE) {
				error = uiomove(ptr, len, &uio);
				if (error)
					goto ret;
			}
		}
		r = usbd_do_request(sc->sc_udev, &ur->request, ptr);
		if (r) {
			error = EIO;
			goto ret;
		}
		if (len != 0) {
			if (uio.uio_rw == UIO_READ) {
				error = uiomove(ptr, len, &uio);
				if (error)
					goto ret;
			}
		}
	ret:
		if (ptr)
			free(ptr, M_TEMP);
		return (error);
	}
	case USB_GET_DEVICEINFO:
		usbd_fill_deviceinfo(sc->sc_udev,
				     (struct usb_device_info *)addr);
		break;
	default:
		return (EINVAL);
	}
	return (0);
}

int
ugenpoll(dev, events, p)
	dev_t dev;
	int events;
	struct proc *p;
{
	struct ugen_softc *sc = ugen_cd.cd_devs[UGENUNIT(dev)];
	/* XXX */
	struct ugen_endpoint *sce;
	int revents = 0;
	int s;

	if (sc->sc_disconnected)
		return (EIO);

	sce = &sc->sc_endpoints[UGENENDPOINT(dev)][IN];
	if (!sce->edesc) return (0); /* XXX */
	s = splusb();
	switch (sce->edesc->bmAttributes & UE_XFERTYPE) {
	case UE_INTERRUPT:
		if (events & (POLLIN | POLLRDNORM)) {
			if (sce->q.c_cc > 0)
				revents |= events & (POLLIN | POLLRDNORM);
			else
				selrecord(p, &sce->rsel);
		}
		break;
	case UE_BULK:
		/* 
		 * We have no easy way of determining if a read will
		 * yield any data or a write will happen.
		 * Pretend they will.
		 */
		revents |= events & 
			   (POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM);
		break;
	default:
		break;
	}
	splx(s);
	return (revents);
}
