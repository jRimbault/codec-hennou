/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-08 21:59:30
 * @Last Modified by:   jRimbault
 * @Description: 
 */

#include "functions.c"

int arg_is(char*, char*, char*);
/*
 * Help triggered if asked or if wrong arguments were given.
 */
void help();

/*
 * A simple progress indicator
 * it adds around 30 to 40% execution time
 * on small files, 5 to 20% on large files
 */
void progress_indicator(long, long);

/*
 * Loops on individual bytes
 */
void* threaded_worker(void* arg);

/*
 * Opens the input and output files, creates buffers,
 * triggers the encode and decode loops
 */
void file_opener_and_writer(void*);
