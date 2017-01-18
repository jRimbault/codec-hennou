/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-09 14:16:04
 * @Last Modified by:   jRimbault
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

/*
 * Right 4bits
 * c is char
 * char are one byte
 * `&` is the and operator, the resulting byte of the operation:
 *     `c & 00001111`
 * is the right side of the c byte
 */
char quartet_1(char c) {
	return (c & 0x0f);
}
/*
 * Left 4bits
 * `>> 4` makes the c byte move 4 bits to the right
 * same operation as above
 */
char quartet_2(char c) {
	return ((c >> 4) & 0x0f);
}

/*
 * Receives a half byte (4bits) and returns the corresponding encoded byte
 * @NOTE: ^ is the xor operator
 */
char encode_switch(char c) {
	char matrix[4] = {0b10001111, 0b11000111, 0b10100100, 0b10010010};
	switch(c) {
		case 0b0000: /*  0 */
			return 0;
		case 0b0001: /*  1 */
			return matrix[3];
		case 0b0010: /*  2 */
			return matrix[2];
		case 0b0011: /*  3 */
			return (matrix[2] ^ matrix[3]);
		case 0b0100: /*  4 */
			return matrix[1];
		case 0b0101: /*  5 */
			return (matrix[1] ^ matrix[3]);
		case 0b0110: /*  6 */
			return (matrix[1] ^ matrix[2]);
		case 0b0111: /*  7 */
			return (matrix[1] ^ matrix[2] ^ matrix[3]);
		case 0b1000: /*  8 */
			return matrix[0];
		case 0b1001: /*  9 */
			return (matrix[0] ^ matrix[3]);
		case 0b1010: /* 10 */
			return (matrix[0] ^ matrix[2]);
		case 0b1011: /* 11 */
			return (matrix[0] ^ matrix[2] ^ matrix[3]);
		case 0b1100: /* 12 */
			return (matrix[0] ^ matrix[1]);
		case 0b1101: /* 13 */
			return (matrix[0] ^ matrix[1] ^ matrix[3]);
		case 0b1110: /* 14 */
			return (matrix[0] ^ matrix[1] ^ matrix[2]);
		case 0b1111: /* 15 */
			return (matrix[0] ^ matrix[1] ^ matrix[2] ^ matrix[3]);
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
 * The only way to speed things up here would be statistical analysis of
 * the frequency of the different cases, I'm betting it would not bring any
 * major improvement and the distribution would be about even for all cases
 * @NOTE: ^ is the xor operator
 */
char decode_switch(char c) {
	char matrix[4] = {0b10001111, 0b11000111, 0b10100100, 0b10010010};
	if(c == 0) {
		return 0b0000;
	} else if (c == matrix[3]) {
		return 0b0001;
	} else if (c == matrix[2]) {
		return 0b0010;
	} else if (c == (matrix[2] ^ matrix[3])) {
		return 0b0011;
	} else if (c == matrix[1]) {
		return 0b0100;
	} else if (c == (matrix[1] ^ matrix[3])) {
		return 0b0101;
	} else if (c == (matrix[1] ^ matrix[2])) {
		return 0b0110;
	} else if (c == (matrix[1] ^ matrix[2] ^ matrix[3])) {
		return 0b0111;
	} else if (c == matrix[0]) {
		return 0b1000;
	} else if (c == (matrix[0] ^ matrix[3])) {
		return 0b1001;
	} else if (c == (matrix[0] ^ matrix[2])) {
		return 0b1010;
	} else if (c == (matrix[0] ^ matrix[2] ^ matrix[3])) {
		return 0b1011;
	} else if (c == (matrix[0] ^ matrix[1])) {
		return 0b1100;
	} else if (c == (matrix[0] ^ matrix[1] ^ matrix[3])) {
		return 0b1101;
	} else if (c == (matrix[0] ^ matrix[1] ^ matrix[2])) {
		return 0b1110;
	} else if (c == (matrix[0] ^ matrix[1] ^ matrix[2] ^ matrix[3])) {
		return 0b1111;
	} else {
		/* 
		 * Shouldn't ever be triggered since all
		 * possible combinations of 4bits have been made. 0-15
		 */
		return 0;
	}
}

/**
 * Frequency analysis on Synaspson.mp4/oHennou.mp4:
 *   case  0:  4454354
 *   case 15:  4315503
 *   case  7:  4296136
 *   case 11:  4291245
 *   case 14:  4267579
 *   case  9:  4267520
 *   case 13:  4255420
 *   case 10:  4252673
 *   case  6:  4242862
 *   case  5:  4239345
 *   case  8:  4235849
 *   case  4:  4231940
 *   case 12:  4228388
 *   case  3:  4218822
 *   case  2:  4199661
 *   case  1:  4196904
 * Indeed, the distribution is about even, I expect different
 *  files to have a different distribution.
 *  All cases appear to be in the same order of magnitude. 10⁶
 * Text files, should have a distribution slightly skewed by the
 *  character frequency of the language they were written in.
 *  Typically: `space`, e, t, a, o, i, n, s, r, etc.
 **/
