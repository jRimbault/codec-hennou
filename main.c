/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
 * @Description: projet de secu, encodage
 * programme original en python réécriture en C pour vitesse
 *
 * `gcc -lpthread -lm -O2 main.c -o codec`
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
	int cpt;
	cpt = argc;
	/*
	 * Parses the arguments
	 * Checking if sane arguments were given
	 * before triggering the program
	 */
	arguments.progress       = 0;
	arguments.thread_num_arg = 1;
	arguments.operation      = 0;
	while(--cpt) {
		if (arg_is(argv[cpt], "--progress", "-p")) {
			arguments.progress    = 1;
		}
		if ((arg_is(argv[cpt], "--thread", "-t")) && (argc > cpt+1)) {
			arguments.thread_num_arg = atoi(argv[cpt+1]);
		}
		if ((arg_is(argv[cpt], "--key", "-k")) && (argc > cpt+1)) {
			arguments.keyfile     = argv[cpt+1];
		}
		if ((arg_is(argv[cpt], "--encode", "-e")) && (argc > cpt+2)) {
			if (arguments.operation) {
				printf("Invalid operation. Either 'encode' or 'decode'.\n");
				return 1;
			}
			arguments.operation   = 1;
			arguments.input_file  = argv[cpt+1];
			arguments.output_file = argv[cpt+2];
		}
		if ((arg_is(argv[cpt], "--decode", "-d")) && (argc > cpt+2)) {
			if (arguments.operation) {
				printf("Invalid operation. Either 'encode' or 'decode'.\n");
				return 1;
			}
			arguments.operation   = 2;
			arguments.input_file  = argv[cpt+1];
			arguments.output_file = argv[cpt+2];
		}
		if (arg_is(argv[cpt], "--help", "-h")) {
			help();
			return 0;
		}
	}
	/*
	 * If sane arguments were given, proceed to the main function
	 */
	if (arguments.operation && arguments.input_file && arguments.output_file) {
		file_opener_and_writer(&arguments);
	} else {
		printf("You didn't tell me what to do! Use --help.\n");
		return 1;
	}

	return 0;
}

