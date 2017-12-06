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
char* get_file(char* filename, long* filesize)
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
    char* buffer = calloc(*filesize, sizeof(char));
    if(!fread(buffer, *filesize, 1, file)) { exit(1); }
    fclose(file);
    return buffer;
}

/**
 * Opens the input and output files, creates buffers,
 * triggers the encode and decode loops
 */
void orchestrator(arguments* arguments)
{
    thread_args args;
    args.threads = arguments->threads;
    /** Parses key file, create matrix */
    args.matrix = matrix(arguments->keyfile);
    args.buffer_input = get_file(arguments->input_file, &args.end);

    /** ENCODE */
    if (arguments->operation == 1) {
        args.buffer_output = malloc((args.end * 2) * sizeof(char));
        for(int i = 0; i < arguments->threads; i++) {
            int err = pthread_create(&args.g_loops[i], NULL, &worker_encoder, &args);
            if (err != 0) {
                fprintf(stderr, "Can't create thread: [%s]\n", strerror(err));
            }
        }
    }
    /** DECODE */
    if (arguments->operation == 2) {
        args.end /= 2;
        args.buffer_output = malloc((args.end) * sizeof(char));
        for(int i = 0; i < arguments->threads; i++) {
            int err = pthread_create(&args.g_loops[i], NULL, &worker_decoder, &args);
            if (err != 0) {
                fprintf(stderr, "Can't create thread: [%s]\n", strerror(err));
            }
        }
    }

    for(int i = 0; i < arguments->threads; i++) {
        pthread_join(args.g_loops[i], NULL);
    }

    /** Dumps output buffer to output file */
    FILE* output = fopen(arguments->output_file, "wb");
    if (!output) {
        fprintf(stderr, "Output file \"%s\" not accessible.\nUse --help.\n", arguments->output_file);
        exit(25);
    }
    fwrite(
        args.buffer_output,
        sizeof(char),
        arguments->operation == 1 ? args.end * 2 : args.end,
        output
    );
    fclose(output);

    free(args.buffer_input);
    free(args.buffer_output);
    free(args.matrix);
}
