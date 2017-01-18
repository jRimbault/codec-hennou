/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-08 22:00:10
 * @Last Modified by:   jRimbault
 * @Description:
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
 * Function to check arguments
 */
int arg_is(char* given_arg,char* long_arg, char* short_arg) {
	if ((!strcmp(given_arg, long_arg)) || (!strcmp(given_arg, short_arg))) {
		return 1;
	}
	return 0;
}
/*
 * Help triggered if asked or if wrong arguments were given.
 */
void help() {
	printf("Usage: codec [--mode inputfile outputfile] [--progress] [--thread 1-4] [--key keyfile] [--help]\n\n"
			"Encode or decode any file with a G4C key.\n\n"
			"Options:\n"
			"    Modes:\n"
			"      Followed by the input file and the output file\n"
			"        --encode  -e    encoding mode\n"
			"             or\n"
			"        --decode  -d    decoding mode\n"
			"    --key       -k      followed by keyfile\n"
			"    --help      -h      show this help\n"
			"    --progress  -p      progress indicator !not enabled in multithreaded mode\n"
			"    --thread    -t      followed by a number 1-4\n\n"
			"Exemples:\n"
			"    To encode a file:\n"
			"        codec -e file.jpg file.jpg.c -k key.txt\n\n"
			"    To decode the resulting file:\n"
			"        codec -d file.jpg.c file.jpg -k key.txt\n\n"
			"Made with ♥ by Jacques Rimbault and Neil Amari.\n"
			"Note: the visible progress increases compute time by up to 40%%.\n"
			);
}

/*
 * A simple progress indicator
 * it adds around 30 to 40% execution time
 * on small files, 5 to 20% on large files
 * @NOTE: NOT IMPLEMENTED IN MUTLITHREADED WORKLOAD
 */
void progress_indicator(long i, long filelen) {
	filelen = filelen / 100;
	if ((i % filelen) == 0) {
		printf(" %ld%%\r", i/filelen);
		fflush(stdout);
	}
}

void* main_loop(void* structure) {
	thread_args* args = structure;
	long i;
	int  j;

	for (j = 0; j < NUM_THREADS; j++) {
		/*
		 * This pthread_equal could could be the only `if`,
		 * in place of the modulo check, but a pthread_equal 
		 * costs more time than an modulo check, so performing it only NUM_THREADS times
		 * instead of (args->end) times is better
		 */
		if (pthread_equal(pthread_self(), args->g_loops[j])) {
			for (i = 0; i < args->end; i++) {
				if (i % NUM_THREADS == j) {
					switch(args->operation) {
						case 1:
							args->buffer_output[i*2]   = encode_switch(quartet_1(args->buffer_input[i]));
							args->buffer_output[i*2+1] = encode_switch(quartet_2(args->buffer_input[i]));
							break;
						case 2:
							args->buffer_output[i] = decode_switch(args->buffer_input[i*2]) + (decode_switch(args->buffer_input[i*2+1]) << 4);
							break;
						default:
							printf("How did you even get here??\n");
							pthread_exit(NULL);
							break;
					}
				}
			}
		}
	}
	pthread_exit(NULL);
}

/*
 * Opens the input and output files, creates buffers,
 * triggers the encode and decode loops
 * @TODO: add the parsing function of the key file in there
 */
void file_opener_and_writer(void* structure) {
	arguments* arguments = structure;
	thread_args args;
	FILE* input;
	FILE* output;
	long  filelen;
	int   err, i;

	/*
	 * Either successfully opens the input and output files,
	 * or fails and print the help() and an error hint.
	 */
	input = fopen(arguments->input_file, "rb");
	if (input) {
		remove(arguments->output_file);
		output = fopen(arguments->output_file, "wb");
		if (output) {
			/* 
			 * Builds the buffers 
			 * of the input and output files
			 * Builds the threads arguments
			 */
			fseek(input, 0, SEEK_END);
			filelen             = ftell(input);
			rewind(input);
			args.buffer_input   = (char *)malloc((filelen + 1)*sizeof(char));
			args.buffer_output  = (char *)malloc((filelen + 1)*sizeof(char)*2);
			args.progress       = arguments->progress;
			args.thread_num_arg = arguments->thread_num_arg;
			args.operation      = arguments->operation;
			fread(args.buffer_input, filelen, 1, input);
			
			/*
			 * Either encode or decode
			 * 1 -> encode
			 * 2 -> decode
			 */
			switch(arguments->operation) {
				case 1:
					for(i = 0; i < NUM_THREADS; i++) {
						args.end = filelen;
						err = pthread_create(&(args.g_loops[i]), NULL, &main_loop, (void *)&args);
						if (err != 0) {
							printf("Can't create thread :[%s]\n", strerror(err));
						}
					}

					for(i = 0; i < NUM_THREADS; i++) {
						pthread_join(args.g_loops[i], NULL);
					}

					fwrite(args.buffer_output, sizeof(char), filelen * 2, output);
					break;
				case 2:
					for(i = 0; i < NUM_THREADS; i++) {
						args.end = filelen / 2;
						err = pthread_create(&(args.g_loops[i]), NULL, &main_loop, (void *)&args);
						if (err != 0) {
							printf("Can't create thread :[%s]\n", strerror(err));
						}
					}

					for(i = 0; i < NUM_THREADS; i++) {
						pthread_join(args.g_loops[i], NULL);
					}
					
					fwrite(args.buffer_output, sizeof(char), filelen / 2, output);
					break;
				default:
					printf("How did you even get here??\n");
					break;
			}

			/*
			 * Clear buffers
			 */
			free(args.buffer_input);
			free(args.buffer_output);
			/*
			 * Close output file
			 */
			fclose(output);
		} else {
			printf("Wrong or no output file %s.\n", arguments->output_file);
			help();
		}
		/*
		 * Close input file
		 */
		fclose(input);
	} else {
		printf("Input file %s not found or not accessible.\n", arguments->input_file);
		help();
	}
	if (arguments->progress) {
		printf("Done!\n");
	}
}
