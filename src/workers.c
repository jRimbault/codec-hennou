/**
 * @author: jRimbault
 * @date:   2017-11-21
 * @last modified by:   jRimbault
 * @last modified time: 2017-11-26
 */

#include <stdlib.h>
#include <pthread.h>
#include "structs.h"
#include "codec.h"


/** To find the thread index, [0|1|2|3] */
int get_thread_index(pthread_t* loops)
{
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_equal(pthread_self(), loops[i])) {
            return i;
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

    for (unsigned long i = thread; i < args->end; i += NUM_THREADS) {
        args->buffer_output[i*2]   = args->matrix[quartet_1(args->buffer_input[i])];
        args->buffer_output[i*2+1] = args->matrix[quartet_2(args->buffer_input[i])];
    }

    pthread_exit(NULL);
}

void* worker_decoder(void* structure)
{
    thread_args* args = structure;
    int thread = get_thread_index(args->g_loops);
    if (thread < 0) { exit(30); }

    for (unsigned long i = thread; i < args->end; i += NUM_THREADS) {
            args->buffer_output[i]
                = decode_switch(args->buffer_input[i*2], args->matrix)
                + (decode_switch(args->buffer_input[i*2+1], args->matrix) << 4);
    }

    pthread_exit(NULL);
}
