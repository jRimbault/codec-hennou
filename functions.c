/**
 * @Author: jRimbault
 * @Date:   2017-01-08 22:00:10
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-08 22:17:22
 * @Description:
 */

#include "header.h"

char matrice[4] = {0b10001111, 0b11000111, 0b10100100, 0b10010010};

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
 * Receives a half byte (4bits) and returns the corresponding encoded byte
 */
char encode_character_switch(char c) {
	switch(c) {
		case 0b0000: //  0
			return 0;
		case 0b0001: //  1
			return matrice[3];
		case 0b0010: //  2
			return matrice[2];
		case 0b0011: //  3
			return (matrice[2] ^ matrice[3]);
		case 0b0100: //  4
			return matrice[1];
		case 0b0101: //  5
			return (matrice[1] ^ matrice[3]);
		case 0b0110: //  6
			return (matrice[1] ^ matrice[2]);
		case 0b0111: //  7
			return (matrice[1] ^ matrice[2] ^ matrice[3]);
		case 0b1000: //  8
			return matrice[0];
		case 0b1001: //  9
			return (matrice[0] ^ matrice[3]);
		case 0b1010: // 10
			return (matrice[0] ^ matrice[2]);
		case 0b1011: // 11
			return (matrice[0] ^ matrice[2] ^ matrice[3]);
		case 0b1100: // 12
			return (matrice[0] ^ matrice[1]);
		case 0b1101: // 13
			return (matrice[0] ^ matrice[1] ^ matrice[3]);
		case 0b1110: // 14
			return (matrice[0] ^ matrice[1] ^ matrice[2]);
		case 0b1111: // 15
			return (matrice[0] ^ matrice[1] ^ matrice[2] ^ matrice[3]);
		default:
			/* 
			 * Shouldn't ever be triggered since all
			 * possible combinations of 4bits have been made. 0-15
			 */
			return 0;
	}
}

/*
 * Receives an encoded byte and returns the original byte
 */
char decode_character_switch(char c) {
	if(c == 0) {
		return 0;
	} else if (c == matrice[3]) {
		return 0b0001;
	} else if (c == matrice[2]) {
		return 0b0010;
	} else if (c == (matrice[2] ^ matrice[3])) {
		return 0b0011;
	} else if (c == matrice[1]) {
		return 0b0100;
	} else if (c == (matrice[1] ^ matrice[3])) {
		return 0b0101;
	} else if (c == (matrice[1] ^ matrice[2])) {
		return 0b0110;
	} else if (c == (matrice[1] ^ matrice[2] ^ matrice[3])) {
		return 0b0111;
	} else if (c == matrice[0]) {
		return 0b1000;
	} else if (c == (matrice[0] ^ matrice[3])) {
		return 0b1001;
	} else if (c == (matrice[0] ^ matrice[2])) {
		return 0b1010;
	} else if (c == (matrice[0] ^ matrice[2] ^ matrice[3])) {
		return 0b1011;
	} else if (c == (matrice[0] ^ matrice[1])) {
		return 0b1100;
	} else if (c == (matrice[0] ^ matrice[1] ^ matrice[3])) {
		return 0b1101;
	} else if (c == (matrice[0] ^ matrice[1] ^ matrice[2])) {
		return 0b1110;
	} else if (c == (matrice[0] ^ matrice[1] ^ matrice[2] ^ matrice[3])) {
		return 0b1111;
	} else {
		/* 
		 * Shouldn't ever be triggered since all
		 * possible combinations of 8bits have been made. 0-255
		 */
		return 0;
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
			buffer_output = (char *)malloc((filelen + 1)*sizeof(char)*2);
			fread(buffer_input, filelen, 1, input);
			/*
			 * Either encrypt or decrypt
			 * argv[1] first argument
			 */
			if (!strcmp(argv[1], "encrypt")) {
				encode_loop(buffer_input, buffer_output, filelen, progress);
				fwrite(buffer_output, sizeof(char), filelen * 2, output);
			} else if (!strcmp(argv[1], "decrypt")) {
				decode_loop(buffer_input, buffer_output, filelen/2, progress);
				fwrite(buffer_output, sizeof(char), filelen / 2, output);
			} else {
				printf("Invalid operation. Either 'encrypt' or 'decrypt'.\n");
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
		} else {
			printf("Wrong or no output file.\n");
			help();
		}
		/*
		 * Close input file
		 */
		fclose(input);
	} else {
		printf("Input file not found or not accessible.\n");
		help();
	}
	if (progress) {
		printf("Done!\n");
	}
}
