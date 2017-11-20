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

/**
 * A simple progress indicator
 * it adds around *up* to 50% execution time
 * @param i   current step
 * @param end maximum
 * @Note: Not 'correctly' implemented in multithreaded workload
 */
void progress_indicator(long i, long end)
{
	end = end / 100;
	if ((i % end) == 0) {
		printf(" %ld%%\r", i/end);
		fflush(stdout);
	}
}

/** To find the thread index, [0|1|2|3] */
int get_thread_index(pthread_t* loops)
{
	for (int j = 0; j < NUM_THREADS; j++) {
		if (pthread_equal(pthread_self(), loops[j])) {
			return j;
		}
	}
	return -1;
}
/**
 * Those are the thread worker functions
 * It splits tasks between threads
 * @param structure containing several arguments:
 *   > g_loops        array containing thread's id
 *   > end            end of the input buffer
 *   > buffer_input   reading buffer
 *   > buffer_output  writing buffer
 *   > matrix         array key matrix
 */
void* worker_encoder(void* structure)
{
	thread_args* args = structure;
	int thread = get_thread_index(args->g_loops);
	if (thread < 0) { exit(30); }

	for (long i = thread; i < args->end; i += NUM_THREADS) {
		args->buffer_output[i*2]   = encode_switch(quartet_1(args->buffer_input[i]), args->matrix);
		args->buffer_output[i*2+1] = encode_switch(quartet_2(args->buffer_input[i]), args->matrix);
	}

	pthread_exit(NULL);
}
void* worker_decoder(void* structure)
{
	thread_args* args = structure;
	int thread = get_thread_index(args->g_loops);
	if (thread < 0) { exit(30); }

	for (long i = thread; i < args->end; i += NUM_THREADS) {
			args->buffer_output[i]
				= decode_switch((args->buffer_input[i*2]), args->matrix)
				+ (decode_switch(args->buffer_input[i*2+1], args->matrix) << 4);
	}

	pthread_exit(NULL);
}
/** I don't want to get a double, so here's my own pow function */
int pow_int(int n, int p)
{
	if (p > 1) {
		return n * pow_int(n, p - 1);
	}
	return n;
}
/**
 * Parses key file, create matrix
 *
 * @param  filename file containing the matrix
 *
 * @return          matrix G4C
 */
char* matrix(char* filename)
{
	FILE* keyfile;
	char* matrix;
	char  keychar[35];
	int i, j;

	keyfile = fopen(filename, "r");
	if (!keyfile) {
		printf("Couldn't access key file. Use --help.\n");
		exit(10);
	}

	fseek(keyfile, 5, SEEK_SET);
	for (i = 0; i < 35; i++) {
		keychar[i] = getc(keyfile);
	}
	matrix = malloc(4 * sizeof(char));
	for (j = 0; j < 4; j++) {
		matrix[j] = 0;
		for (i = 0; i < 8; i++) {
			// 48 = 0 ascii
			// 49 = 1 ascii
			if (keychar[i+(j*9)] == 49) {
				matrix[j] += pow_int(2, 7-i);
			}
		}
	}
	fclose(keyfile);
	return matrix;
}

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
