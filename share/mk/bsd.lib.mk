#	$NetBSD: bsd.lib.mk,v 1.237 2003/10/19 03:00:55 lukem Exp $
#	@(#)bsd.lib.mk	8.3 (Berkeley) 4/22/94

.include <bsd.init.mk>
.include <bsd.shlib.mk>
.include <bsd.gcc.mk>
# Pull in <bsd.sys.mk> here so we can override its .c.o rule
.include <bsd.sys.mk>

##### Basic targets
.PHONY:		checkver cleanlib libinstall
realinstall:	checkver libinstall
clean:		cleanlib

##### LIB specific flags.
COPTS+=    ${COPTS.lib${LIB}}
CPPFLAGS+=  ${CPPFLAGS.lib${LIB}}
CXXFLAGS+=  ${CXXFLAGS.lib${LIB}}
LDADD+=     ${LDADD.lib${LIB}}
LDFLAGS+=   ${LDFLAGS.lib${LIB}}
LDSTATIC+=  ${LDSTATIC.lib${LIB}}

##### Build and install rules
CPPFLAGS+=	${DESTDIR:D-nostdinc ${CPPFLAG_ISYSTEM} ${DESTDIR}/usr/include}
CXXFLAGS+=	${DESTDIR:D-nostdinc++ ${CPPFLAG_ISYSTEMXX} ${DESTDIR}/usr/include/g++}

.if !defined(SHLIB_MAJOR) && exists(${SHLIB_VERSION_FILE})
SHLIB_MAJOR != . ${SHLIB_VERSION_FILE} ; echo $$major
SHLIB_MINOR != . ${SHLIB_VERSION_FILE} ; echo $$minor
SHLIB_TEENY != . ${SHLIB_VERSION_FILE} ; echo $$teeny

# Check for higher installed library versions.
.if !defined(NOCHECKVER) && !defined(NOCHECKVER_${LIB}) && \
	exists(${NETBSDSRCDIR}/lib/checkver)
checkver:
	@(cd ${.CURDIR} && \
		sh ${NETBSDSRCDIR}/lib/checkver -v ${SHLIB_VERSION_FILE} \
		    -d ${DESTDIR}${_LIBSODIR} ${LIB})
.endif
.endif

.if !target(checkver)
checkver:
.endif

print-shlib-major:
.if defined(SHLIB_MAJOR) && ${MKPIC} != "no"
	@echo ${SHLIB_MAJOR}
.else
	@false
.endif

print-shlib-minor:
.if defined(SHLIB_MINOR) && ${MKPIC} != "no"
	@echo ${SHLIB_MINOR}
.else
	@false
.endif

print-shlib-teeny:
.if defined(SHLIB_TEENY) && ${MKPIC} != "no"
	@echo ${SHLIB_TEENY}
.else
	@false
.endif

.if defined(SHLIB_MAJOR) && !empty(SHLIB_MAJOR)
.if defined(SHLIB_MINOR) && !empty(SHLIB_MINOR)
.if defined(SHLIB_TEENY) && !empty(SHLIB_TEENY)
SHLIB_FULLVERSION=${SHLIB_MAJOR}.${SHLIB_MINOR}.${SHLIB_TEENY}
.else
SHLIB_FULLVERSION=${SHLIB_MAJOR}.${SHLIB_MINOR}
.endif
.else
SHLIB_FULLVERSION=${SHLIB_MAJOR}
.endif
.endif

# add additional suffixes not exported.
# .po is used for profiling object files.
# .so is used for PIC object files.
.SUFFIXES: .out .a .ln .so .po .o .s .S .c .cc .cpp .cxx .C .m .F .f .r .y .l .cl .p .h
.SUFFIXES: .sh .m4 .m


# Set PICFLAGS to cc flags for producing position-independent code,
# if not already set.  Includes -DPIC, if required.

# Data-driven table using make variables to control how shared libraries
# are built for different platforms and object formats.
# OBJECT_FMT:		currently either "ELF" or "a.out", from <bsd.own.mk>
# SHLIB_SOVERSION:	version number to be compiled into a shared library
#			via -soname. Usualy ${SHLIB_MAJOR} on ELF.
#			NetBSD/pmax used to use ${SHLIB_MAJOR}[.${SHLIB_MINOR}
#			[.${SHLIB_TEENY}]]
# SHLIB_SHFLAGS:	Flags to tell ${LD} to emit shared library.
#			with ELF, also set shared-lib version for ld.so.
# SHLIB_LDSTARTFILE:	support .o file, call C++ file-level constructors
# SHLIB_LDENDFILE:	support .o file, call C++ file-level destructors
# FPICFLAGS:		flags for ${FC} to compile .[fF] files to .so objects.
# CPPICFLAGS:		flags for ${CPP} to preprocess .[sS] files for ${AS}
# CPICFLAGS:		flags for ${CC} to compile .[cC] files to pic objects.
# CSHLIBFLAGS:		flags for ${CC} to compile .[cC] files to .so objects.
#			(usually includes ${CPICFLAGS})
# CAPICFLAGS:		flags for ${CC} to compiling .[Ss] files
#		 	(usually just ${CPPPICFLAGS} ${CPICFLAGS})
# APICFLAGS:		flags for ${AS} to assemble .[sS] to .so objects.

.if ${MACHINE_ARCH} == "alpha"

FPICFLAGS ?= -fPIC
CPICFLAGS ?= -fPIC -DPIC
CPPPICFLAGS?= -DPIC
CAPICFLAGS?= ${CPPPICFLAGS} ${CPICFLAGS}
APICFLAGS ?=

.elif (${MACHINE_ARCH} == "sparc" || ${MACHINE_ARCH} == "sparc64") && \
       ${OBJECT_FMT} == "ELF"

# If you use -fPIC you need to define BIGPIC to turn on 32-bit
# relocations in asm code
FPICFLAGS ?= -fPIC
CPICFLAGS ?= -fPIC -DPIC
CPPPICFLAGS?= -DPIC -DBIGPIC
CAPICFLAGS?= ${CPPPICFLAGS} ${CPICFLAGS}
APICFLAGS ?= -KPIC

.elif ${MACHINE_ARCH} == "hppa"

FPICFLAGS ?= -fPIC
CPICFLAGS?= -fPIC -DPIC
CPPPICFLAGS?= -DPIC
CAPICFLAGS?= ${CPPPICFLAGS} ${CPICFLAGS}
APICFLAGS?= -k
# XXX libraries often need the millicode functions in libgcc.a,
# so we have to work around the -nostdlib:
LDADD+= `${CC} -print-libgcc-file-name`

.else

# Platform-independent flags for NetBSD a.out shared libraries
SHLIB_SOVERSION=${SHLIB_FULLVERSION}
SHLIB_SHFLAGS=
FPICFLAGS ?= -fPIC
CPICFLAGS?= -fPIC -DPIC
CPPPICFLAGS?= -DPIC
CAPICFLAGS?= ${CPPPICFLAGS} ${CPICFLAGS}
APICFLAGS?= -k

.endif

.if ${MKPICLIB} != "no"
CSHLIBFLAGS+= ${CPICFLAGS}
.endif

.if defined(CSHLIBFLAGS) && !empty(CSHLIBFLAGS)
MKSHLIBOBJS= yes
.else
MKSHLIBOBJS= no
.endif

# Platform-independent linker flags for ELF shared libraries
.if ${OBJECT_FMT} == "ELF"
SHLIB_SOVERSION=	${SHLIB_MAJOR}
SHLIB_SHFLAGS=		-Wl,-soname,lib${LIB}.so.${SHLIB_SOVERSION}
SHLIB_LDSTARTFILE?=	${DESTDIR}/usr/lib/crti.o ${_GCC_CRTBEGINS}
SHLIB_LDENDFILE?=	${_GCC_CRTENDS} ${DESTDIR}/usr/lib/crtn.o
.endif

CFLAGS+=	${COPTS}
FFLAGS+=	${FOPTS}

.c.o:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.c} ${COPTS.${.IMPSRC:T}} ${CPUFLAGS.${.IMPSRC:T}} ${CPPFLAGS.${.IMPSRC:T}} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(COPTS) && !empty(COPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.c.po:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.c} ${COPTS.${.IMPSRC:T}} ${CPUFLAGS.${.IMPSRC:T}} ${CPPFLAGS.${.IMPSRC:T}} -pg ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(COPTS) && !empty(COPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -X -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.c.so:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.c} ${COPTS.${.IMPSRC:T}} ${CPUFLAGS.${.IMPSRC:T}} ${CPPFLAGS.${.IMPSRC:T}} ${CSHLIBFLAGS} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(COPTS) && !empty(COPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.c.ln:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${LINT} ${LINTFLAGS} ${CPPFLAGS:M-[IDU]*} ${CPPFLAGS.${.IMPSRC:T}:M-[-IDU]*} -i ${.IMPSRC}

.cc.o .cpp.o .cxx.o .C.o:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.cc} ${COPTS.${.IMPSRC:T}} ${CPUFLAGS.${.IMPSRC:T}} ${CPPFLAGS.${.IMPSRC:T}} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(COPTS) && !empty(COPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.cc.po .cpp.po .cxx.o .C.po:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.cc} ${COPTS.${.IMPSRC:T}} ${CPUFLAGS.${.IMPSRC:T}} ${CPPFLAGS.${.IMPSRC:T}} -pg ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(COPTS) && !empty(COPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -X -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.cc.so .cpp.so .cxx.so .C.so:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.cc} ${COPTS.${.IMPSRC:T}} ${CPUFLAGS.${.IMPSRC:T}} ${CPPFLAGS.${.IMPSRC:T}} ${CSHLIBFLAGS} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(COPTS) && !empty(COPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.f.o:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.f} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(FOPTS) && !empty(FOPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.f.po:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.f} -pg ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(FOPTS) && !empty(FOPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -X -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.f.so:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.f} ${FPICFLAGS} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(FOPTS) && !empty(FOPTS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.f.ln:
	${_MKMSGCOMPILE}
	@echo Skipping lint for Fortran libraries.

.m.o:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.m} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(OBJCFLAGS) && !empty(OBJCFLAGS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.m.po:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.m} -pg ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(OBJCFLAGS) && !empty(OBJCFLAGS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -X -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.m.so:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.m} ${CSHLIBFLAGS} ${.IMPSRC} -o ${.TARGET}.tmp
.if defined(OBJCFLAGS) && !empty(OBJCFLAGS:M*-g*)
	${_MKCMD}\
	mv ${.TARGET}.tmp ${.TARGET}
.else
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp
.endif

.S.o .s.o:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.S} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} -o ${.TARGET}.tmp
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp

.S.po .s.po:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.S} -DGPROF -DPROF ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} -o ${.TARGET}.tmp
	${_MKCMD}\
	${LD} -X -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp

.S.so .s.so:
	${_MKMSGCOMPILE}
	${_MKCMD}\
	${COMPILE.S} ${CAPICFLAGS} ${CFLAGS:M-[ID]*} ${AINC} ${.IMPSRC} -o ${.TARGET}.tmp
	${_MKCMD}\
	${LD} -x -r ${.TARGET}.tmp -o ${.TARGET}
	${_MKCMD}\
	rm -f ${.TARGET}.tmp

.if defined(LIB)
.if (${MKPIC} == "no" || (defined(LDSTATIC) && ${LDSTATIC} != "") \
	|| ${MKLINKLIB} != "no") && ${MKSTATICLIB} != "no"
_LIBS=lib${LIB}.a
.else
_LIBS=
.endif

OBJS+=${SRCS:N*.h:N*.sh:R:S/$/.o/g}

.if ${MKPROFILE} != "no"
_LIBS+=lib${LIB}_p.a
POBJS+=${OBJS:.o=.po}
.endif

.if ${MKPIC} != "no"
.if ${MKPICLIB} == "no"
.if ${MKSHLIBOBJS} != "no"
# make _pic.a, which isn't really pic,
# since it's needed for making shared lib.
# but don't install it.
SOLIB=lib${LIB}_pic.a
SOBJS+=${OBJS:.o=.so}
.else
SOLIB=lib${LIB}.a
.endif
.else
SOLIB=lib${LIB}_pic.a
_LIBS+=${SOLIB}
SOBJS+=${OBJS:.o=.so}
.endif
.if defined(SHLIB_FULLVERSION)
_LIBS+=lib${LIB}.so.${SHLIB_FULLVERSION}
.endif
.endif

LOBJS+=${LSRCS:.c=.ln} ${SRCS:M*.c:.c=.ln}
.if ${MKLINT} != "no" && ${MKLINKLIB} != "no" && !empty(LOBJS)
_LIBS+=llib-l${LIB}.ln
.endif

ALLOBJS=
.if (${MKPIC} == "no" || (defined(LDSTATIC) && ${LDSTATIC} != "") \
	|| ${MKLINKLIB} != "no") && ${MKSTATICLIB} != "no"
ALLOBJS+=${OBJS}
.endif
ALLOBJS+=${POBJS} ${SOBJS}
.if ${MKLINT} != "no" && ${MKLINKLIB} != "no" && !empty(LOBJS)
ALLOBJS+=${LOBJS}
.endif
.else
LOBJS=
SOBJS=
.endif

.NOPATH: ${ALLOBJS} ${_LIBS} ${SRCS:M*.[ly]:C/\..$/.c/} ${YHEADER:D${SRCS:M*.y:.y=.h}}

realall: ${SRCS} ${ALLOBJS:O} ${_LIBS}

__archivebuild: .USE
	${_MKMSGBUILD}
	${_MKCMD}\
	rm -f ${.TARGET}
	${_MKCMD}\
	${AR} cq ${.TARGET} `NM=${NM} ${LORDER} ${.ALLSRC:M*o} | ${TSORT}`
	${_MKCMD}\
	${RANLIB} ${.TARGET}

__archiveinstall: .USE
	${_MKMSGINSTALL}
	${_MKCMD}\
	${INSTALL_FILE} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
	    ${UPDATE:D:U-a "${RANLIB} -t"} ${SYSPKGTAG} ${.ALLSRC} ${.TARGET}

__archivesymlinkpic: .USE
	${_MKMSGINSTALL}
	${_MKCMD}\
	${INSTALL_SYMLINK} ${SYSPKGTAG} ${.ALLSRC} ${.TARGET}

DPSRCS+=	${SRCS:M*.l:.l=.c} ${SRCS:M*.y:.y=.c}
CLEANFILES+=	${SRCS:M*.l:.l=.c} ${SRCS:M*.y:.y=.c}
CLEANFILES+=	${YHEADER:D${SRCS:M*.y:.y=.h}}

${OBJS} ${POBJS} ${SOBJS} ${LOBJS}: ${DPSRCS}

lib${LIB}.a:: ${OBJS} __archivebuild

lib${LIB}_p.a:: ${POBJS} __archivebuild

lib${LIB}_pic.a:: ${SOBJS} __archivebuild


_LIBLDOPTS=
.if ${SHLIBDIR} != "/usr/lib"
_LIBLDOPTS+=	-Wl,-rpath-link,${DESTDIR}${SHLIBDIR}:${DESTDIR}/usr/lib \
		-R${SHLIBDIR} \
		-L${DESTDIR}${SHLIBDIR}
.elif ${SHLIBINSTALLDIR} != "/usr/lib"
_LIBLDOPTS+=	-Wl,-rpath-link,${DESTDIR}${SHLIBINSTALLDIR}:${DESTDIR}/usr/lib \
		-L${DESTDIR}${SHLIBINSTALLDIR}
.endif

lib${LIB}.so.${SHLIB_FULLVERSION}: ${SOLIB} ${DPADD} \
    ${SHLIB_LDSTARTFILE} ${SHLIB_LDENDFILE}
	${_MKMSGBUILD}
	${_MKCMD}\
	rm -f lib${LIB}.so.${SHLIB_FULLVERSION}
.if defined(DESTDIR)
	${_MKCMD}\
	${CC} -Wl,-nostdlib -B${_GCC_CRTDIR}/ -B${DESTDIR}/usr/lib/ \
	    ${_LIBLDOPTS} \
	    -Wl,-x -shared ${SHLIB_SHFLAGS} ${LDFLAGS} -o ${.TARGET} \
	    -Wl,--whole-archive ${SOLIB} \
	    -Wl,--no-whole-archive ${LDADD} \
	    -L${_GCC_LIBGCCDIR}
.else
	${_MKCMD}\
	${CC} -Wl,-x -shared ${SHLIB_SHFLAGS} ${LDFLAGS} -o ${.TARGET} \
	    ${_LIBLDOPTS} \
	    -Wl,--whole-archive ${SOLIB} -Wl,--no-whole-archive ${LDADD}
.endif
.if ${OBJECT_FMT} == "ELF"
#  We don't use INSTALL_SYMLINK here because this is just
#  happening inside the build directory/objdir. XXX Why does
#  this spend so much effort on libraries that aren't live??? XXX
	${_MKCMD}\
	ln -sf lib${LIB}.so.${SHLIB_FULLVERSION} lib${LIB}.so.${SHLIB_MAJOR}.tmp
	${_MKCMD}\
	mv -f lib${LIB}.so.${SHLIB_MAJOR}.tmp lib${LIB}.so.${SHLIB_MAJOR}
	${_MKCMD}\
	ln -sf lib${LIB}.so.${SHLIB_FULLVERSION} lib${LIB}.so.tmp
	${_MKCMD}\
	mv -f lib${LIB}.so.tmp lib${LIB}.so
.endif

.if !empty(LOBJS)
LLIBS?=		-lc
llib-l${LIB}.ln: ${LOBJS}
	${_MKMSGCOMPILE}
	${_MKCMD}\
	rm -f llib-l${LIB}.ln
.if defined(DESTDIR)
	${_MKCMD}\
	${LINT} -C${LIB} ${.ALLSRC} -L${DESTDIR}/usr/libdata ${LLIBS}
.else
	${_MKCMD}\
	${LINT} -C${LIB} ${.ALLSRC} ${LLIBS}
.endif
.endif

cleanlib:
	${_MKCMD}\
	rm -f a.out [Ee]rrs mklog core *.core ${CLEANFILES}
	${_MKCMD}\
	rm -f lib${LIB}.a ${OBJS}
	${_MKCMD}\
	rm -f lib${LIB}_p.a ${POBJS}
	${_MKCMD}\
	rm -f lib${LIB}_pic.a lib${LIB}.so.* lib${LIB}.so ${SOBJS}
	${_MKCMD}\
	rm -f ${OBJS:=.tmp} ${POBJS:=.tmp} ${SOBJS:=.tmp}
	${_MKCMD}\
	rm -f llib-l${LIB}.ln ${LOBJS}

.if defined(SRCS)
afterdepend: .depend
	@(TMP=/tmp/_depend$$$$; trap 'rm -f $$TMP ; exit 1' 1 2 3 13 15; \
	    sed -e 's/^\([^\.]*\).o[ ]*:/\1.o \1.po \1.so \1.ln:/' \
	      < .depend > $$TMP && \
	    mv $$TMP .depend)
.endif

.if !target(libinstall)
# Make sure it gets defined, in case MKPIC==no && MKLINKLIB==no
libinstall::

.if ${MKLINKLIB} != "no" && ${MKSTATICLIB} != "no"
libinstall:: ${DESTDIR}${LIBDIR}/lib${LIB}.a
.PRECIOUS: ${DESTDIR}${LIBDIR}/lib${LIB}.a

.if !defined(UPDATE)
.if !defined(BUILD) && !make(all) && !make(lib${LIB}.a)
${DESTDIR}${LIBDIR}/lib${LIB}.a! .MADE
.endif
${DESTDIR}${LIBDIR}/lib${LIB}.a! lib${LIB}.a __archiveinstall
.else
.if !defined(BUILD) && !make(all) && !make(lib${LIB}.a)
${DESTDIR}${LIBDIR}/lib${LIB}.a: .MADE
.endif
${DESTDIR}${LIBDIR}/lib${LIB}.a: lib${LIB}.a __archiveinstall
.endif
.endif

.if ${MKPROFILE} != "no"
libinstall:: ${DESTDIR}${LIBDIR}/lib${LIB}_p.a
.PRECIOUS: ${DESTDIR}${LIBDIR}/lib${LIB}_p.a

.if !defined(UPDATE)
.if !defined(BUILD) && !make(all) && !make(lib${LIB}_p.a)
${DESTDIR}${LIBDIR}/lib${LIB}_p.a! .MADE
.endif
${DESTDIR}${LIBDIR}/lib${LIB}_p.a! lib${LIB}_p.a __archiveinstall
.else
.if !defined(BUILD) && !make(all) && !make(lib${LIB}_p.a)
${DESTDIR}${LIBDIR}/lib${LIB}_p.a: .MADE
.endif
${DESTDIR}${LIBDIR}/lib${LIB}_p.a: lib${LIB}_p.a __archiveinstall
.endif
.endif

.if ${MKPIC} != "no" && ${MKPICINSTALL} != "no"
libinstall:: ${DESTDIR}${LIBDIR}/lib${LIB}_pic.a
.PRECIOUS: ${DESTDIR}${LIBDIR}/lib${LIB}_pic.a

.if !defined(UPDATE)
.if !defined(BUILD) && !make(all) && !make(lib${LIB}_pic.a)
${DESTDIR}${LIBDIR}/lib${LIB}_pic.a! .MADE
.endif
.if ${MKPICLIB} == "no"
${DESTDIR}${LIBDIR}/lib${LIB}_pic.a! lib${LIB}.a __archivesymlinkpic
.else
${DESTDIR}${LIBDIR}/lib${LIB}_pic.a! lib${LIB}_pic.a __archiveinstall
.endif
.else
.if !defined(BUILD) && !make(all) && !make(lib${LIB}_pic.a)
${DESTDIR}${LIBDIR}/lib${LIB}_pic.a: .MADE
.endif
.if ${MKPICLIB} == "no"
${DESTDIR}${LIBDIR}/lib${LIB}_pic.a: lib${LIB}.a __archivesymlinkpic
.else
${DESTDIR}${LIBDIR}/lib${LIB}_pic.a: lib${LIB}_pic.a __archiveinstall
.endif
.endif
.endif

.if ${MKPIC} != "no" && defined(SHLIB_FULLVERSION)
libinstall:: ${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION}
.PRECIOUS: ${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION}

.if !defined(UPDATE)
.if !defined(BUILD) && !make(all) && !make(lib${LIB}.so.${SHLIB_FULLVERSION})
${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION}! .MADE
.endif
${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION}! lib${LIB}.so.${SHLIB_FULLVERSION}
.else
.if !defined(BUILD) && !make(all) && !make(lib${LIB}.so.${SHLIB_FULLVERSION})
${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION}: .MADE
.endif
${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION}: lib${LIB}.so.${SHLIB_FULLVERSION}
.endif
	${_MKMSGINSTALL}
	${_MKCMD}\
	${INSTALL_FILE} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
		${SYSPKGTAG} ${.ALLSRC} ${.TARGET}
.if ${_LIBSODIR} != ${LIBDIR}
	${_MKCMD}\
	${INSTALL_SYMLINK} ${SYSPKGTAG} \
		${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION} \
		${DESTDIR}${LIBDIR}/lib${LIB}.so.${SHLIB_FULLVERSION}
.endif
.if ${OBJECT_FMT} == "a.out" && !defined(DESTDIR)
	${_MKCMD}\
	/sbin/ldconfig -m ${_LIBSODIR} ${LIBDIR}
.endif
.if ${OBJECT_FMT} == "ELF"
	${_MKCMD}\
	${INSTALL_SYMLINK} ${SYSPKGTAG} \
		lib${LIB}.so.${SHLIB_FULLVERSION} \
		${DESTDIR}${_LIBSODIR}/lib${LIB}.so.${SHLIB_MAJOR}
.if ${_LIBSODIR} != ${LIBDIR}
	${_MKCMD}\
	${INSTALL_SYMLINK} ${SYSPKGTAG} \
		${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION} \
		${DESTDIR}${LIBDIR}/lib${LIB}.so.${SHLIB_MAJOR}
.endif
.if ${MKLINKLIB} != "no"
	${_MKCMD}\
	${INSTALL_SYMLINK} ${SYSPKGTAG} \
		lib${LIB}.so.${SHLIB_FULLVERSION} \
		${DESTDIR}${_LIBSODIR}/lib${LIB}.so
.if ${_LIBSODIR} != ${LIBDIR}
	${_MKCMD}\
	${INSTALL_SYMLINK} ${SYSPKGTAG} \
		${_LIBSODIR}/lib${LIB}.so.${SHLIB_FULLVERSION} \
		${DESTDIR}${LIBDIR}/lib${LIB}.so
.endif
.endif
.endif
.endif

.if ${MKLINT} != "no" && ${MKLINKLIB} != "no" && !empty(LOBJS)
libinstall:: ${DESTDIR}${LINTLIBDIR}/llib-l${LIB}.ln
.PRECIOUS: ${DESTDIR}${LINTLIBDIR}/llib-l${LIB}.ln

.if !defined(UPDATE)
.if !defined(BUILD) && !make(all) && !make(llib-l${LIB}.ln)
${DESTDIR}${LINTLIBDIR}/llib-l${LIB}.ln! .MADE
.endif
${DESTDIR}${LINTLIBDIR}/llib-l${LIB}.ln! llib-l${LIB}.ln
.else
.if !defined(BUILD) && !make(all) && !make(llib-l${LIB}.ln)
${DESTDIR}${LINTLIBDIR}/llib-l${LIB}.ln: .MADE
.endif
${DESTDIR}${LINTLIBDIR}/llib-l${LIB}.ln: llib-l${LIB}.ln
.endif
	${_MKMSGINSTALL}
	${_MKCMD}\
	${INSTALL_FILE} -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} \
		${SYSPKGTAG} ${.ALLSRC} ${DESTDIR}${LINTLIBDIR}
.endif
.endif

##### Pull in related .mk logic
.include <bsd.man.mk>
.include <bsd.nls.mk>
.include <bsd.files.mk>
.include <bsd.inc.mk>
.include <bsd.links.mk>
.include <bsd.dep.mk>

${TARGETS}:	# ensure existence
