
#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <pthread.h>

#define NUM_THREADS 4

typedef struct thread_args {
    pthread_t g_loops[NUM_THREADS];
    char*     buffer_input;
    char*     buffer_output;
    char*     matrix;
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

#endif // _STRUCTS_H
