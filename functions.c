/**
 * @Author: jRimbault
 * @Date:   2017-01-08 22:00:10
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-10 07:27:05
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

typedef struct arg_struct {
	char* input;
	char* output; 
	long end;
	int progress;
} arg_struct;

/*
 * Help triggered if asked or if wrong arguments were given.
 */
void help() {
	printf("Usage: codec [help] [mode] [key file] [input file] [output file] [-progress]\n\n"
			"Encode or decode any file with a G4C key.\n\n"
			"Options:\n"
			"    help       show this help\n"
			"    encrypt    encoding mode\n"
			"            or              \n"
			"    decrypt    decoding mode\n"
			"    -progress  progress indicator\n\n"
			"Exemples:\n"
			"    To encrypt a file:\n"
			"        codec encrypt key.txt file.jpg file.jpg.c\n\n"
			"    To decrypt the resulting file:\n"
			"        codec decrypt key.txt file.jpg.c file.jpg\n\n"
			"Made with ♥ by Jacques Rimbault and Neil Amari.\n"
			"Note: the visible progress increases compute time by up to 40%%.\n"
			);
}

/*
 * A simple progress indicator
 * it adds around 30 to 40% execution time
 * on small files, 5 to 20% on large files
 */
void progress_indicator(long i, long filelen) {
	filelen = filelen / 100;
	if ((i % filelen) == 0) {
		printf(" %ld%%\r", i/filelen);
		fflush(stdout);
	}
}

/*
 * Left bitshift
 */
char quartet_1(char c) {
	return (((c << 4) >> 4) & 0x0f);
}
/*
 * Right bitshift
 */
char quartet_2(char c) {
	return ((c >> 4) & 0x0f);
}

void encode_loop(char* buffer_input,
				char* buffer_output, 
				long filelen, 
				int progress) {
	long i;
	for (i = 0; i < filelen; i++) {
		buffer_output[i*2]   = encode_character_switch(quartet_1(buffer_input[i]));
		buffer_output[i*2+1] = encode_character_switch(quartet_2(buffer_input[i]));
		if (progress) {
			progress_indicator(i, filelen);
		}
	}
}

void decode_loop(char* buffer_input,
				char* buffer_output, 
				long filelen, 
				int progress) {
	long i;
	char c, d;
	for (i = 0; i < filelen; i++) {
		c = decode_character_switch(buffer_input[i*2]);
		d = decode_character_switch(buffer_input[i*2+1]) << 4; /*Bitshift pour être sur*/
		buffer_output[i] = c + d;
		if (progress) {
			progress_indicator(i, filelen);
		}
	}
}

/*
 * Opens the input and output files, creates buffers,
 * triggers the encode and decode loops
 * @TODO: add the parsing of the key file in there
 * @NOTE: maybe do some multi threading in the future
 */
void file_opener_and_writer(char** argv, int progress) {
	FILE* input;
	FILE* output;
	long filelen;
	char* buffer_input;
	char* buffer_output;
	
	/*
	 * Either successfully opens the input and output files,
	 * or fails and print the help() and an error hint.
	 */
	input = fopen(argv[2], "rb");
	if (input) {
		output = fopen(argv[3], "wb");
		if (output) {
			/* 
			 * Construction des buffer de bytes 
			 * du fichier d'input et d'output
			 */
			fseek(input, 0, SEEK_END);
			filelen = ftell(input);
			rewind(input);
			buffer_input = (char *)malloc((filelen + 1)*sizeof(char));
			fread(buffer_input, filelen, 1, input);
			/*
			 * Either encrypt or decrypt
			 * argv[1] first argument
			 */
			if (!strcmp(argv[1], "encrypt")) {
				buffer_output = (char *)malloc((filelen + 1)*sizeof(char)*2);
				encode_loop(buffer_input, buffer_output, filelen, progress);
				fwrite(buffer_output, sizeof(char), filelen * 2, output);
			} else if (!strcmp(argv[1], "decrypt")) {
				buffer_output = (char *)malloc((filelen + 1)*sizeof(char)/2);
				decode_loop(buffer_input, buffer_output, filelen / 2, progress);
				fwrite(buffer_output, sizeof(char), filelen / 2, output);
			} else {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n\n");
				help();
			}

			/*
			 * Clear buffers
			 */
			free(buffer_input);
			free(buffer_output);
			/*
			 * Close output file
			 */
			fclose(output);
			if (progress) {
				printf("Done!\n");
			}
		} else {
			printf("Wrong output file '%s'. Check if you have write access.\n\n", argv[3]);
			help();
		}
		/*
		 * Close input file
		 */
		fclose(input);
	} else {
		printf("Input file '%s' not found or not accessible.\n\n", argv[2]);
		help();
	}
}
