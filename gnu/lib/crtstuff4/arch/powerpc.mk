# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-gcc,v 1.16 2006/05/12 02:12:37 mrg Exp 
# and from: NetBSD: mknative.common,v 1.6 2006/05/15 21:01:42 mrg Exp 
#
G_INCLUDES=-I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/. -I${GNUHOSTDIST}/gcc/../include -I${GNUHOSTDIST}/gcc/../libcpp/include
G_CRTSTUFF_CFLAGS=-O2 -DIN_GCC -W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -isystem ./include -I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/. -I${GNUHOSTDIST}/gcc/../include -I${GNUHOSTDIST}/gcc/../libcpp/include -g0 -finhibit-size-directive -fno-inline-functions -fno-exceptions -fno-zero-initialized-in-bss -fno-unit-at-a-time
G_CRTSTUFF_T_CFLAGS=-fPIC -msdata=none
G_tm_defines=NETBSD_ENABLE_PTHREADS
G_xm_file=
G_xm_defines=
