/*
 * simulator.c
 *
 * Created: 18/10/2019 12:59:28
 *  Author: Einar
 */ 

#include "oled_buffer.h"

uint8_t lastangle;
uint8_t triggered = 0;
uint8_t lasttriggered = 0;
uint8_t pos = 63;
uint8_t lastpos = 63;

void sim_update_actor_angle(uint8_t angle) {
	if (angle == lastangle && triggered == lasttriggered && lastpos == pos) return;
	draw_rotated_box(63, lastpos, 6, 12, lastangle, 1, 1, 0, OLED_ADDR_DISABLE);
	draw_rotated_box(63, lastpos, 6, 3, lastangle, 1, 1, lasttriggered ? 18 : 15, OLED_ADDR_DISABLE);
	draw_rotated_box(63, lastpos, 2, lasttriggered ? 6 : 3, lastangle, 1, 1, 12, OLED_ADDR_DISABLE);

	draw_rotated_box(63, pos, 6, 12, angle, 1, 1, 0, OLED_ADDR_LAYER);
	draw_rotated_box(63, pos, 6, 3, angle, 1, 1, triggered ? 18 : 15, OLED_ADDR_LAYER);
	draw_rotated_box(63, pos, 2, triggered ? 6 : 3, angle, 1, 1, 12, OLED_ADDR_LAYER);
	lastangle = angle;
	lasttriggered = triggered;
	lastpos = pos;
}

void sim_trigger_solenoid() {
	if (!triggered) {
		triggered = 1;
		sim_update_actor_angle(lastangle);
	}
}

uint8_t reset_solenoid = 0;

void sim_tick() {
	if (triggered && reset_solenoid++ == 5) {
		triggered = 0;
		reset_solenoid = 0;
		sim_update_actor_angle(lastangle);
	}
}

void sim_update_pos(uint8_t newpos) {
	if (newpos != pos) {
		pos = newpos;
		sim_update_actor_angle(lastangle);
	}
}

