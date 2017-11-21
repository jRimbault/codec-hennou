/**
 * @Author: jRimbault nAmari
 * @Date:   2017-01-09 14:16:04
 * @Last Modified by:   jRimbault
 * @Description:
 */

#ifndef _SWITCHES_H
#define _SWITCHES_H

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
char encode_switch(char, char*);

/*
 * Receives an encoded byte and returns the original byte
 */
char decode_switch(char, char*);

#endif // _SWITCHES_H
