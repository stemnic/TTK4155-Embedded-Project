/*
 * geometry.h
 *
 * Created: 25/10/2019 10:51:48
 *  Author: Einar
 */ 


#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>


typedef struct rectangle {
	uint8_t ax;
	uint8_t ay;
	uint8_t bx;
	uint8_t by;
	uint8_t cx;
	uint8_t cy;
	uint8_t dx;
	uint8_t dy;
} rectangle;

typedef struct circle {
	uint8_t cx;
	uint8_t cy;
	uint8_t rad;
} circle;

rectangle geo_build_rect(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t angle, uint8_t offset);
circle geo_build_circle(uint8_t cx, uint8_t cy, uint8_t rad);
uint8_t geo_intersect(rectangle *_rect, circle *_circ);





#endif /* GEOMETRY_H_ */