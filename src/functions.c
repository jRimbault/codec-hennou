/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-08 22:00:10
 * @Last Modified by:   jRimbault
 * @Description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "structs.h"
#include "matrix.h"
#include "workers.h"

/**
 * Opens the input and output files, creates buffers,
 * triggers the encode and decode loops
 * @param structure containing several arguments:
 *   > input_file      name
 *   > output_file     name
 *   > keyfile         name
 *   > operation       1 or 2 (encode or decode)
 *   > progress        display progress indicator or not (1 or 0)
 *   > thread_num_arg  number of threads
 */
void file_opener_and_writer(void* structure)
{
	arguments* arguments = structure;
	thread_args args_t;
	FILE* input;
	FILE* output;
	long  filelen;
	int   err, i;

	/*
	 * Parses key file, create matrix
	 */
	args_t.matrix = matrix(arguments->keyfile);

	/*
	 * Either successfully opens the input and output files,
	 * or fails and print the help() and an error hint.
	 */
	input = fopen(arguments->input_file, "rb");
	if (!input) {
		printf("Input file \"%s\" not accessible.\nUse --help.\n", arguments->input_file);
		exit(25);
	}
	/*
	 * Builds the buffers
	 * of the input and output files
	 * Builds the threads arguments
	 * Extremely reliant on the amount of free RAM
	 */
	fseek(input, 0, SEEK_END);
	filelen = ftell(input);
	rewind(input);
	args_t.buffer_input = malloc(filelen * sizeof(char));

	args_t.progress       = arguments->progress;
	args_t.thread_num_arg = arguments->thread_num_arg;
	args_t.operation      = arguments->operation;
	/** Reads whole file into buffer_input */
	if(!fread(args_t.buffer_input, filelen, 1, input)) { exit(1); }
	fclose(input);

	/*
	 * Either encode or decode
	 * 1 -> encode
	 * 2 -> decode
	 */
	if (arguments->operation == 1) {
		args_t.end = filelen;
		args_t.buffer_output = malloc((filelen * 2) * sizeof(char));
		for(i = 0; i < NUM_THREADS; i++) {
			err = pthread_create(&args_t.g_loops[i], NULL, &worker_encoder, &args_t);
			if (err != 0) {
				printf("Can't create thread :[%s]\n", strerror(err));
			}
		}
	}
	if (arguments->operation == 2) {
		args_t.end = filelen / 2;
		args_t.buffer_output = malloc((filelen / 2) * sizeof(char));
		for(i = 0; i < NUM_THREADS; i++) {
			err = pthread_create(&args_t.g_loops[i], NULL, &worker_decoder, &args_t);
			if (err != 0) {
				printf("Can't create thread :[%s]\n", strerror(err));
			}
		}
	}

	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(args_t.g_loops[i], NULL);
	}

	/** Deletes the output file to overwrite it */
	// remove(arguments->output_file);
	output = fopen(arguments->output_file, "wb");
	if (!output) {
		printf("Output file \"%s\" not accessible.\nUse --help.\n", arguments->output_file);
		exit(25);
	}
	fwrite(
		args_t.buffer_output,
		sizeof(char),
		arguments->operation == 1 ? filelen * 2 : filelen / 2,
		output
	);

	/*
	 * Clear buffers
	 */
	free(args_t.buffer_input);
	free(args_t.buffer_output);
	free(args_t.matrix);
	/*
	 * Close output file
	 */
	fclose(output);
}
