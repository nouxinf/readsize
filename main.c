#include <stdio.h>

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

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Error: No arguments were provided");
	} else {
		printf("%d", findSize(argv[1]));
	}
	
	return 0;
}