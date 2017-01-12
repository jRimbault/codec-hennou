/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-12 14:11:08
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
	arguments arguments;
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
	arguments.progress       = 0;
	arguments.thread_num_arg = 1;
	arguments.operation      = 0;
	for(i = 0; i < argc; i++) {
		if ((!strcmp(argv[i], "--progress")) || (!strcmp(argv[i], "-p"))) {
			arguments.progress = 1;
		}
		if ((!strcmp(argv[i], "--thread")) || (!strcmp(argv[i], "-t"))) {
			arguments.thread_num_arg = atoi(argv[i+1]);
		}
		if ((!strcmp(argv[i], "--key")) || (!strcmp(argv[i], "-k"))) {
			arguments.keyfile = argv[i+1];
		}
		if ((!strcmp(argv[i], "--encrypt")) || (!strcmp(argv[i], "-e"))) {
			if (arguments.operation) {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n");
				return 0;
			}
			arguments.operation   = 1;
			arguments.input_file  = argv[i+1];
			arguments.output_file = argv[i+2];
		}
		if ((!strcmp(argv[i], "--decrypt")) || (!strcmp(argv[i], "-d"))) {
			if (arguments.operation) {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n");
				return 0;
			}
			arguments.operation   = 2;
			arguments.input_file  = argv[i+1];
			arguments.output_file = argv[i+2];
		}
		if ((!strcmp(argv[i], "--help")) || (!strcmp(argv[i], "-h"))) {
			help();
			return 0;
		}
	}
	file_opener_and_writer(&arguments);

	return 0;
}

