#	$NetBSD: bsd.own.mk,v 1.222 2001/11/19 04:44:32 perry Exp $

.if !defined(_BSD_OWN_MK_)
_BSD_OWN_MK_=1

MAKECONF?=	/etc/mk.conf
.-include "${MAKECONF}"

# NEED_OWN_INSTALL_TARGET is set to "no" by pkgsrc/mk/bsd.pkg.mk to
# ensure that things defined by <bsd.own.mk> (default targets,
# INSTALL_FILE, etc.) are not conflicting with bsd.pkg.mk.
NEED_OWN_INSTALL_TARGET?=	yes

# Temporary; this will become default when all platforms have migrated.
.if defined(USE_NEW_TOOLCHAIN) && ${USE_NEW_TOOLCHAIN} == "no"
.undef USE_NEW_TOOLCHAIN
.else
.if ${MACHINE_ARCH} == "arm" || \
    ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "powerpc" || \
    ${MACHINE_ARCH} == "sparc" || \
    ${MACHINE_ARCH} == "sparc64"
USE_NEW_TOOLCHAIN=nowarn
.endif
.endif

.if defined(USE_NEW_TOOLCHAIN)
CPPFLAG_ISYSTEM=	-isystem
.else
CPPFLAG_ISYSTEM=	-idirafter
.endif

.if empty(.MAKEFLAGS:M-V*)
PRINTOBJDIR=	${MAKE} -V .OBJDIR
.else
PRINTOBJDIR=	echo # prevent infinite recursion
.endif

.if !defined(_SRC_TOP_)
# Find the top of the source tree to see if we're inside of $BSDSRCDIR
_SRC_TOP_!= cd ${.CURDIR}; while :; do \
		here=`pwd`; \
		[ -f build.sh  ] && [ -d tools ] && { echo $$here; break; }; \
		case $$here in /) echo ""; break;; esac; \
		cd ..; done 

.MAKEOVERRIDES+=	_SRC_TOP_
.endif

.if !defined(_SRC_TOP_OBJ_)
_SRC_TOP_OBJ_!=	cd ${_SRC_TOP_} && ${PRINTOBJDIR}
.MAKEOVERRIDES+=	_SRC_TOP_OBJ_
.endif

.if (${_SRC_TOP_} != "") && defined(USE_NEW_TOOLCHAIN)
USETOOLS?=	yes
.endif
USETOOLS?=	no

.if ${MACHINE_ARCH} == "mips" || ${MACHINE_ARCH} == "sh3"
.BEGIN:
	@echo "Must set MACHINE_ARCH to one of ${MACHINE_ARCH}eb or ${MACHINE_ARCH}el"
	@false
.elif defined(REQUIRETOOLS) && defined(USE_NEW_TOOLCHAIN) && ${USETOOLS} == "no"
.BEGIN:
	@echo "USETOOLS=no, but this component requires a version-specific host toolchain"
	@false
.endif

# Host platform information; may be overridden
.if !defined(HOST_OSTYPE)
_HOST_OSNAME!=	uname -s
_HOST_OSREL!=	uname -r
_HOST_ARCH!=	uname -p 2>/dev/null || uname -m
HOST_OSTYPE:=	${_HOST_OSNAME}-${_HOST_OSREL}-${_HOST_ARCH}
.MAKEOVERRIDES+= HOST_OSTYPE
.endif

.if ${USETOOLS} == "yes"
# Provide a default for TOOLDIR.
.if !defined(TOOLDIR)
_TOOLOBJ!=	cd ${_SRC_TOP_:U${BSDSRCDIR}}/tools && ${PRINTOBJDIR}
TOOLDIR:=	${_TOOLOBJ}/tools.${HOST_OSTYPE}
.MAKEOVERRIDES+= TOOLDIR
.endif

# Define default locations for common tools.
AR=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-ar
AS=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-as
ASN1_COMPILE=	${TOOLDIR}/bin/nbasn1_compile
CC=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-gcc
COMPILE_ET=	${TOOLDIR}/bin/nbcompile_et
CONFIG=		${TOOLDIR}/bin/nbconfig
CPP=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-cpp
CRUNCHGEN=	MAKE=${.MAKE:Q} ${TOOLDIR}/bin/nbcrunchgen
CXX=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-c++
DBSYM=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-dbsym
EQN=		${TOOLDIR}/bin/nbeqn
FC=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-g77
GENCAT=		${TOOLDIR}/bin/nbgencat
#GRIND=		${TOOLDIR}/bin/nbvgrind -f
GROFF=		${TOOLDIR}/bin/nbgroff
INDXBIB=	${TOOLDIR}/bin/nbindxbib
INSTALL=	STRIP=${STRIP:Q} ${TOOLDIR}/bin/nbinstall
INSTALL_INFO=	${TOOLDIR}/bin/nbinstall-info
LD=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-ld
LEX=		${TOOLDIR}/bin/nblex
LINT=		CC=${CC:Q} ${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-lint
LORDER=		NM=${NM:Q} ${TOOLDIR}/bin/nblorder
MAKEINFO=	${TOOLDIR}/bin/nbmakeinfo
MDSETIMAGE=	${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-mdsetimage
MENUC=		MENUDEF=${TOOLDIR}/share/misc ${TOOLDIR}/bin/nbmenuc
MKDEP=		CC=${CC:Q} ${TOOLDIR}/bin/nbmkdep
MKLOCALE=	${TOOLDIR}/bin/nbmklocale
MSGC=		MSGDEF=${TOOLDIR}/share/misc ${TOOLDIR}/bin/nbmsgc
MTREE=		${TOOLDIR}/bin/nbmtree
NM=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-nm
OBJC=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-gcc
OBJCOPY=	${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-objcopy
OBJDUMP=	${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-objdump
PIC=		${TOOLDIR}/bin/nbpic
RANLIB=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-ranlib
REFER=		${TOOLDIR}/bin/nbrefer
RPCGEN=		${TOOLDIR}/bin/nbrpcgen
SIZE=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-size
SOELIM=		${TOOLDIR}/bin/nbsoelim
STRIP=		${TOOLDIR}/bin/${MACHINE_GNU_PLATFORM}-strip
TBL=		${TOOLDIR}/bin/nbtbl
TSORT=		${TOOLDIR}/bin/nbtsort -q
YACC=		${TOOLDIR}/bin/nbyacc

# Make sure DESTDIR is set, so that builds with these tools always
# get appropriate -nostdinc, -nostdlib, etc. handling.  The default is
# <empty string>, meaning start from /, the root directory.
DESTDIR?=
.endif

# Defining `SKEY' causes support for S/key authentication to be compiled in.
SKEY=		yes

# where the system object and source trees are kept; can be configurable
# by the user in case they want them in ~/foosrc and ~/fooobj, for example
BSDSRCDIR?=	/usr/src
BSDOBJDIR?=	/usr/obj

BINGRP?=	wheel
BINOWN?=	root
BINMODE?=	555
NONBINMODE?=	444

# Define MANZ to have the man pages compressed (gzip)
#MANZ=		1

MANDIR?=	/usr/share/man
MANGRP?=	wheel
MANOWN?=	root
MANMODE?=	${NONBINMODE}
MANINSTALL?=	maninstall catinstall

INFODIR?=	/usr/share/info
INFOGRP?=	wheel
INFOOWN?=	root
INFOMODE?=	${NONBINMODE}

LIBDIR?=	/usr/lib
LINTLIBDIR?=	/usr/libdata/lint
LIBGRP?=	${BINGRP}
LIBOWN?=	${BINOWN}
LIBMODE?=	${NONBINMODE}

DOCDIR?=	/usr/share/doc
HTMLDOCDIR?=	/usr/share/doc/html
DOCGRP?=	wheel
DOCOWN?=	root
DOCMODE?=	${NONBINMODE}

NLSDIR?=	/usr/share/nls
NLSGRP?=	wheel
NLSOWN?=	root
NLSMODE?=	${NONBINMODE}

KMODDIR?=	/usr/lkm
KMODGRP?=	wheel
KMODOWN?=	root
KMODMODE?=	${NONBINMODE}

LOCALEDIR?=	/usr/share/locale
LOCALEGRP?=	wheel
LOCALEOWN?=	root
LOCALEMODE?=	${NONBINMODE}

COPY?=		-c
PRESERVE?=	${UPDATE:D-p}
RENAME?=	-r
HRDLINK?=	-l h
SYMLINK?=	-l s

METALOG?=	${_SRC_TOP_OBJ_}/METALOG
INSTPRIV?=	${UNPRIVED:D-U -M ${METALOG}}
STRIPFLAG?=	-s

.if ${NEED_OWN_INSTALL_TARGET} == "yes"
INSTALL_DIR?=		${INSTALL} ${INSTPRIV} -d
INSTALL_FILE?=		${INSTALL} ${INSTPRIV} ${COPY} ${PRESERVE} ${RENAME}
INSTALL_LINK?=		${INSTALL} ${INSTPRIV} ${HRDLINK}
INSTALL_SYMLINK?=	${INSTALL} ${INSTPRIV} ${SYMLINK}
HOST_INSTALL_FILE?=	${INSTALL} ${COPY} ${PRESERVE} ${RENAME}
.endif

# Define SYS_INCLUDE to indicate whether you want symbolic links to the system
# source (``symlinks''), or a separate copy (``copies''); (latter useful
# in environments where it's not possible to keep /sys publicly readable)
#SYS_INCLUDE= 	symlinks

# The sh3 port is incomplete.
.if ${MACHINE_ARCH} == "sh3eb" || ${MACHINE_ARCH} == "sh3el"
NOLINT=1
NOPROFILE=1
OBJECT_FMT?=COFF
NOPIC?=1
.endif

# Profiling and linting is also off on the x86_64 port at the moment.
# The x86_64 port is incomplete.
.if ${MACHINE_ARCH} == "x86_64"
NOPROFILE=1
NOLINT=1
.endif

# The m68000 port is incomplete.
.if ${MACHINE_ARCH} == "m68000"
NOLINT=1
NOPROFILE=1
NOPIC?=1
.endif

# Data-driven table using make variables to control how 
# toolchain-dependent targets and shared libraries are built
# for different platforms and object formats.
# OBJECT_FMT:		currently either "ELF" or "a.out".
# SHLIB_TYPE:		"ELF" or "a.out" or "" to force static libraries.
#
.if ${MACHINE_ARCH} == "alpha" || \
    ${MACHINE_ARCH} == "arm" || \
    ${MACHINE_ARCH} == "mipsel" || ${MACHINE_ARCH} == "mipseb" || \
    ${MACHINE_ARCH} == "powerpc" || \
    ${MACHINE_ARCH} == "sparc" || \
    ${MACHINE_ARCH} == "sparc64" || \
    ${MACHINE_ARCH} == "x86_64" || \
    ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "m68000" || \
    ${MACHINE} == "next68k" || \
    ${MACHINE} == "sun3" || \
    ${MACHINE} == "mvme68k" || \
    ${MACHINE} == "hp300" || \
    ${MACHINE} == "news68k" || \
    ${MACHINE} == "cesfic" || \
    ${MACHINE} == "atari"
OBJECT_FMT?=ELF
.else
OBJECT_FMT?=a.out
.endif

.if ${MACHINE_ARCH} == "x86_64"
CFLAGS+=-Wno-format -fno-builtin
.endif

# Location of the file that contains the major and minor numbers of the
# version of a shared library.  If this file exists a shared library
# will be built by <bsd.lib.mk>.
SHLIB_VERSION_FILE?= ${.CURDIR}/shlib_version

# GNU sources and packages sometimes see architecture names differently.
GNU_ARCH.arm26=arm
GNU_ARCH.arm32=arm
GNU_ARCH.sh3eb=sh
GNU_ARCH.sh3el=sh
MACHINE_GNU_ARCH=${GNU_ARCH.${MACHINE_ARCH}:U${MACHINE_ARCH}}

# In order to identify NetBSD to GNU packages, we sometimes need
# an "elf" tag for historically a.out platforms.
.if ${OBJECT_FMT} == "ELF" && \
    (${MACHINE_GNU_ARCH} == "arm" || \
     ${MACHINE_ARCH} == "i386" || \
     ${MACHINE_ARCH} == "m68k" || \
     ${MACHINE_ARCH} == "sparc" || \
     ${MACHINE_ARCH} == "vax")
MACHINE_GNU_PLATFORM=${MACHINE_GNU_ARCH}--netbsdelf
.else
MACHINE_GNU_PLATFORM=${MACHINE_GNU_ARCH}--netbsd
.endif

# CPU model, derived from MACHINE_ARCH
MACHINE_CPU=	${MACHINE_ARCH:C/mipse[bl]/mips/:S/arm26/arm/:S/arm32/arm/:C/sh3e[bl]/sh3/:S/m68000/m68k/}

TARGETS+=	all clean cleandir depend dependall includes \
		install lint obj regress tags html installhtml cleanhtml
.PHONY:		all clean cleandir depend dependall distclean includes \
		install lint obj regress tags beforedepend afterdepend \
		beforeinstall afterinstall realinstall realdepend realall \
		html installhtml cheanhtml

.if ${NEED_OWN_INSTALL_TARGET} == "yes"
.if !target(install)
install:	.NOTMAIN beforeinstall subdir-install realinstall afterinstall
beforeinstall:	.NOTMAIN
subdir-install:	.NOTMAIN beforeinstall
realinstall:	.NOTMAIN beforeinstall
afterinstall:	.NOTMAIN subdir-install realinstall
.endif
all:		.NOTMAIN realall subdir-all
subdir-all:	.NOTMAIN
realall:	.NOTMAIN
depend:		.NOTMAIN realdepend subdir-depend
subdir-depend:	.NOTMAIN
realdepend:	.NOTMAIN
distclean:	.NOTMAIN cleandir
cleandir:	.NOTMAIN clean

dependall:	.NOTMAIN realdepend .MAKE
	@cd ${.CURDIR}; ${MAKE} realall
.endif

# Define MKxxx variables (which are either yes or no) for users
# to set in /etc/mk.conf and override on the make commandline.
# These should be tested with `== "no"' or `!= "no"'.
# The NOxxx variables should only be used by Makefiles.
#

MKCATPAGES?=yes

.if defined(NODOC)
MKDOC=no
.else
MKDOC?=yes
.endif

MKINFO?=yes

.if defined(NOLINKLIB)
MKLINKLIB=no
.else
MKLINKLIB?=yes
.endif
.if ${MKLINKLIB} == "no"
MKPICINSTALL=no
MKPROFILE=no
.endif

.if defined(NOLINT)
MKLINT=no
.else
MKLINT?=yes
.endif

.if defined(NOMAN)
MKMAN=no
.else
MKMAN?=yes
.endif
.if ${MKMAN} == "no"
MKCATPAGES=no
.endif

.if defined(NONLS)
MKNLS=no
.else
MKNLS?=yes
.endif

#
# MKOBJDIRS controls whether object dirs are created during "make build".
# MKOBJ controls whether the "make obj" rule does anything.
#
.if defined(NOOBJ)
MKOBJ=no
MKOBJDIRS=no
.else
MKOBJ?=yes
MKOBJDIRS?=no
.endif

.if defined(NOPIC)
MKPIC=no
.else
MKPIC?=yes
.endif

.if defined(NOPICINSTALL)
MKPICINSTALL=no
.else
MKPICINSTALL?=yes
.endif

.if defined(NOPROFILE)
MKPROFILE=no
.else
MKPROFILE?=yes
.endif

.if defined(NOSHARE)
MKSHARE=no
.else
MKSHARE?=yes
.endif
.if ${MKSHARE} == "no"
MKCATPAGES=no
MKDOC=no
MKINFO=no
MKMAN=no
MKNLS=no
.endif

.if defined(NOCRYPTO)
MKCRYPTO=no
.else
MKCRYPTO?=yes
.endif

MKCRYPTO_IDEA?=no

MKCRYPTO_RC5?=no

.if defined(NOKERBEROS) || (${MKCRYPTO} == "no")
MKKERBEROS=no
.else
MKKERBEROS?=yes
.endif

MKSOFTFLOAT?=no

.endif		# _BSD_OWN_MK_
