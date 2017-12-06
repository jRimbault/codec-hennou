/**
 * @author: jRimbault
 * @date:   2017-11-21
 * @last modified by:   jRimbault
 * @last modified time: 2017-12-06
 */

#include <stdlib.h>
#include <pthread.h>
#include "structs.h"
#include "codec.h"


/** To find the thread index, [0|1|2|3] */
int get_thread_index(thread_args* args)
{
    for (int i = 0; i < args->threads; i++) {
        if (pthread_equal(pthread_self(), args->g_loops[i])) {
            return i;
        }
    }
    return -1;
}

/** Computes the lower and upper bounds for a given thread */
void get_worker_bounds(thread_args* args, long* start, long* end)
{
    int thread = get_thread_index(args);
    if (thread < 0) { exit(30); }

    long unit = args->end / args->threads;
    *start = thread * unit;
    *end = (*start) + unit;
    if (args->threads == thread + 1) {
        *end = args->end;
    }
}

/**
 * Those are the thread worker functions
 * Splits tasks between threads
 */
void* worker_encoder(thread_args* args)
{
    long start = 0;
    long end = 0;
    get_worker_bounds(args, &start, &end);

    for (long i = start; i < end; i += 1) {
        args->buffer_output[i*2]   = args->matrix[quartet_1(args->buffer_input[i])];
        args->buffer_output[i*2+1] = args->matrix[quartet_2(args->buffer_input[i])];
    }

    pthread_exit(NULL);
}

void* worker_decoder(thread_args* args)
{
    long start = 0;
    long end = 0;
    get_worker_bounds(args, &start, &end);

    for (long i = start; i < end; i += 1) {
        args->buffer_output[i]
            = decode_switch(args->buffer_input[i*2], args->matrix)
            + (decode_switch(args->buffer_input[i*2+1], args->matrix) << 4);
    }

    pthread_exit(NULL);
}
