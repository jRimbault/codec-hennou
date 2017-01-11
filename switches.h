/**
 * @Author: jRimbault
 * @Date:   2017-01-09 14:16:04
 * @Last Modified by:   jRimbault
 * @Last Modified time: 2017-01-11 12:44:58
 * @Description: 
 */

#include "switches.c"

/*
 * Left bitshift
 */
char quartet_1(char);

/*
 * Right bitshift
 */
char quartet_2(char);

/*
 * Receives a half byte (4bits) and returns the corresponding encoded byte
 */
char encode_character_switch(char);

/*
 * Receives an encoded byte and returns the original byte
 */
char decode_character_switch(char);
