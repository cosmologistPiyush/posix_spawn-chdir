/*	$NetBSD: content-bozo.c,v 1.4 2009/04/18 07:28:24 mrg Exp $	*/

/*	$eterna: content-bozo.c,v 1.12 2009/04/17 22:52:20 mrg Exp $	*/

/*
 * Copyright (c) 1997-2009 Matthew R. Green
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer and
 *    dedication in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/* this code implements content-type handling for bozohttpd */

#include <sys/param.h>

#include <errno.h>
#include <string.h>

#include "bozohttpd.h"

/*
 * this map and the functions below map between filenames and the
 * content type and content encoding definitions.  this should become
 * a configuration file, perhaps like apache's mime.types (but that
 * has less info per-entry).
 */

static struct content_map content_map[] = {
	{ ".html",	"text/html",			"",		"", NULL },
	{ ".htm",	"text/html",			"",		"", NULL },
	{ ".gif",	"image/gif",			"",		"", NULL },
	{ ".jpeg",	"image/jpeg",			"",		"", NULL },
	{ ".jpg",	"image/jpeg",			"",		"", NULL },
	{ ".jpe",	"image/jpeg",			"",		"", NULL },
	{ ".png",	"image/png",			"",		"", NULL },
	{ ".mp3",	"audio/mpeg",			"",		"", NULL },
	{ ".css",	"text/css",			"",		"", NULL },
	{ ".txt",	"text/plain",			"",		"", NULL },
	{ ".swf",	"application/x-shockwave-flash","",		"", NULL },
	{ ".dcr",	"application/x-director",	"",		"", NULL },
	{ ".pac",	"application/x-ns-proxy-autoconfig", "",	"", NULL },
	{ ".pa",	"application/x-ns-proxy-autoconfig", "",	"", NULL },
	{ ".tar",	"multipart/x-tar",		"",		"", NULL },
	{ ".gtar",	"multipart/x-gtar",		"",		"", NULL },
	{ ".tar.Z",	"multipart/x-tar",		"x-compress",	"compress", NULL },
	{ ".tar.gz",	"multipart/x-tar",		"x-gzip",	"gzip", NULL },
	{ ".taz",	"multipart/x-tar",		"x-gzip",	"gzip", NULL },
	{ ".tgz",	"multipart/x-tar",		"x-gzip",	"gzip", NULL },
	{ ".tar.z",	"multipart/x-tar",		"x-pack",	"x-pack", NULL },
	{ ".Z",		"application/x-compress",	"x-compress",	"compress", NULL },
	{ ".gz",	"application/x-gzip",		"x-gzip",	"gzip", NULL },
	{ ".z",		"unknown",			"x-pack",	"x-pack", NULL },
	{ ".bz2",	"application/x-bzip2",		"x-bzip2",	"x-bzip2", NULL },
	{ ".ogg",	"application/x-ogg",		"",		"", NULL },
	{ ".xbel",	"text/xml",			"",		"", NULL },
	{ ".xml",	"text/xml",			"",		"", NULL },
	{ ".xsl",	"text/xml",			"",		"", NULL },
	{ ".hqx",	"application/mac-binhex40",	"",		"", NULL },
	{ ".cpt",	"application/mac-compactpro",	"",		"", NULL },
	{ ".doc",	"application/msword",		"",		"", NULL },
	{ ".bin",	"application/octet-stream",	"",		"", NULL },
	{ ".dms",	"application/octet-stream",	"",		"", NULL },
	{ ".lha",	"application/octet-stream",	"",		"", NULL },
	{ ".lzh",	"application/octet-stream",	"",		"", NULL },
	{ ".exe",	"application/octet-stream",	"",		"", NULL },
	{ ".class",	"application/octet-stream",	"",		"", NULL },
	{ ".oda",	"application/oda",		"",		"", NULL },
	{ ".pdf",	"application/pdf",		"",		"", NULL },
	{ ".ai",	"application/postscript",	"",		"", NULL },
	{ ".eps",	"application/postscript",	"",		"", NULL },
	{ ".ps",	"application/postscript",	"",		"", NULL },
	{ ".ppt",	"application/powerpoint",	"",		"", NULL },
	{ ".rtf",	"application/rtf",		"",		"", NULL },
	{ ".bcpio",	"application/x-bcpio",		"",		"", NULL },
	{ ".torrent",	"application/x-bittorrent",	"",		"", NULL },
	{ ".vcd",	"application/x-cdlink",		"",		"", NULL },
	{ ".cpio",	"application/x-cpio",		"",		"", NULL },
	{ ".csh",	"application/x-csh",		"",		"", NULL },
	{ ".dir",	"application/x-director",	"",		"", NULL },
	{ ".dxr",	"application/x-director",	"",		"", NULL },
	{ ".dvi",	"application/x-dvi",		"",		"", NULL },
	{ ".hdf",	"application/x-hdf",		"",		"", NULL },
	{ ".cgi",	"application/x-httpd-cgi",	"",		"", NULL },
	{ ".skp",	"application/x-koan",		"",		"", NULL },
	{ ".skd",	"application/x-koan",		"",		"", NULL },
	{ ".skt",	"application/x-koan",		"",		"", NULL },
	{ ".skm",	"application/x-koan",		"",		"", NULL },
	{ ".latex",	"application/x-latex",		"",		"", NULL },
	{ ".mif",	"application/x-mif",		"",		"", NULL },
	{ ".nc",	"application/x-netcdf",		"",		"", NULL },
	{ ".cdf",	"application/x-netcdf",		"",		"", NULL },
	{ ".patch",	"application/x-patch",		"",		"", NULL },
	{ ".sh",	"application/x-sh",		"",		"", NULL },
	{ ".shar",	"application/x-shar",		"",		"", NULL },
	{ ".sit",	"application/x-stuffit",	"",		"", NULL },
	{ ".sv4cpio",	"application/x-sv4cpio",	"",		"", NULL },
	{ ".sv4crc",	"application/x-sv4crc",		"",		"", NULL },
	{ ".tar",	"application/x-tar",		"",		"", NULL },
	{ ".tcl",	"application/x-tcl",		"",		"", NULL },
	{ ".tex",	"application/x-tex",		"",		"", NULL },
	{ ".texinfo",	"application/x-texinfo",	"",		"", NULL },
	{ ".texi",	"application/x-texinfo",	"",		"", NULL },
	{ ".t",		"application/x-troff",		"",		"", NULL },
	{ ".tr",	"application/x-troff",		"",		"", NULL },
	{ ".roff",	"application/x-troff",		"",		"", NULL },
	{ ".man",	"application/x-troff-man",	"",		"", NULL },
	{ ".me",	"application/x-troff-me",	"",		"", NULL },
	{ ".ms",	"application/x-troff-ms",	"",		"", NULL },
	{ ".ustar",	"application/x-ustar",		"",		"", NULL },
	{ ".src",	"application/x-wais-source",	"",		"", NULL },
	{ ".zip",	"application/zip",		"",		"", NULL },
	{ ".au",	"audio/basic",			"",		"", NULL },
	{ ".snd",	"audio/basic",			"",		"", NULL },
	{ ".mpga",	"audio/mpeg",			"",		"", NULL },
	{ ".mp2",	"audio/mpeg",			"",		"", NULL },
	{ ".aif",	"audio/x-aiff",			"",		"", NULL },
	{ ".aiff",	"audio/x-aiff",			"",		"", NULL },
	{ ".aifc",	"audio/x-aiff",			"",		"", NULL },
	{ ".ram",	"audio/x-pn-realaudio",		"",		"", NULL },
	{ ".rpm",	"audio/x-pn-realaudio-plugin",	"",		"", NULL },
	{ ".ra",	"audio/x-realaudio",		"",		"", NULL },
	{ ".wav",	"audio/x-wav",			"",		"", NULL },
	{ ".pdb",	"chemical/x-pdb",		"",		"", NULL },
	{ ".xyz",	"chemical/x-pdb",		"",		"", NULL },
	{ ".ief",	"image/ief",			"",		"", NULL },
	{ ".tiff",	"image/tiff",			"",		"", NULL },
	{ ".tif",	"image/tiff",			"",		"", NULL },
	{ ".ras",	"image/x-cmu-raster",		"",		"", NULL },
	{ ".pnm",	"image/x-portable-anymap",	"",		"", NULL },
	{ ".pbm",	"image/x-portable-bitmap",	"",		"", NULL },
	{ ".pgm",	"image/x-portable-graymap",	"",		"", NULL },
	{ ".ppm",	"image/x-portable-pixmap",	"",		"", NULL },
	{ ".rgb",	"image/x-rgb",			"",		"", NULL },
	{ ".xbm",	"image/x-xbitmap",		"",		"", NULL },
	{ ".xpm",	"image/x-xpixmap",		"",		"", NULL },
	{ ".xwd",	"image/x-xwindowdump",		"",		"", NULL },
	{ ".rtx",	"text/richtext",		"",		"", NULL },
	{ ".tsv",	"text/tab-separated-values",	"",		"", NULL },
	{ ".etx",	"text/x-setext",		"",		"", NULL },
	{ ".sgml",	"text/x-sgml",			"",		"", NULL },
	{ ".sgm",	"text/x-sgml",			"",		"", NULL },
	{ ".mpeg",	"video/mpeg",			"",		"", NULL },
	{ ".mpg",	"video/mpeg",			"",		"", NULL },
	{ ".mpe",	"video/mpeg",			"",		"", NULL },
	{ ".qt",	"video/quicktime",		"",		"", NULL },
	{ ".mov",	"video/quicktime",		"",		"", NULL },
	{ ".avi",	"video/x-msvideo",		"",		"", NULL },
	{ ".movie",	"video/x-sgi-movie",		"",		"", NULL },
	{ ".ice",	"x-conference/x-cooltalk",	"",		"", NULL },
	{ ".wrl",	"x-world/x-vrml",		"",		"", NULL },
	{ ".vrml",	"x-world/x-vrml",		"",		"", NULL },
	{ NULL,		NULL,				NULL,		NULL, NULL, },
};

static struct content_map *dynamic_content_map;

/* call with stage == 0 for full search, stage == 1 for dynamic only */
struct content_map *
match_content_map(const char *name, int stage)
{
	size_t	len = strlen(name), nlen;
	struct	content_map	*map;

	for (map = dynamic_content_map; map && map->name; map++) {
again:
		nlen = strlen(map->name);
		if (nlen > len || strcasecmp(map->name, name + (len - nlen)) != 0)
			continue;
		return (map);
	}
	if (stage++ == 0) {
		map = content_map;
		goto again;
	}
		
	return (NULL);
}

/*
 * given the file name, return a valid Content-Type: value.
 */
/* ARGSUSED */
const char *
content_type(http_req *request, const char *file)
{
	struct	content_map	*map;

	map = match_content_map(file, 0);
	if (map)
		return (map->type);
	return (text_plain);
}

/*
 * given the file name, return a valid Content-Encoding: value.
 */
const char *
content_encoding(http_req *request, const char *file)
{
	struct	content_map	*map;

	map = match_content_map(file, 0);
	if (map)
		return (request->hr_proto == http_11 ?
		    map->encoding11 : map->encoding);
	return (NULL);
}

#ifndef NO_DYNAMIC_CONTENT
static int dynamic_content_map_size;

struct content_map *
get_content_map(const char *name)
{
	struct	content_map	*map;

	if ((map = match_content_map(name, 1)))
		return (map);
	
	dynamic_content_map_size++;
	dynamic_content_map = bozorealloc(dynamic_content_map,
	    (dynamic_content_map_size + 1) * sizeof *map);
	if (dynamic_content_map == NULL)
		error(1, "out of memory allocating content map");
	map = &dynamic_content_map[dynamic_content_map_size];
	map->name = map->type = map->encoding = map->encoding11 =
	    map->cgihandler = NULL;
	map--;

	return (map);
}

/*
 * mime content maps look like:
 *	".name type encoding encoding11"
 * where any of type, encoding or encoding11 a dash "-" means "".
 * eg the .gtar, .tar.Z from above  could be written like:
 *	".gtar multipart/x-gtar - -"
 *	".tar.Z multipart/x-tar x-compress compress"
 * or
 *	".gtar multipart/x-gtar"
 *	".tar.Z multipart/x-tar x-compress compress"
 * NOTE: we destroy 'arg'
 */
void
add_content_map_mime(char *cmap0, char *cmap1, char *cmap2, char *cmap3)
{
	struct content_map *map;

	debug((DEBUG_FAT, "add_content_map: name %s type %s enc %s enc11 %s ",
		cmap0, cmap1, cmap2, cmap3));

	map = get_content_map(cmap0);
#define CHECKMAP(s)	(!s || ((s)[0] == '-' && (s)[1] == '\0') ? "" : (s))
	map->name = CHECKMAP(cmap0);
	map->type = CHECKMAP(cmap1);
	map->encoding = CHECKMAP(cmap2);
	map->encoding11 = CHECKMAP(cmap3);
#undef CHECKMAP
	map->cgihandler = NULL;
}
#endif /* NO_DYNAMIC_CONTENT */
