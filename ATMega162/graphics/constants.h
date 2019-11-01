/*
 * constants.h
 *
 * Created: 01/11/2019 08:38:43
 *  Author: Einar
 */ 


#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <avr/pgmspace.h>
#include <stdfix.h>

/* Buffered compact, low res sine values */
const short accum pregen_sin[32] = {
	0K, 0.1K, 0.2K, 0.29K, 0.38K, 0.47K, 0.56K, 0.63K, 0.71K, 0.77K, 0.83K, 0.88K, 0.92K,
	0.96K, 0.98K, 1K, 1K, 1K, 0.98K, 0.96K, 0.92K, 0.88K, 0.83K, 0.77K, 0.71K, 0.63K, 0.56K, 0.47K, 0.38K, 0.29K, 0.2K, 0.1K
};

/* Buffered compact, low res cosine values */
const short accum pregen_cos[32] = {
	1K, 1K, 0.98K, 0.96K, 0.92K, 0.88K, 0.83K, 0.77K, 0.71K, 0.63K, 0.56K, 0.47K, 0.38K, 0.29K,
	0.2K, 0.1K, 0K, -0.1K, -0.2K, -0.29K, -0.38K, -0.47K, -0.56K, -0.63K, -0.71K, -0.77K, -0.83K, -0.88K, -0.92K, -0.96K, -0.98K, -1K
};



#endif /* CONSTANTS_H_ */