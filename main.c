#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include "tinydir.h"

#ifdef _WIN32
    #define stat_fn _stat64
    typedef struct _stat64 stat_t;
    #include <windows.h>
#else
    #define stat_fn stat
    typedef struct stat stat_t;
    #include <sys/statvfs.h>
#endif

tinydir_file file;

bool findSize(const char *path, long long *out) {
    stat_t st;
    if (stat_fn(path, &st) != 0) {
        return false;
    }
    *out = (long long)st.st_size;
    return true;
}

static bool dirSize(const char *path, long long *total) {
    tinydir_dir dir;
    if (tinydir_open(&dir, path) != 0) return false;

    while (dir.has_next) {
        tinydir_file f;
        if (tinydir_readfile(&dir, &f) != 0) break;

        if (strcmp(f.name, ".") != 0 && strcmp(f.name, "..") != 0) {
            if (f.is_dir) {
                dirSize(f.path, total);
            } else {
                long long sz;
                if (findSize(f.path, &sz)) *total += sz;
            }
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);
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
	const char *path = NULL;
	bool raw = false;
	bool disk_usage = false;
    for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--disk-usage") == 0) {
			disk_usage = true;
		} else if (strcmp(argv[i], "--raw") == 0) {
			raw = true;
		} else if (strncmp(argv[i], "--", 2) == 0) {
			fprintf(stderr, "readsize: unknown option '%s'\n", argv[i]);
		} else if (path == NULL) {
			path = argv[i];
		} else {
			fprintf(stderr, "readsize: only one path allowed\n");
			return 1;
		}
	}

	if (path == NULL && !disk_usage) {
		fprintf(stderr, "usage: readsize <path> [--raw] [--disk-usage]\n");
		return 1;
	}
	if (disk_usage && path == NULL) {
		#if _WIN32
		ULARGE_INTEGER freeToCaller, total, totalFree;
		if (!GetDiskFreeSpaceExW(NULL, &freeToCaller, &total, &totalFree)) {
			fprintf(stderr, "GetDiskFreeSpaceExW failed: %lu\n", GetLastError());
			return 1;
		}

		ULONGLONG used = total.QuadPart - totalFree.QuadPart;
		if (raw) {
			printf("%llu/%llu\n", (unsigned long long)used, (unsigned long long)total.QuadPart);
			return 0;
		} else {
			char usedStr[32], totalStr[32];
			snprintf(usedStr, sizeof usedStr, "%s", humanReadableBytes(used));
			snprintf(totalStr, sizeof totalStr, "%s", humanReadableBytes(total.QuadPart));
			printf("%s/%s\n", usedStr, totalStr);
			return 0;
		}
		#else
		struct statvfs fs;
		if (statvfs("/", &fs) != 0) {
			perror("statvfs");
			return 1;
		}
		unsigned long long total = (unsigned long long)fs.f_blocks * fs.f_frsize;
		unsigned long long free = (unsigned long long)fs.f_bavail * fs.f_frsize;
		unsigned long long used = total - free;
		if (raw) {
			printf("%llu/%llu\n", used, total);
			return 0;
		} else {
			char usedStr[32], totalStr[32];
			snprintf(usedStr, sizeof usedStr, "%s", humanReadableBytes(used));
			snprintf(totalStr, sizeof totalStr, "%s", humanReadableBytes(total));
			printf("%s/%s\n", usedStr, totalStr);
			return 0;
		}
		#endif
	} else {
		if (tinydir_file_open(&file, path) == 0) {
			if (file.is_dir) {
				// is a folder
				long long totalSize = 0;
				if (!dirSize(path, &totalSize)) {
					fprintf(stderr, "readsize: cannot read '%s'\n", path);
					return 1;
				}
				if (raw) {
					printf("%lld\n", totalSize);
				} else {
					printf("%s\n", humanReadableBytes(totalSize));
				}
			} else {
				long long size;
				if (!findSize(path, &size)) {
					fprintf(stderr, "readsize: cannot access '%s': %s\n",
					path, strerror(errno));
				return 1;
				}
				if (raw) {
					printf("%lld\n", size);
				} else {
					printf("%s\n", humanReadableBytes(size));
				}
			}
		} else {
			fprintf(stderr, "File %s wasn't found\n", path);
		}
	}

    return 0;
}
