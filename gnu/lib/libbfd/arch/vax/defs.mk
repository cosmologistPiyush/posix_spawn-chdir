# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative,v 1.9 2003/02/07 01:52:54 lukem Exp 
#
G_libbfd_la_DEPENDENCIES=elf32-vax.lo elf32.lo elf.lo elflink.lo elf-strtab.lo elf-eh-frame.lo dwarf1.lo vaxnetbsd.lo aout32.lo vax1knetbsd.lo elf32-gen.lo cpu-vax.lo netbsd-core.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo cache.lo coffgen.lo  corefile.lo format.lo init.lo libbfd.lo opncls.lo reloc.lo section.lo  syms.lo targets.lo hash.lo linker.lo srec.lo binary.lo tekhex.lo  ihex.lo stabs.lo stab-syms.lo merge.lo dwarf2.lo archive64.lo
G_DEFS=-DHAVE_CONFIG_H -I. -I${GNUHOSTDIST}/bfd -I.
G_INCLUDES=-D_GNU_SOURCE  -DNETBSD_CORE   -I. -I${GNUHOSTDIST}/bfd -I${GNUHOSTDIST}/bfd/../include  -I${GNUHOSTDIST}/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf32_vax_vec -DSELECT_VECS='&bfd_elf32_vax_vec,&vaxnetbsd_vec,&vax1knetbsd_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_vax_arch' -DHAVE_bfd_elf32_vax_vec -DHAVE_vaxnetbsd_vec -DHAVE_vax1knetbsd_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec
