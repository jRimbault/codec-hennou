/**
 * @Author: jRimbault
 * @Date:   2017-01-08 22:00:10
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-11 16:29:34
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

#define NUM_THREADS 4

pthread_t g_loops[NUM_THREADS];

typedef struct thread_args {
	char* buffer_input;
	char* buffer_output; 
	long begin;
	long end;
	int progress;
	int thread_num_arg;
} thread_args;

typedef struct arguments {
	int operation;
	char* input_file;
	char* output_file;
	int progress;
	int thread_num_arg;
} arguments;
/*
 * Help triggered if asked or if wrong arguments were given.
 */
void help() {
	printf("Usage: codec [help] [mode] [key file] [input file] [output file] [-progress]\n\n"
			"Encode or decode any file with a G4C key.\n\n"
			"Options:\n"
			"    help       show this help\n"
			"    encrypt    encoding mode\n"
			"            or              \n"
			"    decrypt    decoding mode\n"
			"    -progress  progress indicator\n"
			"    -thread    followed by a number 1-4\n\n"
			"Exemples:\n"
			"    To encrypt a file:\n"
			"        codec encrypt key.txt file.jpg file.jpg.c\n\n"
			"    To decrypt the resulting file:\n"
			"        codec decrypt key.txt file.jpg.c file.jpg\n\n"
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

void* encode_loop(void* arg) {
    struct    thread_args *args = arg;
	char*     buffer_input      = args->buffer_input;
	char*     buffer_output     = args->buffer_output;
	pthread_t id                = pthread_self();
	long      i, j;

	for (j = 0; j < NUM_THREADS; j++) {
		/*
		 * This pthread_equal could could be the only `if`,
		 * in place of the modulo check, but a pthread_equal 
		 * costs more time than an modulo check, so performing it only NUM_THREADS times
		 * instead of (end-begin) times is better
		 */
		if (pthread_equal(id, g_loops[j])) {
			for (i = args->begin; i < args->end; i++) {
				if (i%NUM_THREADS==j)	{
					buffer_output[i*2]   = encode_character_switch(quartet_1(buffer_input[i]));
					buffer_output[i*2+1] = encode_character_switch(quartet_2(buffer_input[i]));
				}
				/*progress_indicator(i, args->progress);*/
			}
		}
	}
	pthread_exit(NULL);
}

void* decode_loop(void* arg) {
	struct    thread_args *args = arg;
	char*     buffer_input      = args->buffer_input;
	char*     buffer_output     = args->buffer_output;
	pthread_t id                = pthread_self();
	long      i, j;
	char      c, d;

	for (j = 0; j < NUM_THREADS; j++) {
		/*
		 * This pthread_equal could could be the only `if`,
		 * in place of the modulo check, but a pthread_equal 
		 * costs more time than an modulo check, so performing it only NUM_THREADS times
		 * instead of (end-begin) times is better
		 */
		if (pthread_equal(id, g_loops[j])) {
			for (i = args->begin; i < args->end; i++) {
				if (i%NUM_THREADS==j)	{
					c = decode_character_switch(buffer_input[i*2]);
					d = decode_character_switch(buffer_input[i*2+1]) << 4; /*Bitshift pour être sur*/
					buffer_output[i] = c + d;
				}
				/*progress_indicator(i, args->progress);*/
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
void file_opener_and_writer(void* arg) {
	arguments *arguments = arg;
	thread_args args;
	FILE* input;
	FILE* output;
	char* buffer_input;
	char* buffer_output;
	long  filelen;
	int   err, i;
	/*
	 * Either successfully opens the input and output files,
	 * or fails and print the help() and an error hint.
	 */
	input = fopen(arguments->input_file, "rb");
	// printf("SOME.\n");
	if (input) {
		output = fopen(arguments->output_file, "wb");
		if (output) {
			/* 
			 * Construction des buffer de bytes 
			 * du fichier d'input et d'output
			 */
			fseek(input, 0, SEEK_END);
			filelen       = ftell(input);
			rewind(input);
			buffer_input  = (char *)malloc((filelen + 1)*sizeof(char));
			buffer_output = (char *)malloc((filelen + 1)*sizeof(char)*2);
			fread(buffer_input, filelen, 1, input);
			
			args.buffer_input   = buffer_input;
			args.buffer_output  = buffer_output;
			args.begin          = 0;
			args.progress       = arguments->progress;
			args.thread_num_arg = arguments->thread_num_arg;
			/*
			 * Either encrypt or decrypt
			 * arguments-> first argument
			 */
			if (arguments->operation == 1) {
				for(i = 0; i < NUM_THREADS; i++) {
					args.end = filelen;
					err = pthread_create(&(g_loops[i]), NULL, &encode_loop, (void *)&args);
					if (err != 0) {
						printf("Can't create thread :[%s]\n", strerror(err));
					}
				}
				for(i = 0; i < NUM_THREADS; i++) {
					pthread_join(g_loops[i], NULL);
				}
				fwrite(buffer_output, sizeof(char), filelen * 2, output);
			} else if (arguments->operation == 2) {
				for(i = 0; i < NUM_THREADS; i++) {
					args.end = filelen/2;
					err = pthread_create(&(g_loops[i]), NULL, &decode_loop, (void *)&args);
					if (err != 0) {
						printf("Can't create thread :[%s]\n", strerror(err));
					}
				}
				for(i = 0; i < NUM_THREADS; i++) {
					pthread_join(g_loops[i], NULL);
				}
				fwrite(buffer_output, sizeof(char), filelen / 2, output);
			} else {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n");
				help();
			}

			/*
			 * Clear buffers
			 */
			free(buffer_input);
			free(buffer_output);
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
	// printf("SOME.\n");
}
