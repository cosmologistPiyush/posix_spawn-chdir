#	Id: HP-UX.10.x,v 8.20 2003/11/21 01:05:09 lijian Exp
#	$NetBSD: HP-UX.10.x,v 1.6 2005/03/15 02:14:16 atatat Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

define(`confCC', `cc -Aa')
define(`confMAPDEF', `-DNDBM -DNIS -DMAP_REGEX')
define(`confENVDEF', `-D_HPUX_SOURCE -DHPUX10 -DV4FS')
define(`confSM_OS_HEADER', `sm_os_hp')
define(`confLIBS', `-lndbm')
define(`confSHELL', `/usr/bin/sh')
define(`confINSTALL', `${BUILDBIN}/install.sh')
define(`confSBINGRP', `mail')

dnl Don't indent or put any tab/space in this file.
dnl Tab/space here causes make syntax error
ifelse(confBLDVARIANT, `DEBUG',
dnl Debug build
`
define(`confOPTIMIZE',`-g')
',
dnl Optimized build
confBLDVARIANT, `OPTIMIZED',
`
define(`confOPTIMIZE',`+O3')
',
dnl Purify build
confBLDVARIANT, `PURIFY',
`
define(`confOPTIMIZE',`-g')
',
dnl default
`
define(`confOPTIMIZE',`+O3')
')
