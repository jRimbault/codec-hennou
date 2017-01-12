/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-08 21:59:30
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-12 10:42:25
 * @Description: 
 */

#include "functions.c"

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
void* encode_loop(void*);
void* decode_loop(void*);

/*
 * Opens the input and output files, creates buffers,
 * triggers the encode and decode loops
 */
void file_opener_and_writer(void*);
