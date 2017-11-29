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

/** Shortened version of my file_to_string function */
char* get_file(char* filename, size_t* filesize)
{
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "File \"%s\" not readable. Use --help.\n", filename);
        exit(25);
    }

    fseek(file, 0, SEEK_END);
    *filesize = ftell(file);
    rewind(file);

    /** Reads whole file into buffer */
    char* buffer = malloc(*filesize * sizeof(char));
    if(!fread(buffer, *filesize, 1, file)) { exit(1); }
    fclose(file);
    return buffer;
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
void orchestrator(arguments* arguments)
{
    thread_args args_t;

    /** Parses key file, create matrix */
    args_t.matrix = matrix(arguments->keyfile);
    args_t.buffer_input = get_file(arguments->input_file, &args_t.end);

    /** ENCODE */
    if (arguments->operation == 1) {
        args_t.buffer_output = malloc((args_t.end * 2) * sizeof(char));
        for(int i = 0; i < NUM_THREADS; i++) {
            int err = pthread_create(&args_t.g_loops[i], NULL, &worker_encoder, &args_t);
            if (err != 0) {
                fprintf(stderr, "Can't create thread: [%s]\n", strerror(err));
            }
        }
    }
    /** DECODE */
    if (arguments->operation == 2) {
        args_t.end /= 2;
        args_t.buffer_output = malloc((args_t.end) * sizeof(char));
        for(int i = 0; i < NUM_THREADS; i++) {
            int err = pthread_create(&args_t.g_loops[i], NULL, &worker_decoder, &args_t);
            if (err != 0) {
                fprintf(stderr, "Can't create thread: [%s]\n", strerror(err));
            }
        }
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(args_t.g_loops[i], NULL);
    }

    /** Dumps output buffer to output file */
    FILE* output = fopen(arguments->output_file, "wb");
    if (!output) {
        fprintf(stderr, "Output file \"%s\" not accessible.\nUse --help.\n", arguments->output_file);
        exit(25);
    }
    fwrite(
        args_t.buffer_output,
        sizeof(char),
        arguments->operation == 1 ? args_t.end * 2 : args_t.end,
        output
    );
    fclose(output);

    free(args_t.buffer_input);
    free(args_t.buffer_output);
    free(args_t.matrix);
}
