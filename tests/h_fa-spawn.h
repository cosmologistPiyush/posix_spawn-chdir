#include <atf-c.h>

#include <sys/stat.h>

#include <stdio.h>
#include <string.h>

off_t filesize(const char * restrict fname);
void empty_outfile(const char *restrict filename);
