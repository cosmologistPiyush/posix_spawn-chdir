#include <sys/types.h>
#include <sys/param.h>
#include <sys/device.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <scsi/scsi_all.h>
#include <scsi/scsiconf.h>

#include <dev/isa/isavar.h>
#include <dev/isa/isadmavar.h>

#include <dev/ic/bhareg.h>
#include <dev/ic/bhavar.h>

#define	BHA_ISA_IOSIZE	4

int	bha_isa_probe __P((struct device *, void *, void *));
void	bha_isa_attach __P((struct device *, struct device *, void *));

struct cfattach bha_isa_ca = {
	sizeof(struct bha_softc), bha_isa_probe, bha_isa_attach
};

/*
 * Check the slots looking for a board we recognise
 * If we find one, note it's address (slot) and call
 * the actual probe routine to check it out.
 */
int
bha_isa_probe(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct isa_attach_args *ia = aux;
	struct bha_softc sc;
	bus_chipset_tag_t bc = ia->ia_bc;
	bus_io_handle_t ioh;
	isa_chipset_tag_t ic = ia->ia_ic;
	int rv;

	if (bus_io_map(bc, ia->ia_iobase, BHA_ISA_IOSIZE, &ioh))
		return (0);

	rv = bha_find(bc, ioh, &sc);

	bus_io_unmap(bc, ioh, BHA_ISA_IOSIZE);

	if (rv) {
		if (ia->ia_irq != -1 && ia->ia_irq != sc.sc_irq)
			return (0);
		if (ia->ia_drq != -1 && ia->ia_drq != sc.sc_drq)
			return (0);
		ia->ia_irq = sc.sc_irq;
		ia->ia_drq = sc.sc_drq;
		ia->ia_msize = 0;
		ia->ia_iosize = BHA_ISA_IOSIZE;
	}
	return (rv);
}

/*
 * Attach all the sub-devices we can find
 */
void
bha_isa_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct isa_attach_args *ia = aux;
	struct bha_softc *sc = (void *)self;
	bus_chipset_tag_t bc = ia->ia_bc;
	bus_io_handle_t ioh;
	isa_chipset_tag_t ic = ia->ia_ic;

	printf("\n");

	if (bus_io_map(bc, ia->ia_iobase, BHA_ISA_IOSIZE, &ioh))
		panic("bha_attach: bus_io_map failed!");

	sc->sc_bc = bc;
	sc->sc_ioh = ioh;
	if (!bha_find(bc, ioh, sc))
		panic("bha_attach: bha_find failed!");

	if (sc->sc_drq != -1)
		isa_dmacascade(sc->sc_drq);

	sc->sc_ih = isa_intr_establish(ic, sc->sc_irq, IST_EDGE, IPL_BIO,
	    bha_intr, sc);
	if (sc->sc_ih == NULL) {
		printf("%s: couldn't establish interrupt\n",
		    sc->sc_dev.dv_xname);
		return;
	}

	bha_attach(sc);
}
