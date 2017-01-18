/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-08 22:01:39
 * @Last Modified by:   jRimbault
 * @Description: standards libraries used
 * number of threads
 * structures definition
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct thread_args {
	pthread_t g_loops[NUM_THREADS];
	char*     buffer_input;
	char*     buffer_output;
	long      end;
	int       operation;
	int       progress;
	int       thread_num_arg;
} thread_args;

typedef struct arguments {
	char* input_file;
	char* output_file;
	char* keyfile;
	int   operation;
	int   progress;
	int   thread_num_arg;
} arguments;
