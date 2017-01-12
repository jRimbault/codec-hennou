/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-12 11:40:17
 * @Description: projet de secu, encodage
 * programme original en python réécriture en C pour vitesse
 *
 * `gcc -O2 -lpthread main.c -o codec` pour compiler
 * `-Wall` pour débug
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
	arguments argument;
	int i;
	/*
	 * Checking if sane arguments were given
	 * before triggering the program
	 */
	if (argc < 4) {
		help();
		return 1;
	}
	/*
	 * Parses the arguments
	 */
	argument.progress       = 0;
	argument.thread_num_arg = 1;
	argument.operation      = 0;
	for(i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-progress")) {
			argument.progress = 1;
		}
		if (!strcmp(argv[i], "-thread")) {
			argument.thread_num_arg = atoi(argv[i+1]);
		}
		if (!strcmp(argv[i], "encrypt")) {
			if (argument.operation) {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n");
				return 0;
			}
			argument.operation   = 1;
			argument.input_file  = argv[i+1];
			argument.output_file = argv[i+2];
		}
		if (!strcmp(argv[i], "decrypt")) {
			if (argument.operation) {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n");
				return 0;
			}
			argument.operation   = 2;
			argument.input_file  = argv[i+1];
			argument.output_file = argv[i+2];
		}
		if (!strcmp(argv[i], "help")) {
			help();
			return 0;
		}
	}
	file_opener_and_writer(&argument);

	return 0;
}

