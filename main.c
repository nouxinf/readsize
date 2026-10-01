#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long int findSize(char file_name[]) {
	FILE* fp = fopen(file_name, "r");
	if (fp == NULL) {
		printf("Error: File not found!\n");
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	long int res = ftell(fp);
	fclose(fp);
	return res;
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
		printf("%s\n", humanReadableBytes(findSize(argv[1])));
	}
	
	return 0;
}