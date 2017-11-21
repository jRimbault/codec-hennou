/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-05 18:56:54
 * @Last Modified by:   jRimbault
 * @Description: projet de secu, encodage
 * programme original en python réécriture en C pour vitesse
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"

int arg_is(char*, char*, char*);
/** Help triggered if asked or if wrong arguments were given. */
void help();
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


/**
 * Function to check arguments
 * @param  given_arg string received
 * @param  long_arg  1st string it's compared to
 * @param  short_arg 2nd string it's compared to
 * @return           returns 1 if received string is either the 1st or 2nd,
 *                   else it returns 0
 */
int arg_is(char* given_arg,char* long_arg, char* short_arg)
{
	if ((!strcmp(given_arg, long_arg)) || (!strcmp(given_arg, short_arg))) {
		return 1;
	}
	return 0;
}

/**
 * Help triggered if asked or if wrong arguments were given.
 */
void help()
{
	printf("Usage: codec [--mode inputfile outputfile] [--progress] [--thread 1-4] [--key keyfile] [--help]\n\n"
			"Encode or decode any file with a G4C key.\n\n"
			"Options:\n"
			"    Modes:\n"
			"      Followed by the input file path and the output file path\n"
			"        --encode  -e    encoding mode\n"
			"             or\n"
			"        --decode  -d    decoding mode\n"
			"    --key       -k      followed by keyfile\n"
			"    --help      -h      show this help\n"
			"    --progress  -p      progress indicator !caution in multithreaded mode\n"
			"    --thread    -t      followed by a number 1-4\n\n"
			"Exemples:\n"
			"    To encode a file:\n"
			"        codec -e file.jpg file.jpg.c -k key.txt\n\n"
			"    To decode the resulting file:\n"
			"        codec -d file.jpg.c file.jpg -k key.txt\n\n"
			"Made with ♥ by Jacques Rimbault and Neil Amari.\n"
			"Note: the visible progress increases compute time by *up* to 50%%.\n"
			);
}
