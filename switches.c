/**
 * @Author: jRimbault
 * @Date:   2017-01-09 14:16:04
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-11 12:45:13
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

char matrice[4] = {0b10001111, 0b11000111, 0b10100100, 0b10010010};

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

/*
 * Receives a half byte (4bits) and returns the corresponding encoded byte
 */
char encode_character_switch(char c) {
	switch(c) {
		case 0b0000: /*  0 */
			return 0;
		case 0b0001: /*  1 */
			return matrice[3];
		case 0b0010: /*  2 */
			return matrice[2];
		case 0b0011: /*  3 */
			return (matrice[2] ^ matrice[3]);
		case 0b0100: /*  4 */
			return matrice[1];
		case 0b0101: /*  5 */
			return (matrice[1] ^ matrice[3]);
		case 0b0110: /*  6 */
			return (matrice[1] ^ matrice[2]);
		case 0b0111: /*  7 */
			return (matrice[1] ^ matrice[2] ^ matrice[3]);
		case 0b1000: /*  8 */
			return matrice[0];
		case 0b1001: /*  9 */
			return (matrice[0] ^ matrice[3]);
		case 0b1010: /* 10 */
			return (matrice[0] ^ matrice[2]);
		case 0b1011: /* 11 */
			return (matrice[0] ^ matrice[2] ^ matrice[3]);
		case 0b1100: /* 12 */
			return (matrice[0] ^ matrice[1]);
		case 0b1101: /* 13 */
			return (matrice[0] ^ matrice[1] ^ matrice[3]);
		case 0b1110: /* 14 */
			return (matrice[0] ^ matrice[1] ^ matrice[2]);
		case 0b1111: /* 15 */
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
		return 0b0000;
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
