/*
 * geometry.h
 *
 * Created: 25/10/2019 10:51:48
 *  Author: Einar
 */ 


#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>
#include <stdfix.h>


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

/* Create a rotated rectangle from the center position of one edge (x0, y0),
the width (length of that edge), height, (length of perpendicular edges), angle,
where 0 means that the rectangle is horizontal, extending in positive x,
and an offset, which is shifted in the same direction as height */
rectangle geo_build_rect(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t angle, uint8_t offset);
/* Construct circle from given center point (cx, cy) and radius rad */
circle geo_build_circle(uint8_t cx, uint8_t cy, uint8_t rad);
/* Return true if the given rectangle and circle intersect */
uint8_t geo_intersect(rectangle *_rect, circle *_circ);

/* Return compact low-res cosine value for the first two quadrants, where 0 is 0 and 255 is pi */
short accum geo_cos(uint8_t val);
/* Return compact low-res sine value for the first two quadrants, where 0 is 0 and 255 is pi */
short accum geo_sin(uint8_t val);




#endif /* GEOMETRY_H_ */