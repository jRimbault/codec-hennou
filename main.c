/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-08 22:16:55
 * @Description: projet de crypto
 * programme original en python réécriture en C
 */

#include "header.h"
#include "functions.h"

int main(int argc, char** argv) {
	int progress;
	/*
	 * Checking if sane arguments were given
	 * before triggering the program
	 */
	if (argc < 4) {
		help();
		return 1;
	}
	if (!strcmp(argv[1], "help")) {
		help();
		return 0;
	}
	/*
	 * Does the user want a progress indicator?
	 */
	progress = 0;
	while (--argc > 0) {
		if (!strcmp(argv[argc], "-progress")) {
			progress = 1;
			break;
		}
	}

	file_opener_and_writer(argv, progress);

	return 0;
}

