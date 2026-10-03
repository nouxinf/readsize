#ifdef __unix__
#include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)
#define OS_windows 1
#include <fileapi.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#define getcwd _getcwd
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h> 
#include "tinydir.h"

tinydir_file file;

long long findSize(char file_name[]) {
	#if defined(OS_windows)
		struct _stat buf;
		int result;
		result = _stat(file_name, &buf);
		if (result != 0) {
			perror( "Problem getting information" );
	 		switch (errno) {
				case ENOENT:
					printf("File %s not found.\n", file_name);
					break;
		 		case EINVAL:
					printf("Invalid parameter to _stat.\n");
		   		break;
			default:
		   		/* Should never be reached. */
		   		printf("Unexpected error in _stat.\n");
	  		}
		} else {
			return buf.st_size;
		}
	#else
		FILE* fp = fopen(file_name, "r");
		if (fp == NULL) {
			printf("Error: File not found!\n");
			return -1;
		}
		fseek(fp, 0L, SEEK_END);
		long int res = ftell(fp);
		fclose(fp);
		return res;
	#endif
	
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
	if (argc == 1) {
		printf("Error: No arguments were provided");
	} else {
		if (tinydir_file_open(&file, argv[1]) == 0) {
			if (file.is_dir) {
				printf("Folder exists!\n");
			} else {
				printf("Path exists but is not a folder\n");
			}
		} else {
			printf("Path does not exist\n");
		}
		printf("%s\n", humanReadableBytes(findSize(argv[1])));
	}
	
	return 0;
}