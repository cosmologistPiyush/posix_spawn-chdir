# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-binutils,v 1.4 2004/12/10 13:57:08 mrg Exp 
#
G_DEFS=-DHAVE_CONFIG_H -I. -I${GNUHOSTDIST}/ld -I.
G_EMUL=elf32btsmipn32
G_EMULATION_OFILES=eelf32btsmipn32.o eelf64ltsmip.o eelf64btsmip.o eelf32btsmip.o eelf32ltsmipn32.o eelf32ltsmip.o
G_INCLUDES=-D_GNU_SOURCE -I. -I${GNUHOSTDIST}/ld -I../bfd -I${GNUHOSTDIST}/ld/../bfd -I${GNUHOSTDIST}/ld/../include -I${GNUHOSTDIST}/ld/../intl -I../intl   -DLOCALEDIR="\"/usr/local/share/locale\""
G_OFILES=ldgram.o ldlex.o lexsup.o ldlang.o mri.o ldctor.o ldmain.o  ldwrite.o ldexp.o  ldemul.o ldver.o ldmisc.o  ldfile.o ldcref.o eelf32btsmipn32.o eelf64ltsmip.o eelf64btsmip.o eelf32btsmip.o eelf32ltsmipn32.o eelf32ltsmip.o
G_STRINGIFY=astring.sed
G_TEXINFOS=ld.texinfo
G_target_alias=mips64--netbsd
