/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-09 14:16:04
 * @Last Modified by:   erelde
 * @Description:
 */

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
 * @param c is half a byte, so always between 0 and 16
 * @Note: ^ is the xor operator
 */
char encode_switch(char c, char* matrix) {
	return matrix[c];
}

/*
 * Receives an encoded byte and returns the original byte
 * The only way to speed things up here would be statistical analysis of
 * the frequency of the different cases, I'm betting it would not bring any
 * major improvement and the distribution would be about even for all cases
 * @Note: ^ is the xor operator
 */
char decode_switch(char c, char* matrix) {
	for (int i = 0; i < 16; i += 1) {
		if (c == matrix[i]) return i;
	}
	return 0;
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
