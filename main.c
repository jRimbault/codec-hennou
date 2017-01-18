/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
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

/*
 * main() serves only to parse the arguments and call the actual
 * program after parsing
 */
int main(int argc, char** argv) {
	arguments arguments;
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
	while(--argc) {
		if (arg_is(argv[argc], "--progress", "-p")) {
			arguments.progress    = 1;
		}
		if (arg_is(argv[argc], "--thread", "-t")) {
			arguments.thread_num_arg = atoi(argv[argc+1]);
		}
		if (arg_is(argv[argc], "--key", "-k")) {
			arguments.keyfile     = argv[argc+1];
		}
		if (arg_is(argv[argc], "--encode", "-e")) {
			if (arguments.operation) {
				printf("Invalid operation. Either 'encode' or 'decode'.\n");
				return 1;
			}
			arguments.operation   = 1;
			arguments.input_file  = argv[argc+1];
			arguments.output_file = argv[argc+2];
		}
		if (arg_is(argv[argc], "--decode", "-d")) {
			if (arguments.operation) {
				printf("Invalid operation. Either 'encode' or 'decode'.\n");
				return 1;
			}
			arguments.operation   = 2;
			arguments.input_file  = argv[argc+1];
			arguments.output_file = argv[argc+2];
		}
		if (arg_is(argv[argc], "--help", "-p")) {
			help();
			return 0;
		}
	}
	if (arguments.operation && arguments.input_file && arguments.output_file) {
		file_opener_and_writer(&arguments);
	} else {
		help();
	}

	return 0;
}

