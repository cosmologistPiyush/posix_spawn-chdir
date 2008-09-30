/*	$NetBSD: plist.c,v 1.1.1.1 2008/09/30 19:00:27 joerg Exp $	*/

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <nbcompat.h>
#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif
#ifndef lint
#if 0
static const char *rcsid = "from FreeBSD Id: plist.c,v 1.24 1997/10/08 07:48:15 charnier Exp";
#else
__RCSID("$NetBSD: plist.c,v 1.1.1.1 2008/09/30 19:00:27 joerg Exp $");
#endif
#endif

/*
 * FreeBSD install - a package for the installation and maintainance
 * of non-core utilities.
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
 * Jordan K. Hubbard
 * 18 July 1993
 *
 * General packing list routines.
 *
 */

/*-
 * Copyright (c) 2008 Joerg Sonnenberger <joerg@NetBSD.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "lib.h"
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#if HAVE_ERR_H
#include <err.h>
#endif
#ifndef NETBSD
#include <nbcompat/md5.h>
#else
#include <md5.h>
#endif

/* This struct defines a plist command type */
typedef struct cmd_t {
	const char   *c_s;		/* string to recognise */
	pl_ent_t c_type;	/* type of command */
	int     c_argc;		/* # of arguments */
	int     c_subst;	/* can substitute real prefix */
}       cmd_t;

/* Commands to recognise */
static const cmd_t cmdv[] = {
	{"cwd", PLIST_CWD, 1, 1},
	{"src", PLIST_SRC, 1, 1},
	{"cd", PLIST_CWD, 1, 1},
	{"exec", PLIST_CMD, 1, 0},
	{"unexec", PLIST_UNEXEC, 1, 0},
	{"mode", PLIST_CHMOD, 1, 0},
	{"owner", PLIST_CHOWN, 1, 0},
	{"group", PLIST_CHGRP, 1, 0},
	{"comment", PLIST_COMMENT, 1, 0},
	{"ignore", PLIST_IGNORE, 0, 0},
	{"ignore_inst", PLIST_IGNORE_INST, 0, 0},
	{"name", PLIST_NAME, 1, 0},
	{"display", PLIST_DISPLAY, 1, 0},
	{"pkgdep", PLIST_PKGDEP, 1, 0},
	{"pkgcfl", PLIST_PKGCFL, 1, 0},
	{"mtree", PLIST_MTREE, 1, 0},
	{"dirrm", PLIST_DIR_RM, 1, 0},
	{"option", PLIST_OPTION, 1, 0},
	{"blddep", PLIST_BLDDEP, 1, 0},
	{NULL, FAIL, 0, 0}
};

/*
 * Add an item to the end of a packing list
 */
void
add_plist(package_t *p, pl_ent_t type, const char *arg)
{
	plist_t *tmp;

	tmp = new_plist_entry();
	tmp->name = (arg == (char *) NULL) ? (char *) NULL : strdup(arg);
	tmp->type = type;
	if (!p->head) {
		p->head = p->tail = tmp;
	} else {
		tmp->prev = p->tail;
		p->tail->next = tmp;
		p->tail = tmp;
	}
}

/*
 * Add an item to the start of a packing list
 */
void
add_plist_top(package_t *p, pl_ent_t type, const char *arg)
{
	plist_t *tmp;

	tmp = new_plist_entry();
	tmp->name = (arg == (char *) NULL) ? (char *) NULL : strdup(arg);
	tmp->type = type;
	if (!p->head) {
		p->head = p->tail = tmp;
	} else {
		tmp->next = p->head;
		p->head->prev = tmp;
		p->head = tmp;
	}
}

/*
 * Return the last (most recent) entry in a packing list
 */
plist_t *
last_plist(package_t *p)
{
	return p->tail;
}

/*
 * Mark all items in a packing list to prevent iteration over them
 */
void
mark_plist(package_t *pkg)
{
	plist_t *pp;

	for (pp = pkg->head; pp; pp = pp->next) {
		pp->marked = TRUE;
	}
}

/*
 * Find a given item in a packing list and, if so, return it (else NULL)
 */
plist_t *
find_plist(package_t *pkg, pl_ent_t type)
{
	plist_t *pp;

	for (pp = pkg->head; pp && pp->type != type; pp = pp->next) {
	}
	return pp;
}

/*
 * Look for a specific boolean option argument in the list
 */
char   *
find_plist_option(package_t *pkg, char *name)
{
	plist_t *p;

	for (p = pkg->head; p; p = p->next) {
		if (p->type == PLIST_OPTION
		    && strcmp(p->name, name) == 0) {
			return p->name;
		}
	}
	
	return (char *) NULL;
}

/*
 * Delete plist item 'type' in the list (if 'name' is non-null, match it
 * too.)  If 'all' is set, delete all items, not just the first occurance.
 */
void
delete_plist(package_t *pkg, Boolean all, pl_ent_t type, char *name)
{
	plist_t *p = pkg->head;

	while (p) {
		plist_t *pnext = p->next;

		if (p->type == type && (!name || !strcmp(name, p->name))) {
			free(p->name);
			if (p->prev)
				p->prev->next = pnext;
			else
				pkg->head = pnext;
			if (pnext)
				pnext->prev = p->prev;
			else
				pkg->tail = p->prev;
			free(p);
			if (!all)
				return;
			p = pnext;
		} else
			p = p->next;
	}
}

/*
 * Allocate a new packing list entry, and return a pointer to it. 
 */
plist_t *
new_plist_entry(void)
{
	plist_t *ret;

	if ((ret = (plist_t *) malloc(sizeof(plist_t))) == (plist_t *) NULL) {
		err(EXIT_FAILURE, "can't allocate %ld bytes", (long) sizeof(plist_t));
	}
	memset(ret, 0, sizeof(plist_t));
	return ret;
}

/*
 * Free an entire packing list
 */
void
free_plist(package_t *pkg)
{
	plist_t *p = pkg->head;

	while (p) {
		plist_t *p1 = p->next;

		free(p->name);
		free(p);
		p = p1;
	}
	pkg->head = pkg->tail = NULL;
}

/*
 * For an ASCII string denoting a plist command, return its code and
 * optionally its argument(s)
 */
static int
plist_cmd(const char *s, char **arg)
{
	const cmd_t *cmdp;
	const char *cp, *sp;
	char *sp2;

	for (cmdp = cmdv; cmdp->c_s; ++cmdp) {
		for (sp = s, cp = cmdp->c_s; *sp && *cp; ++cp, ++sp)
			if (tolower((unsigned char)*sp) != *cp)
				break;
		if (*cp == '\0')
			break;
	}

	if (cmdp->c_s == NULL || arg == NULL)
		return cmdp->c_type;

	while (isspace((unsigned char)*sp))
		++sp;
	*arg = strdup(sp);
	if (*arg == NULL)
		err(2, "strdup failed");
	if (*sp) {
		sp2 = *arg + strlen(*arg) - 1;
		/*
		 * The earlier loop ensured that at least one non-whitespace
		 * is in the string.
		 */
		while (isspace((unsigned char)*sp2))
			--sp2;
		sp2[1] = '\0';
	}
	return cmdp->c_type;
}

/*
 * Parse a packaging list from a memory buffer.
 */
void
parse_plist(package_t *pkg, const char *buf)
{
	int cmd;
	char *line, *cp;
	const char *eol, *next;
	size_t len;

	for (; *buf; buf = next) {
		/* Until add_plist can deal with trailing whitespace. */
		if ((eol = strchr(buf, '\n')) != NULL) {
			next = eol + 1;
			len = eol - buf;
		} else {
			len = strlen(buf);
			next = buf + len;
		}

		while (len && isspace((unsigned char)buf[len - 1]))
			--len;

		if (len == 0)
			continue;

		line = malloc(len + 1);
		if (line == NULL)
			err(2, "malloc failed");
		memcpy(line, buf, len);
		line[len] = '\0';

		if (*(cp = line) == CMD_CHAR) {
			if ((cmd = plist_cmd(line + 1, &cp)) == FAIL) {
				warnx("Unrecognised PLIST command `%s'", line);
				continue;
			}
			if (*cp == '\0') {
				free(cp);
				cp = NULL;
			}
		} else {
			cmd = PLIST_FILE;
		}
		add_plist(pkg, cmd, cp);
		free(cp);
	}
}

/*
 * Read a packing list from a file
 */
void
append_plist(package_t *pkg, FILE * fp)
{
	char    pline[MaxPathSize];
	char   *cp;
	int     cmd;
	int     len;
	int	free_cp;

	while (fgets(pline, MaxPathSize, fp) != (char *) NULL) {
		for (len = strlen(pline); len &&
		    isspace((unsigned char) pline[len - 1]);) {
			pline[--len] = '\0';
		}
		if (len == 0) {
			continue;
		}
		free_cp = 0;
		if (*(cp = pline) == CMD_CHAR) {
			if ((cmd = plist_cmd(pline + 1, &cp)) == FAIL) {
				warnx("Unrecognised PLIST command `%s'", pline);
				continue;
			}
			if (*cp == '\0') {
				free(cp);
				cp = NULL;
			}
			free_cp = 1;
		} else {
			cmd = PLIST_FILE;
		}
		add_plist(pkg, cmd, cp);
		if (free_cp)
			free(cp);
	}
}

void
read_plist(package_t *pkg, FILE * fp)
{
	pkg->head = NULL;
	pkg->tail = NULL;

	append_plist(pkg, fp);
}

/*
 * Write a packing list to a file, converting commands to ASCII equivs
 */
void
write_plist(package_t *pkg, FILE * fp, char *realprefix)
{
	plist_t *p;
	const cmd_t *cmdp;

	for (p = pkg->head; p; p = p->next) {
		if (p->type == PLIST_FILE) {
			/* Fast-track files - these are the most common */
			(void) fprintf(fp, "%s\n", p->name);
			continue;
		}
		for (cmdp = cmdv; cmdp->c_type != FAIL && cmdp->c_type != p->type; cmdp++) {
		}
		if (cmdp->c_type == FAIL) {
			warnx("Unknown PLIST command type %d (%s)", p->type, p->name);
		} else if (cmdp->c_argc == 0) {
			(void) fprintf(fp, "%c%s\n", CMD_CHAR, cmdp->c_s);
		} else if (cmdp->c_subst && realprefix) {
			(void) fprintf(fp, "%c%s %s\n", CMD_CHAR, cmdp->c_s, realprefix);
		} else {
			(void) fprintf(fp, "%c%s %s\n", CMD_CHAR, cmdp->c_s,
			    (p->name) ? p->name : "");
		}
	}
}

/*
 * Like write_plist, but compute memory string.
 */
void
stringify_plist(package_t *pkg, char **real_buf, size_t *real_len, char *realprefix)
{
	plist_t *p;
	const cmd_t *cmdp;
	char *buf;
	size_t len;
	int item_len;

	/* Pass One: compute output size only. */
	len = 0;

	for (p = pkg->head; p; p = p->next) {
		if (p->type == PLIST_FILE) {
			len += strlen(p->name) + 1;
			continue;
		}
		for (cmdp = cmdv; cmdp->c_type != FAIL && cmdp->c_type != p->type; cmdp++) {
		}
		if (cmdp->c_type == FAIL)
			continue;
		if (cmdp->c_argc == 0)
			len += 1 + strlen(cmdp->c_s) + 1;
		else if (cmdp->c_subst && realprefix)
			len += 1 + strlen(cmdp->c_s) + 1 + strlen(realprefix) + 1;
		else
			len += 1 + strlen(cmdp->c_s) + 1 + strlen(p->name ? p->name : "") + 1;
	}

	/* Pass Two: build actual string. */
	if ((buf = malloc(len + 1)) == NULL)
		err(2, "malloc failed");
	*real_buf = buf;
	*real_len = len;
	++len;

#define	UPDATE_LEN							\
do {									\
	if (item_len < 0 || item_len > len)				\
		errx(2, "Size computation failed, aborted.");		\
	buf += item_len;						\
	len -= item_len;						\
} while (0)

	for (p = pkg->head; p; p = p->next) {
		if (p->type == PLIST_FILE) {
			/* Fast-track files - these are the most common */
			item_len = snprintf(buf, len, "%s\n", p->name);
			UPDATE_LEN;
			continue;
		}
		for (cmdp = cmdv; cmdp->c_type != FAIL && cmdp->c_type != p->type; cmdp++) {
		}
		if (cmdp->c_type == FAIL) {
			warnx("Unknown PLIST command type %d (%s)", p->type, p->name);
		} else if (cmdp->c_argc == 0) {
			item_len = snprintf(buf, len, "%c%s\n", CMD_CHAR, cmdp->c_s);
			UPDATE_LEN;
		} else if (cmdp->c_subst && realprefix) {
			item_len = snprintf(buf, len, "%c%s %s\n", CMD_CHAR, cmdp->c_s, realprefix);
			UPDATE_LEN;
		} else {
			item_len = snprintf(buf, len, "%c%s %s\n", CMD_CHAR, cmdp->c_s,
			    (p->name) ? p->name : "");
			UPDATE_LEN;
		}
	}

	if (len != 1)
		errx(2, "Size computation failed, aborted.");
}

/*
 * Delete the results of a package installation.
 *
 * This is here rather than in the pkg_delete code because pkg_add needs to
 * run it too in cases of failure.
 */
int
delete_package(Boolean ign_err, Boolean nukedirs, package_t *pkg, Boolean NoDeleteFiles)
{
	plist_t *p;
	char   *Where = ".", *last_file = "";
	int     fail = SUCCESS;
	Boolean preserve;
	char    tmp[MaxPathSize], *name = NULL;

	if (!pkgdb_open(ReadWrite)) {
		err(EXIT_FAILURE, "cannot open pkgdb");
	}

	preserve = find_plist_option(pkg, "preserve") ? TRUE : FALSE;
	for (p = pkg->head; p; p = p->next) {
		switch (p->type) {
		case PLIST_NAME:
			name = p->name;
			break;

		case PLIST_IGNORE:
			p = p->next;
			break;

		case PLIST_CWD:
			Where = p->name;
			if (Verbose)
				printf("Change working directory to %s\n", Where);
			break;

		case PLIST_UNEXEC:
			if (NoDeleteFiles)
				break;
			format_cmd(tmp, sizeof(tmp), p->name, Where, last_file);
			printf("Executing `%s'\n", tmp);
			if (!Fake && system(tmp)) {
				warnx("unexec command for `%s' failed", tmp);
				fail = FAIL;
			}
			break;

		case PLIST_FILE:
			last_file = p->name;
			(void) snprintf(tmp, sizeof(tmp), "%s/%s", Where, p->name);
			if (isdir(tmp)) {
				warnx("attempting to delete directory `%s' as a file\n"
				    "this packing list is incorrect - ignoring delete request", tmp);
			} else {
				int     restored = 0;	/* restored from preserve? */

				if (p->next && p->next->type == PLIST_COMMENT) {
					if (strncmp(p->next->name, CHECKSUM_HEADER, ChecksumHeaderLen) == 0) {
						char   *cp, buf[LegibleChecksumLen];

						if ((cp = MD5File(tmp, buf)) != NULL) {
							/* Mismatch? */
							if (strcmp(cp, p->next->name + ChecksumHeaderLen) != 0) {
								printf("original MD5 checksum failed, %s: %s\n",
								    Force ? "deleting anyway" : "not deleting", tmp);
								if (!Force) {
									fail = FAIL;
									continue;
								}
							}
						}
					} else if (strncmp(p->next->name, SYMLINK_HEADER, SymlinkHeaderLen) == 0) {
						char	buf[MaxPathSize + SymlinkHeaderLen];
						int	cc;

						(void) strlcpy(buf, SYMLINK_HEADER,
						    sizeof(buf));
						if ((cc = readlink(tmp, &buf[SymlinkHeaderLen],
							  sizeof(buf) - SymlinkHeaderLen - 1)) < 0) {
							warn("can't readlink `%s'", tmp);
							continue;
						}
						buf[SymlinkHeaderLen + cc] = 0x0;
						if (strcmp(buf, p->next->name) != 0) {
							if ((cc = readlink(&buf[SymlinkHeaderLen], &buf[SymlinkHeaderLen],
								  sizeof(buf) - SymlinkHeaderLen)) < 0) {
								printf("symlink %s is not same as recorded value, %s: %s\n",
								    buf, Force ? "deleting anyway" : "not deleting", tmp);
								if (!Force) {
									fail = FAIL;
									continue;
								}
							}
							buf[SymlinkHeaderLen + cc] = 0x0;
							if (strcmp(buf, p->next->name) != 0) {
								printf("symlink %s is not same as recorded value, %s: %s\n",
								    buf, Force ? "deleting anyway" : "not deleting", tmp);
								if (!Force) {
									fail = FAIL;
									continue;
								}
							}
						}
					}
				}
				if (Verbose && !NoDeleteFiles)
					printf("Delete file %s\n", tmp);
				if (!Fake && !NoDeleteFiles) {
					if (delete_hierarchy(tmp, ign_err, nukedirs))
						fail = FAIL;
					if (preserve && name) {
						char    tmp2[MaxPathSize];

						if (make_preserve_name(tmp2, MaxPathSize, name, tmp)) {
							if (fexists(tmp2)) {
								if (rename(tmp2, tmp))
									warn("preserve: unable to restore %s as %s",
									    tmp2, tmp);
								else
									restored = 1;
							}
						}
					}
				}

				if (!Fake) {
					if (!restored) {
#ifdef PKGDB_DEBUG
						printf("pkgdb_remove(\"%s\")\n", tmp);	/* HF */
#endif
						errno = 0;
						if (pkgdb_remove(tmp)) {
							if (errno) {
								perror("pkgdb_remove");
							}
						} else {
#ifdef PKGDB_DEBUG
							printf("pkgdb_remove: ok\n");
#endif
						}
					}
				}
			}
			break;

		case PLIST_DIR_RM:
			if (NoDeleteFiles)
				break;

			(void) snprintf(tmp, sizeof(tmp), "%s/%s", Where, p->name);
			if (fexists(tmp)) {
			    if (!isdir(tmp)) {
				warnx("cannot remove `%s' as a directory\n"
				    "this packing list is incorrect - ignoring delete request", tmp);
			    } else {
				if (Verbose)
					printf("Delete directory %s\n", tmp);
				if (!Fake && delete_hierarchy(tmp, ign_err, FALSE)) {
					warnx("unable to completely remove directory '%s'", tmp);
					fail = FAIL;
				}
			    }
			} else {
			    warnx("cannot remove non-existent directory `%s'\n"
			            "this packing list is incorrect - ignoring delete request", tmp);
			}
			last_file = p->name;
			break;
		default:
			break;
		}
	}
	pkgdb_close();
	return fail;
}

#ifdef DEBUG
#define RMDIR(dir) fexec(RMDIR_CMD, dir, NULL)
#define REMOVE(dir,ie) fexec_skipemtpy(REMOVE_CMD, (ie) ? "-f " : "", dir, NULL)
#else
#define RMDIR rmdir
#define	REMOVE(file,ie) (remove(file) && !(ie))
#endif

/*
 * Selectively delete a hierarchy
 * Returns 1 on error, 0 else.
 */
int
delete_hierarchy(char *dir, Boolean ign_err, Boolean nukedirs)
{
	char   *cp1, *cp2;

	cp1 = cp2 = dir;
	if (!fexists(dir)) {
		if (!ign_err)
			warnx("%s `%s' doesn't really exist",
			    isdir(dir) ? "directory" : "file", dir);
		return !ign_err;
	} else if (nukedirs) {
		if (fexec_skipempty(REMOVE_CMD, "-r", ign_err ? "-f" : "", dir, NULL))
			return 1;
	} else if (isdir(dir)) {
		if (RMDIR(dir) && !ign_err)
			return 1;
	} else {
		if (REMOVE(dir, ign_err))
			return 1;
	}

	if (!nukedirs)
		return 0;
	while (cp2) {
		if ((cp2 = strrchr(cp1, '/')) != NULL)
			*cp2 = '\0';
		if (!isemptydir(dir))
			return 0;
		if (RMDIR(dir) && !ign_err) {
			if (!fexists(dir))
				warnx("directory `%s' doesn't really exist", dir);
			else
				return 1;
		}
		/* back up the pathname one component */
		if (cp2) {
			cp1 = dir;
		}
	}
	return 0;
}
