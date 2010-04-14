/* $NetBSD: acpi_wakedev.c,v 1.10 2010/04/14 17:12:14 jruoho Exp $ */

/*-
 * Copyright (c) 2009 Jared D. McNeill <jmcneill@invisible.ca>
 * All rights reserved.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: acpi_wakedev.c,v 1.10 2010/04/14 17:12:14 jruoho Exp $");

#include <sys/param.h>
#include <sys/device.h>
#include <sys/sysctl.h>
#include <sys/systm.h>

#include <dev/acpi/acpireg.h>
#include <dev/acpi/acpivar.h>
#include <dev/acpi/acpi_wakedev.h>

#define _COMPONENT		   ACPI_BUS_COMPONENT
ACPI_MODULE_NAME		   ("acpi_wakedev")

static const char * const acpi_wakedev_default[] = {
	"PNP0C0C",	/* power button */
	"PNP0C0E",	/* sleep button */
	"PNP0C0D",	/* lid switch */
	"PNP03??",	/* PC KBD port */
	NULL,
};

static const struct sysctlnode *rnode = NULL;

static void	acpi_wakedev_prepare(struct acpi_devnode *, int, int);
static void	acpi_wakedev_gpe(ACPI_HANDLE, bool);


SYSCTL_SETUP(sysctl_acpi_wakedev_setup, "sysctl hw.acpi.wake subtree setup")
{
	int err;

	err = sysctl_createv(NULL, 0, NULL, &rnode,
	    CTLFLAG_PERMANENT, CTLTYPE_NODE, "hw",
	    NULL, NULL, 0, NULL, 0,
	    CTL_HW, CTL_EOL);

	if (err != 0)
		goto fail;

	err = sysctl_createv(NULL, 0, &rnode, &rnode,
	    CTLFLAG_PERMANENT, CTLTYPE_NODE, "acpi",
	    NULL, NULL, 0, NULL, 0,
	    CTL_CREATE, CTL_EOL);

	if (err != 0)
		goto fail;

	err = sysctl_createv(NULL, 0, &rnode, &rnode,
	    CTLFLAG_PERMANENT, CTLTYPE_NODE,
	    "wake", SYSCTL_DESCR("ACPI device wake-up"),
	    NULL, 0, NULL, 0,
	    CTL_CREATE, CTL_EOL);

	if (err != 0)
		goto fail;

	return;

fail:
	rnode = NULL;
}

void
acpi_wakedev_add(struct acpi_devnode *ad)
{
	int err;

	KASSERT(ad != NULL && ad->ad_parent != NULL);
	KASSERT((ad->ad_flags & ACPI_DEVICE_WAKEUP) != 0);

	ad->ad_wake = 0;

	if (acpi_match_hid(ad->ad_devinfo, acpi_wakedev_default))
		ad->ad_wake = 1;

	if (rnode == NULL)
		return;

	err = sysctl_createv(NULL, 0, &rnode, NULL,
	    CTLFLAG_READWRITE, CTLTYPE_BOOL, ad->ad_name,
	    NULL, NULL, 0, &ad->ad_wake, 0,
	    CTL_CREATE, CTL_EOL);

	if (err != 0)
		aprint_error_dev(ad->ad_parent, "sysctl_createv"
		    "(hw.acpi.wake.%s) failed (err %d)\n", ad->ad_name, err);
}

void
acpi_wakedev_commit(struct acpi_softc *sc, int state)
{
	struct acpi_devnode *ad;

	/*
	 * As noted in ACPI 3.0 (p. 243), preparing
	 * a device for wakeup is a two-step process:
	 *
	 *  1.	Enable all power resources in _PRW.
	 *
	 *  2.	If present, execute _DSW/_PSW method.
	 *
	 * XXX: The first one is yet to be implemented.
	 */
	SIMPLEQ_FOREACH(ad, &sc->sc_devnodes, ad_list) {

		if ((ad->ad_flags & ACPI_DEVICE_WAKEUP) == 0)
			continue;

		if (ad->ad_wake == 0)
			acpi_wakedev_gpe(ad->ad_handle, false);
		else {
			aprint_debug_dev(ad->ad_parent,
			    "set wake GPE for %s\n", ad->ad_name);
			acpi_wakedev_gpe(ad->ad_handle, true);
		}

		acpi_wakedev_prepare(ad, ad->ad_wake, state);
	}
}

static void
acpi_wakedev_prepare(struct acpi_devnode *ad, int enable, int state)
{
	ACPI_OBJECT_LIST arg;
	ACPI_OBJECT obj[3];
	ACPI_STATUS rv;

	/*
	 * First try to call the Device Sleep Wake control method, _DSW.
	 * Only if this is not available, resort to to the Power State
	 * Wake control method, _PSW, which was deprecated in ACPI 3.0.
	 *
	 * The arguments to these methods are as follows:
	 *
	 *		arg0		arg1		arg2
	 *		----		----		----
	 *	 _PSW	0: disable
	 *		1: enable
	 *
	 *	 _DSW	0: disable	0: S0		0: D0
	 *		1: enable	1: S1		1: D0 or D1
	 *						2: D0, D1, or D2
	 *				x: Sx		3: D0, D1, D2 or D3
	 */
	arg.Count = 3;
	arg.Pointer = obj;

	obj[0].Integer.Value = enable;
	obj[1].Integer.Value = state;
	obj[2].Integer.Value = 3;

	obj[0].Type = obj[1].Type = obj[2].Type = ACPI_TYPE_INTEGER;

	rv = AcpiEvaluateObject(ad->ad_handle, "_DSW", &arg, NULL);

	if (ACPI_SUCCESS(rv))
		return;

	if (rv != AE_NOT_FOUND)
		goto fail;

	rv = acpi_eval_set_integer(ad->ad_handle, "_PSW", enable);

	if (ACPI_FAILURE(rv) && rv != AE_NOT_FOUND)
		goto fail;

	return;

fail:
	aprint_error_dev(ad->ad_parent, "failed to evaluate wake "
	    "control method: %s\n", AcpiFormatException(rv));
}

static void
acpi_wakedev_gpe(ACPI_HANDLE handle, bool enable)
{
	ACPI_OBJECT *elm, *obj;
	ACPI_INTEGER val;
	ACPI_BUFFER buf;
	ACPI_STATUS rv;

	rv = acpi_eval_struct(handle, METHOD_NAME__PRW, &buf);

	if (ACPI_FAILURE(rv))
		return;

	obj = buf.Pointer;

	if (obj->Type != ACPI_TYPE_PACKAGE || obj->Package.Count < 2)
		goto out;

	/*
	 * As noted in ACPI 3.0 (section 7.2.10), the _PRW object is
	 * a package in which the first element is either an integer
	 * or again a package. In the latter case the package inside
	 * the package element has two elements, a reference handle
	 * and the GPE number.
	 */
	elm = &obj->Package.Elements[0];

	switch (elm->Type) {

	case ACPI_TYPE_INTEGER:
		val = elm->Integer.Value;
		break;

	case ACPI_TYPE_PACKAGE:

		if (elm->Package.Count < 2)
			goto out;

		if (elm->Package.Elements[0].Type != ACPI_TYPE_LOCAL_REFERENCE)
			goto out;

		if (elm->Package.Elements[1].Type != ACPI_TYPE_INTEGER)
			goto out;

		val = elm->Package.Elements[1].Integer.Value;
		break;

	default:
		goto out;
	}

	/*
	 * Set or unset a GPE as both runtime and wake.
	 */
	if (enable != true)
		(void)AcpiDisableGpe(NULL, val, ACPI_NOT_ISR);
	else {
		(void)AcpiSetGpeType(NULL, val, ACPI_GPE_TYPE_WAKE_RUN);
		(void)AcpiEnableGpe(NULL, val, ACPI_NOT_ISR);
	}

out:
	ACPI_FREE(buf.Pointer);
}
