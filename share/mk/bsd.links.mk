#	$NetBSD: bsd.links.mk,v 1.28 2004/05/16 09:44:38 lukem Exp $

.include <bsd.init.mk>

##### Basic targets
install:	linksinstall

##### Default values
LINKS?=
SYMLINKS?=

##### Install rules
.PHONY:		linksinstall
linksinstall::	realinstall
.if !empty(SYMLINKS)
	@(set ${SYMLINKS}; \
	 while test $$# -ge 2; do \
		l=$$1; shift; \
		t=${DESTDIR}$$1; shift; \
		if  ttarg=`${TOOL_STAT} -qf '%Y' $$t` && \
		    [ "$$l" = "$$ttarg" ]; then \
			continue ; \
		fi ; \
		${_MKSHMSG_INSTALL} $$t; \
		${_MKSHECHO} ${INSTALL_SYMLINK} ${SYSPKGTAG} $$l $$t; \
		${INSTALL_SYMLINK} ${SYSPKGTAG} $$l $$t; \
	 done; )
.endif
.if !empty(LINKS)
	@(set ${LINKS}; \
	 while test $$# -ge 2; do \
		l=${DESTDIR}$$1; shift; \
		t=${DESTDIR}$$1; shift; \
		if  ldevino=`${TOOL_STAT} -qf '%d %i' $$l` && \
		    tdevino=`${TOOL_STAT} -qf '%d %i' $$t` && \
		    [ "$$ldevino" = "$$tdevino" ]; then \
			continue ; \
		fi ; \
		${_MKSHMSG_INSTALL} $$t; \
		${_MKSHECHO} ${INSTALL_LINK} ${SYSPKGTAG} $$l $$t; \
		${INSTALL_LINK} ${SYSPKGTAG} $$l $$t; \
	done ; )
.endif


configinstall:		configlinksinstall
configlinksinstall::	.PHONY
.if defined(CONFIGSYMLINKS)
	@(set ${CONFIGSYMLINKS}; \
	 while test $$# -ge 2; do \
		l=$$1; shift; \
		t=${DESTDIR}$$1; shift; \
		if  ttarg=`${TOOL_STAT} -qf '%Y' $$t` && \
		    [ "$$l" = "$$ttarg" ]; then \
			continue ; \
		fi ; \
		${_MKSHMSG_INSTALL} $$t; \
		${_MKSHECHO} ${INSTALL_SYMLINK} ${SYSPKGTAG} $$l $$t; \
		${INSTALL_SYMLINK} ${SYSPKGTAG} $$l $$t; \
	 done; )
.endif
.if !empty(CONFIGLINKS)
	@(set ${CONFIGLINKS}; \
	 while test $$# -ge 2; do \
		l=${DESTDIR}$$1; shift; \
		t=${DESTDIR}$$1; shift; \
		if  ldevino=`${TOOL_STAT} -qf '%d %i' $$l` && \
		    tdevino=`${TOOL_STAT} -qf '%d %i' $$t` && \
		    [ "$$ldevino" = "$$tdevino" ]; then \
			continue ; \
		fi ; \
		${_MKSHMSG_INSTALL} $$t; \
		${_MKSHECHO} ${INSTALL_LINK} ${SYSPKGTAG} $$l $$t; \
		${INSTALL_LINK} ${SYSPKGTAG} $$l $$t; \
	done ; )
.endif

.include <bsd.sys.mk>
