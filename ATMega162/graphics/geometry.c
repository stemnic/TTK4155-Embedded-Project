/*
 * geometry.c
 *
 * Created: 25/10/2019 10:51:38
 *  Author: Einar
 */ 

#include <math.h>
#include "geometry.h"
#include "constants.h"
#include "../compact_math.h"

/* Create a rotated rectangle from the center position of one edge (x0, y0),
the width (length of that edge), height, (length of perpendicular edges), angle,
where 0 means that the rectangle is horizontal, extending in positive x,
and an offset, which is shifted in the same direction as height */
rectangle geo_build_rect(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t angle, uint8_t offset) {
	short accum axx = geo_cos(angle);
	short accum ayy = -geo_sin(angle);
	rectangle rect;
	width = width / 2;
	rect.bx = x0 + accum_mult(axx, width) + accum_mult(ayy, offset);
	rect.by = y0 - accum_mult(ayy, width) + accum_mult(axx, offset);
	rect.cx = x0 - accum_mult(axx, width) + accum_mult(ayy, offset); //D
	rect.cy = y0 + accum_mult(ayy, width) + accum_mult(axx, offset);
	rect.ax = rect.bx + accum_mult(ayy, height); //B
	rect.ay = rect.by + accum_mult(axx, height);
	rect.dx = rect.cx + accum_mult(ayy, height); //C
	rect.dy = rect.cy + accum_mult(axx, height);
	return rect;
}

/* Returns 1 if the given point (px, py) is within the given rectangle
Uses formula AP*AD <= AD*AD && AP*AD >= 0 && AP*AB <= AB*AB && AP*AB >= 0
In other words, the projection onto two perpendicular edges of the rectangle
is shorter than the edge itself. */
uint8_t point_in_rectangle(int16_t px, int16_t py, rectangle *_r) {
	rectangle r = *_r;
	int16_t apab = (px - r.ax) * (r.bx - r.ax) + (py - r.ay) * (r.by - r.ay);
	int16_t abab = (r.bx - r.ax) * (r.bx - r.ax) + (r.by - r.ay) * (r.by - r.ay);

	int16_t apad = (px - r.ax) * (r.dx - r.ax) + (py - r.ay) * (r.dy - r.ay);
	int16_t adad = (r.dx - r.ax) * (r.dx - r.ax) + (r.dy - r.ay) * (r.dy - r.ay);
	return (apad <= adad) && (apad >= 0) && (apab <= abab) && (apab >= 0);
}

/* Check if a circle with center (px, py) and radius rad intersects the line ((x0, y0), (x1, y1))
Calculates the projection onto the line, then tests if the projection is within the end points,
also tests if the point is within rad of each of the end points.
Slightly confusing as it only uses squared arithemtic, to avoid taking the square root*/
uint8_t intersect_circle(uint8_t px, uint8_t py, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t rad) {
	// Get the length of the projection onto the line
	int16_t llen_sq = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
	int16_t len_sq = (((y1 - y0)*px - (x1 - x0)*py + x1 * y0 - y1 * x0) * ((y1 - y0)*px - (x1 - x0)*py + x1 * y0 - y1 * x0)) / llen_sq;
	if (len_sq > rad*rad) return 0; // Too far away for collision to be possible
	
	// Get the squared distance to the two points
	int16_t dist0 = (px - x0) * (px - x0) + (py - y0) * (py - y0);
	int16_t dist1 = (px - x1) * (px - x1) + (py - y1) * (py - y1);

	if (dist0 <= rad * rad || dist1 <= rad * rad) return 1; // Projection is close enough to one of the corners
	if ((dist0 <= llen_sq + len_sq) && (dist1 <= llen_sq + len_sq)) return 1; // Projection is between the two outer points
	return 0;
}
/* Construct a circle struct from center point (cx, cy) and rad */
circle geo_build_circle(uint8_t cx, uint8_t cy, uint8_t rad) {
	circle circ;
	circ.cx = cx;
	circ.cy = cy;
	circ.rad = rad;
	return circ;
}
/* Check if a circle intersects with a rectangle, return 1 if it does */
uint8_t geo_intersect(rectangle *_rect, circle *_circ) {
	rectangle rect = *_rect;
	circle circ = *_circ;
	return point_in_rectangle(circ.cx, circ.cy, &rect)
		|| intersect_circle(circ.cx, circ.cy, rect.ax, rect.ay, rect.bx, rect.by, circ.rad)
		|| intersect_circle(circ.cx, circ.cy, rect.bx, rect.by, rect.cx, rect.cy, circ.rad)
		|| intersect_circle(circ.cx, circ.cy, rect.cx, rect.cy, rect.dx, rect.dy, circ.rad)
		|| intersect_circle(circ.cx, circ.cy, rect.dx, rect.dy, rect.ax, rect.ay, circ.rad);
}
/* Return compact low-res cosine value for the first two quadrants, where 0 is 0 and 255 is pi */
short accum geo_cos(uint8_t val) {
	return pregen_cos[val / 8];
}
/* Return compact low-res sine value for the first two quadrants, where 0 is 0 and 255 is pi */
short accum geo_sin(uint8_t val) {
	return pregen_sin[val / 8];
}