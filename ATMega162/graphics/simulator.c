/*
 * simulator.c
 *
 * Created: 18/10/2019 12:59:28
 *  Author: Einar
 */ 

#include "oled_buffer.h"
#include "simulator.h"
#include <math.h>
#include <stdlib.h>

#define SIM_BALL_RAD 3
#define SIM_ACTOR_BASE_HEIGHT 12
#define SIM_ACTOR_WIDTH 6
#define SIM_ACTOR_SOL_BASE 3
#define SIM_ACTOR_SOL_EXT 3
#define SIM_ACTOR_ROD_WIDTH 2
#define SIM_LEFT_BORDER 0
#define SIM_RIGHT_BORDER 127
#define SIM_TOP_BORDER 0
#define SIM_BOTTOM_BORDER 63
#define SIM_BALL_SPEED_INIT 0.3
#define SIM_BALL_SPEED_ACC 0.05

typedef struct simulatorState {
	uint8_t angle;
	uint8_t last_angle;
	uint8_t triggered;
	uint8_t last_triggered;
	uint8_t pos;
	uint8_t last_pos;
	uint8_t reset_solenoid;
	uint8_t mode;
	uint8_t force;
	uint8_t score;
	uint8_t redraw_score;
	float ball_spd_abs;
} simulatorState;

simulatorState state;

typedef struct ballState {
	float xpos;
	float ypos;
	uint8_t last_xpos; // Only used for drawing
	uint8_t last_ypos;
	uint8_t rad;
	float xvel;
	float yvel;
} ballState;

ballState ball;


void sim_update_actor_angle(uint8_t angle) {
	if (angle == state.last_angle && state.triggered == state.last_triggered && state.last_pos == state.pos && !state.force) return;
	draw_rotated_box(63, state.last_pos, SIM_ACTOR_WIDTH, SIM_ACTOR_BASE_HEIGHT, state.last_angle, 1, 1, 0, OLED_ADDR_DISABLE);
	draw_rotated_box(63, state.last_pos, SIM_ACTOR_WIDTH, SIM_ACTOR_SOL_BASE, state.last_angle, 1, 1, SIM_ACTOR_BASE_HEIGHT + SIM_ACTOR_SOL_BASE + (state.last_triggered ? SIM_ACTOR_SOL_EXT : 0), OLED_ADDR_DISABLE);
	draw_rotated_box(63, state.last_pos, SIM_ACTOR_ROD_WIDTH, SIM_ACTOR_SOL_BASE + (state.last_triggered ? SIM_ACTOR_SOL_EXT : 0), state.last_angle, 1, 1, SIM_ACTOR_BASE_HEIGHT, OLED_ADDR_DISABLE);

	draw_rotated_box(63, state.pos, SIM_ACTOR_WIDTH, SIM_ACTOR_BASE_HEIGHT, angle, 1, 1, 0, OLED_ADDR_LAYER);
	draw_rotated_box(63, state.pos, SIM_ACTOR_WIDTH, SIM_ACTOR_SOL_BASE, angle, 1, 1, SIM_ACTOR_BASE_HEIGHT + SIM_ACTOR_SOL_BASE + (state.triggered ? SIM_ACTOR_SOL_EXT : 0), OLED_ADDR_LAYER);
	draw_rotated_box(63, state.pos, SIM_ACTOR_ROD_WIDTH, SIM_ACTOR_SOL_BASE + (state.triggered ? SIM_ACTOR_SOL_EXT : 0), angle, 1, 1, SIM_ACTOR_BASE_HEIGHT, OLED_ADDR_LAYER);
	state.last_angle = angle;
	state.angle = angle;
	state.last_triggered = state.triggered;
	state.last_pos = state.pos;
	state.force = 0;
}



// Checks if the point (px, py) is within the rectangle (A, B, C, D)
uint8_t point_in_rectangle(int16_t px, int16_t py, int16_t ax, int16_t ay, int16_t bx, int16_t by, int16_t cx, int16_t cy, int16_t dx, int16_t dy) {
	int16_t apab = (px - ax) * (bx - ax) + (py - ay) * (by - ay);
	int16_t abab = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);

	int16_t apad = (px - ax) * (dx - ax) + (py - ay) * (dy - ay);
	int16_t adad = (dx - ax) * (dx - ax) + (dy - ay) * (dy - ay);
	return (apad <= adad) && (apad >= 0) && (apab <= abab) && (apab >= 0);
}

// Check if a circle with center (px, py) and radius rad intersects the line ((x0, y0), (x1, y1))
// Calculates the projection onto the line, then tests if 
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

// Rough collision checking, uses integers so not perfectly accurate, but we don't need it to be.
// If this is too insensitive, we can bump the effective radius
// Note that despite appearances, this probably isn't very expensive, as it is actually O(1), both in memory and time.
uint8_t sim_check_collision() {
	// First get the collision box for the solenoid
	float rads = ((float)state.angle) / 512.0 * 2.0 * 22/7; // Scale to 512, multiply by 2*pi
	float axx = cos(rads);
	float ayy = -sin(rads);
	uint8_t bx = 63 + axx*SIM_ACTOR_WIDTH/2 + ayy*(SIM_ACTOR_BASE_HEIGHT); //A
	uint8_t by = state.pos - ayy*SIM_ACTOR_WIDTH/2 + axx*(SIM_ACTOR_BASE_HEIGHT);
	uint8_t cx = 63 - axx*SIM_ACTOR_WIDTH/2 + ayy*(SIM_ACTOR_BASE_HEIGHT); //D
	uint8_t cy = state.pos + ayy*SIM_ACTOR_WIDTH/2 + axx*(SIM_ACTOR_BASE_HEIGHT);
	uint8_t ax = bx + ayy*(SIM_ACTOR_SOL_EXT+SIM_ACTOR_SOL_BASE)*2; //B
	uint8_t ay = by + axx*(SIM_ACTOR_SOL_EXT+SIM_ACTOR_SOL_BASE)*2;
	uint8_t dx = cx + ayy*(SIM_ACTOR_SOL_EXT+SIM_ACTOR_SOL_BASE)*2; //C
	uint8_t dy = cy + axx*(SIM_ACTOR_SOL_EXT+SIM_ACTOR_SOL_BASE)*2;

	// Check if center is within the rectangle
	uint8_t ballx = (uint8_t)ball.xpos;
	uint8_t bally = (uint8_t)ball.ypos;

	return point_in_rectangle(bally, ballx, ax, ay, bx, by, cx, cy, dx, dy)
		|| intersect_circle(bally, ballx, ax, ay, bx, by, SIM_BALL_RAD)
		|| intersect_circle(bally, ballx, bx, by, cx, cy, SIM_BALL_RAD)
		|| intersect_circle(bally, ballx, cx, cy, dx, dy, SIM_BALL_RAD)
		|| intersect_circle(bally, ballx, dx, dy, ax, ay, SIM_BALL_RAD);
}

void sim_trigger_solenoid() {
	if (!state.triggered) {
		state.triggered = 1;
		sim_update_actor_angle(state.last_angle);
		if (state.mode == SIM_MODE_SIMULATOR) {
			if (sim_check_collision()) {
				if (ball.yvel > 0) {
					state.score++;
					state.redraw_score = 1;
					state.ball_spd_abs += SIM_BALL_SPEED_ACC;
				}
				// Get new speed
				float rads = ((float)state.angle) / 512.0 * 2.0 * 22/7; // Scale to 512, multiply by 2*pi
				float ax = cos(rads);
				float ay = -sin(rads);

				ball.xvel = ax * state.ball_spd_abs;
				ball.yvel = ay * state.ball_spd_abs;
			}
		}
	}
}

void sim_update_ball() {
	ball.xpos += ball.xvel;
	ball.ypos += ball.yvel;
	
	int16_t sxpos = (int16_t)ball.xpos;
	int16_t sypos = (int16_t)ball.ypos;

	if (sxpos + SIM_BALL_RAD > SIM_RIGHT_BORDER) {
		ball.xpos = SIM_RIGHT_BORDER - SIM_BALL_RAD;
		ball.xvel = -1 * ball.xvel;
	}
	if (sxpos - SIM_BALL_RAD < SIM_LEFT_BORDER) {
		ball.xpos = SIM_LEFT_BORDER + SIM_BALL_RAD;
		ball.xvel = -1 * ball.xvel;
	}
	if (sypos + SIM_BALL_RAD > SIM_BOTTOM_BORDER) {
		// TODO Trigger failure in a neat way
		ball.xpos = 63;
		ball.ypos = 3;
		ball.xvel = 0;
		ball.yvel = SIM_BALL_SPEED_INIT;
		state.ball_spd_abs = SIM_BALL_SPEED_INIT;
		draw_large_num(10, 40, state.score, OLED_ADDR_DISABLE);
		state.score = 0;
		state.redraw_score = 1;
	}
	if (sypos - SIM_BALL_RAD < SIM_TOP_BORDER) {
		ball.ypos = SIM_TOP_BORDER + SIM_BALL_RAD;
		ball.yvel = -1 * ball.yvel;
	}

	uint8_t xpos = (uint8_t)ball.xpos;
	uint8_t ypos = (uint8_t)ball.ypos;
	

	if (xpos == ball.last_xpos && ypos == ball.last_ypos) return;
	draw_circle(ball.last_ypos, ball.last_xpos, SIM_BALL_RAD, 1, OLED_ADDR_DISABLE);
	draw_circle(ypos, xpos, SIM_BALL_RAD, 1, OLED_ADDR_LAYER);
	ball.last_xpos = xpos;
	ball.last_ypos = ypos;
	if (ball.ypos > 55) state.force = 1;
	if (ball.xpos > 40 && ball.xpos < 80 && ball.ypos > 10 && ball.ypos < 30) state.redraw_score = 1;
}

void sim_draw_score() {
	if (!state.redraw_score) return;
	if (state.score > 0) {
		draw_large_num(10, 40, state.score - 1, OLED_ADDR_DISABLE);
	}
	draw_large_num(10, 40, state.score, OLED_ADDR_LAYER);
}

void sim_tick() {
	if (state.triggered && state.reset_solenoid++ == 5) {
		state.triggered = 0;
		state.reset_solenoid = 0;
		sim_update_actor_angle(state.last_angle);
	}
	if (state.mode == SIM_MODE_SIMULATOR) {
		sim_update_ball();
		sim_draw_score();
	}
}

void sim_update_pos(uint8_t newpos) {
	if (newpos != state.pos) {
		state.pos = newpos;
		sim_update_actor_angle(state.last_angle);
	}
}

void sim_init(uint8_t mode) {
	state.last_angle = 0;
	state.pos = 63;
	state.last_pos = 63;
	state.triggered = 0;
	state.last_triggered = 0;
	state.reset_solenoid = 0;
	state.mode = mode;
	state.score = 0;
	state.redraw_score = 1;
	sim_update_actor_angle(127);

	if (mode == SIM_MODE_SIMULATOR) {
		ball.xpos = 63;
		ball.ypos = 31;
		ball.last_xpos = 63;
		ball.last_ypos = 31;
		ball.rad = 3;
		ball.xvel = 0;
		ball.yvel = SIM_BALL_SPEED_INIT;
		state.ball_spd_abs = SIM_BALL_SPEED_INIT;
	}
}



