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

/*
 * A simple progress indicator
 * it adds around *up* to 50% execution time
 * @NOTE: NOT IMPLEMENTED IN MUTLITHREADED WORKLOAD
 */
void progress_indicator(long i, long end) {
	end = end / 100;
	if ((i % end) == 0) {
		printf(" %ld%%\r", i/end);
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
							args->buffer_output[i*2]   = encode_switch(quartet_1(args->buffer_input[i]), args->matrix);
							args->buffer_output[i*2+1] = encode_switch(quartet_2(args->buffer_input[i]), args->matrix);
							if (args->progress) {
								progress_indicator(i, args->end);
							}
							break;
						case 2:
							args->buffer_output[i] = decode_switch((args->buffer_input[i*2]), args->matrix);
							args->buffer_output[i] = args->buffer_output[i] + (decode_switch(args->buffer_input[i*2+1], args->matrix) << 4);
							if (args->progress) {
								progress_indicator(i, args->end);
							}
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
 * @TODO: add a real parsing function of the key file in there
 */
void file_opener_and_writer(void* structure) {
	arguments* arguments = structure;
	thread_args args;
	FILE* input;
	FILE* output;
	FILE* keyfile;
	char  keychar[35];
	long  filelen;
	int   err, i, j;


	keyfile = fopen(arguments->keyfile, "r");
	if (keyfile) {      
		fseek(keyfile, 5, SEEK_SET);
		for (i = 0; i < 35; i++) {
			keychar[i] = getc(keyfile);
		}
		args.matrix = (char *)malloc(4*sizeof(char));
		for (j = 0; j < 4; j++) {
			args.matrix[j] = 0;
			for (i = 0; i < 8; i++) {
				if (keychar[i+(j*9)] == 49) {
					args.matrix[j] = args.matrix[j] + pow(2, 7-i);
				}
			}
			// printf("%d\n", args.matrix[j]);
		}
		fclose(keyfile);
	} else {
		printf("Couldn't access key file. Use --help.\n");
		return;
	}
	/*
	 * Either successfully opens the input and output files,
	 * or fails and print the help() and an error hint.
	 */
	input = fopen(arguments->input_file, "rb");
	if (!input) {
		printf("Input file \"%s\" not accessible.\nUse --help.\n", arguments->input_file);
	} else {
		remove(arguments->output_file);
		output = fopen(arguments->output_file, "wb");
		if (!output) {
			printf("Output file \"%s\" not accessible.\nUse --help.\n", arguments->output_file);
		} else {
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
			}

			/*
			 * Clear buffers
			 */
			free(args.buffer_input);
			free(args.buffer_output);
			free(args.matrix);
			/*
			 * Close output file
			 */
			fclose(output);
		}
		/*
		 * Close input file
		 */
		fclose(input);
	}
	if (arguments->progress) {
		printf("Done!\n");
	}
}
