# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-binutils,v 1.2 2003/11/27 10:52:53 mrg Exp 
#
G_libbfd_la_DEPENDENCIES=elf64-sparc.lo elf64.lo elf.lo elflink.lo elf-strtab.lo elf-eh-frame.lo dwarf1.lo elf32-sparc.lo elf32.lo sparcnetbsd.lo aout32.lo sunos.lo elf64-gen.lo elf32-gen.lo cpu-sparc.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo bfdio.lo bfdwin.lo  cache.lo coffgen.lo corefile.lo format.lo init.lo libbfd.lo opncls.lo  reloc.lo section.lo syms.lo targets.lo hash.lo linker.lo srec.lo  binary.lo tekhex.lo ihex.lo stabs.lo stab-syms.lo merge.lo dwarf2.lo  simple.lo archive64.lo
G_DEFS=-DHAVE_CONFIG_H -I. -I${GNUHOSTDIST}/bfd -I.
G_INCLUDES=-D_GNU_SOURCE     -I. -I${GNUHOSTDIST}/bfd -I${GNUHOSTDIST}/bfd/../include  -I${GNUHOSTDIST}/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf64_sparc_vec -DSELECT_VECS='&bfd_elf64_sparc_vec,&bfd_elf32_sparc_vec,&sparcnetbsd_vec,&sunos_big_vec,&bfd_elf64_little_generic_vec,&bfd_elf64_big_generic_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_sparc_arch' -DHAVE_bfd_elf64_sparc_vec -DHAVE_bfd_elf32_sparc_vec -DHAVE_sparcnetbsd_vec -DHAVE_sunos_big_vec -DHAVE_bfd_elf64_little_generic_vec -DHAVE_bfd_elf64_big_generic_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec
