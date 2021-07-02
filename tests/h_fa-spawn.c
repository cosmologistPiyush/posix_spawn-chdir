#include "h_fa-spawn.h"

off_t
filesize(const char * restrict fname)
{
	struct stat st;
	int err;

	err = stat(fname, &st);
	ATF_REQUIRE(err == 0);
	return st.st_size;
}

void
empty_outfile(const char *restrict filename)
{
	FILE *f;

	f = fopen(filename, "w");
	ATF_REQUIRE(f != NULL);
	fclose(f);
}
