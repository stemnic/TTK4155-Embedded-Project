/*
 * simulator.c
 *
 * Created: 18/10/2019 12:59:28
 *  Author: Einar
 */ 

#include "oled_buffer.h"
#include "simulator.h"
#include "geometry.h"
#include <stdlib.h>
#include <stdfix-avrlibc.h>
#include "../drivers/eeprom.h"
#include "../compact_math.h"

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
#define SIM_BALL_SPEED_INIT 0.3K
#define SIM_BALL_SPEED_ACC 0.05K

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
	uint16_t score;
	uint8_t redraw_score;
	unsigned short accum ball_spd_abs;
} simulatorState;

simulatorState state;

typedef struct ballState {
	accum xpos;
	accum ypos;
	uint8_t last_xpos; // Only used for drawing
	uint8_t last_ypos;
	uint8_t rad;
	short accum xvel;
	short accum yvel;
} ballState;

ballState ball;

/* Update the actor state with a new angle, also re-draw the actor, clearing the old actor depending on the previous state */
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

// Rough collision checking, uses integers so not perfectly accurate, but we don't need it to be.
// If this is too insensitive, we can bump the effective radius
// Note that despite appearances, this probably isn't very expensive, as it is actually O(1), both in memory and time.
uint8_t sim_check_collision() {
	// Check if center is within the rectangle
	uint8_t ballx = (uint8_t)ball.xpos;
	uint8_t bally = (uint8_t)ball.ypos;

	rectangle rect = geo_build_rect(63, state.pos, SIM_ACTOR_WIDTH, SIM_ACTOR_SOL_EXT + SIM_ACTOR_SOL_BASE, state.angle, SIM_ACTOR_BASE_HEIGHT+SIM_ACTOR_SOL_EXT);
	circle circ = geo_build_circle(bally, ballx, ball.rad);

	return geo_intersect(&rect, &circ);
}

/* Trigger the simulated solenoid, causes the actor to be re-rendered, and checks for collision in simulator mode */
void sim_trigger_solenoid() {
	if (!state.triggered) {
		state.triggered = 1;
		sim_update_actor_angle(state.last_angle);
		if (state.mode == SIM_MODE_SIMULATOR) {
			if (sim_check_collision()) {
				if (ball.yvel > 0) {
					sim_set_score(state.score + 1);
					state.ball_spd_abs += SIM_BALL_SPEED_ACC;
				}
				// Get new speed
				short accum ax = geo_cos(state.angle);
				short accum ay = -geo_sin(state.angle);

				ball.xvel = ax * state.ball_spd_abs;
				ball.yvel = ay * state.ball_spd_abs;
			}
		}
	}
}

/* Tick the ball, updating position and calculating whether to bounce against a wall */
void sim_update_ball(uint8_t frames) {
	ball.xpos += long_accum_mult(ball.xvel, frames);
	ball.ypos += long_accum_mult(ball.yvel, frames);
	
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
		ball.xpos = 63;
		ball.ypos = 3;
		ball.xvel = 0;
		ball.yvel = SIM_BALL_SPEED_INIT;
		state.ball_spd_abs = SIM_BALL_SPEED_INIT;
		update_scores(SIM_MODE_SIMULATOR, state.score);
		sim_set_score(0);
		state.redraw_score = 1;
		state.force = 1;
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
	if (ball.ypos > 40) state.force = 1;
	if (ball.xpos > 40 && ball.xpos < 80 && ball.ypos > 10 && ball.ypos < 30) state.redraw_score = 1;
}

/* Simulator tick, count the solenoid reset, update the ball, re-draw the score, draw the angle if state.force is set */
void sim_tick(uint8_t frames) {
	#ifndef DEBUG
	if (state.triggered && (state.reset_solenoid += frames) >= 5) {
		state.triggered = 0;
		state.reset_solenoid = 0;
		sim_update_actor_angle(state.last_angle);
	}
	if (state.mode == SIM_MODE_SIMULATOR) {
		sim_update_ball(frames);
		if (state.redraw_score) {
			sim_set_score(state.score);
		}
	}
	if (state.force) {
		sim_update_actor_angle(state.angle);
	}
	#endif
}

/* Set a new position for the actor */
void sim_update_pos(uint8_t newpos) {
	if (newpos != state.pos) {
		state.pos = newpos;
		sim_update_actor_angle(state.last_angle);
	}
}

void sim_set_score(uint16_t score) {
	if (score != state.score) {
		draw_num(10, 40, state.score, 1, OLED_ADDR_DISABLE);
	}
	draw_num(10, 40, score, 1, OLED_ADDR_LAYER);
	state.score = score;
	state.redraw_score = 1;
}

/* Init the simulator in given mode, resets the state and the ball */
void sim_init(uint8_t mode) {
	state.last_angle = 0;
	state.pos = 63;
	state.last_pos = 63;
	state.triggered = 0;
	state.last_triggered = 0;
	state.reset_solenoid = 0;
	state.mode = mode;
	state.redraw_score = 1;
	#ifndef DEBUG
	sim_update_actor_angle(127);
	sim_set_score(0);
	#endif
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



