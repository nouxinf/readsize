#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/stat.h>
//#include "tinydir.h"

#ifdef _WIN32
    #define stat_fn _stat64
    typedef struct _stat64 stat_t;
#else
    #define stat_fn stat
    typedef struct stat stat_t;
#endif

//tinydir_file file;

bool findSize(const char *path, long long *out) {
    stat_t st;
    if (stat_fn(path, &st) != 0) {
        return false;
    }
    *out = (long long)st.st_size;
    return true;
}

const char *humanReadableBytes(long long bytes) {
	double num = (double)bytes;
	static char buffer[100];
	const char *units[] = {
		"",
		"Ki",
		"Mi",
		"Gi",
		"Ti",
		"Pi",
		"Ei"
	};
	int unit = 0;
	while ((num >= 1024 || num <= -1024) && unit < 6) {
		num /= 1024;
		unit++;
	}
	snprintf(buffer, sizeof buffer, "%.1f %sB", num, units[unit]);
	return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    long long size;
    if (!findSize(argv[1], &size)) {
        fprintf(stderr, "readsize: cannot access '%s': %s\n",
                argv[1], strerror(errno));
        return 1;
    }

    printf("%s\n", humanReadableBytes(size));
    return 0;
}