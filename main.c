/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-10 18:31:30
 * @Description: projet de crypto
 * programme original en python réécriture en C
 */

#ifndef HEADER
#define HEADER
#include "header.h"
#endif

#ifndef FUNCTIONS
#define FUNCTIONS
#include "functions.h"
#endif

#ifndef SWITCHES
#define SWITCHES
#include "switches.h"
#endif

int main(int argc, char** argv) {
	int progress;
	int number_of_threads_arg;
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
	 * Does the user want a progress indicator
	 * defines the number of threads used
	 * Dynamic thread number is weird, disabled for now
	 */
	progress = 0;
	number_of_threads_arg = 1;
	while (--argc > 0) {
		if (!strcmp(argv[argc], "-progress")) {
			progress = 1;
		}
		if (!strcmp(argv[argc], "-thread")) {
			number_of_threads_arg = atoi(argv[argc+1]);
		}
	}
	file_opener_and_writer(argv, progress, number_of_threads_arg);

	return 0;
}

