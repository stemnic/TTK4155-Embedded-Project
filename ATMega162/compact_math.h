/*
 * compact_math.h
 *
 * Created: 08/11/2019 10:07:15
 *  Author: Einar
 */ 


#ifndef COMPACT_MATH_H_
#define COMPACT_MATH_H_

#include <stdint.h>
#include <stdfix.h>

short accum accum_mult(short accum l, uint8_t r);
accum long_accum_mult(accum l, int8_t r);

uint8_t abs_diff(uint8_t lhs, uint8_t rhs);
int16_t abs_diff_16(int16_t lhs, int16_t rhs);
uint8_t num_len(uint16_t num);





#endif /* COMPACT_MATH_H_ */