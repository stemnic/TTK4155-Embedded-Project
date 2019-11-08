/*
 * compact_math.c
 *
 * Created: 08/11/2019 10:06:33
 *  Author: Einar
 */ 
#include "compact_math.h"
/* We ran into some serious progmem issues, even with size optimization.
This file contains a number of simple math operations which save space elsewhere
(pushing and popping to the stack a few extra times is cheaper than repeatedly inlining the code)
This probably makes the program run slower, but that is a sacrifice we have to make */

short accum accum_mult(short accum l, uint8_t r) {
	return l * r;
}
accum long_accum_mult(accum l, int8_t r) {
	return l * r;
}

uint8_t abs_diff(uint8_t lhs, uint8_t rhs) {
	return lhs > rhs ? lhs - rhs : rhs - lhs;
}

int16_t abs_diff_16(int16_t lhs, int16_t rhs) {
	return lhs > rhs ? lhs - rhs : rhs - lhs;
}

uint8_t num_len(uint16_t num) {
	uint8_t cnt = 0;
	do {
		num = num / 10;
		cnt++;
	} while (num > 0);
	return cnt;
}