/*
 * ui.c
 *
 * Created: 20/09/2019 12:33:10
 *  Author: Einar
 */ 

#include "oled_buffer.h"
#include "ui.h"
#include "simulator.h"
#include "../drivers/io.h"
#include "../drivers/oled.h"
#include "../drivers/eeprom.h"
#include <stdlib.h>

uint8_t listSel = 0;
uint8_t listLen = 0;
int16_t ballx = -50;
int8_t dirball = 1;
uint8_t ballcnt = 0;

uint8_t *lengths;

uint8_t rtrigg = 0;
uint8_t ltrigg = 0;
uint8_t last_rtrigg = 0;
uint8_t last_ltrigg = 0;

uint8_t wipe_state = 0;

/* Select the element of the main-menu list at given index
Simply inverts the line, which gives a clear impression of index */
void ui_list_select(uint8_t row) {
	for (int j = 0; j < lengths[row] * 8 + 4; j++) {
		draw_data_at(row * 8 + 32, 63 - lengths[row] * 4 - 2 + j, 0xFF, 1, OLED_ADDR_INVERT);
	}
}

/* Init the ui-list with a given list of strings, with length len */
void ui_list_init(char **options, uint8_t len) {
	lengths = malloc(len);
	for (uint8_t i = 0; i < len; i++) {
		uint8_t len = 0;
		while (options[i][len] != '\0') len++;
		lengths[i] = len;
		draw_string_at(i * 8 + 32, 63-len * 4, options[i], FONT8x8, OLED_ADDR_LAYER);
	}

	listLen = len;
	listSel = 0;
	ui_list_select(listSel);
}

void ui_scores_init(uint16_t *run_scores, uint16_t *sim_scores) {
	wipe_state = 0;
	draw_string_at(0, 8, "Live", FONT8x8, OLED_ADDR_LAYER);
	for (uint8_t i = 0; i < 5; i++) {
		draw_num(i * 8 + 8, 0, run_scores[i], 0, OLED_ADDR_LAYER);
	}
	draw_string_at(0, 71, "Sim", FONT8x8, OLED_ADDR_LAYER);
	for (uint8_t i = 0; i < 5; i++) {
		draw_num(i * 8 + 8, 63, sim_scores[i], 0, OLED_ADDR_LAYER);
	}
	fill_box(52, 72, 63, 127, OLED_ADDR_LAYER);
	draw_string_at(54, 80, "Wipe", FONT8x8, OLED_ADDR_DISABLE);
}

void ui_scores_button() {
	if (wipe_state == 0) {
		fill_box(52, 72, 63, 127, OLED_ADDR_LAYER);
		draw_string_at(54, 80, "Sure?", FONT8x8, OLED_ADDR_DISABLE);
		wipe_state++;
	} else {
		wipe_state = 0;
		uint16_t run_scores[5] = {0, 0, 0, 0, 0};
		uint16_t sim_scores[5] = {0, 0, 0, 0, 0};
		wipe_scores();
		wipe_buffer();
		ui_scores_init(run_scores, sim_scores);
	}
}

/* Update the list with the given value of joystick_trigger, 1 to increment selection, -1 to decrement */
void ui_list_update(int8_t joystick_trigger) {
	if (joystick_trigger != 0 && (joystick_trigger != 1 || listSel > 0) && (joystick_trigger != -1 || listSel < listLen - 1)) {
		ui_list_select(listSel);
		ui_list_select(listSel -= joystick_trigger);
	}
}

/* Return the current selected list index */
uint8_t get_list_pos() {
	return listSel;
}

/* Draw a big number in the UI, for debug */
void ui_draw_big_number(uint8_t num) {
	draw_num(16, 90, num, 1, OLED_ADDR_LAYER);
}

/* Draw a big racket to the left and right of the screen, shifting them based on button presses */
void ui_draw_rackets(uint8_t left, uint8_t right) {
	if (last_ltrigg != ltrigg && left) {
		draw_image_at(last_ltrigg ? 4 : 0, 0, RACKET, 0, 32, 64, OLED_ADDR_DISABLE);
		last_ltrigg = ltrigg;
	}
	if (last_rtrigg != rtrigg && right) {
		draw_image_at(last_rtrigg ? 91 : 95, 0, RACKET, 1, 32, 64, OLED_ADDR_DISABLE);
		last_rtrigg = rtrigg;
	}
	if (left) {
		draw_image_at(ltrigg ? 4 : 0, 0, RACKET, 0, 32, 64, OLED_ADDR_LAYER);
	}
	if (right) {
		draw_image_at(rtrigg ? 91 : 95, 0, RACKET, 1, 32, 64, OLED_ADDR_LAYER);
	}
}

/* Tick for the UI-menu, updates the ball graphic
The ball follows a simple parabolic curve */
void ui_menu_tick(uint8_t frames) {
	#ifndef DEBUG
	ballcnt = 0;
	draw_circle(5 + (ballx * ballx) / 125, 63 + ballx, 3, 1, OLED_ADDR_DISABLE);
	ballx += dirball * frames * 2;
	draw_circle(5 + (ballx * ballx) / 125, 63 + ballx, 3, 1, OLED_ADDR_LAYER);
	if (ballx >= 50) dirball = -1;
	if (ballx <= -50) dirball = 1;
	if (ballx < -30) ui_draw_rackets(1, 0);
	if (ballx > 30) ui_draw_rackets(0, 1);
	#endif
}

/* Initialize the menu with a given list of menu entries of length len */
void ui_menu_init(char **list, uint8_t len) {
	ui_list_init(list, len);
	ui_draw_rackets(1, 1);
}

/* Update menu graphics with controller input object */
void ui_menu_update(controller_input_t *_input) {
	controller_input_t input = *(_input);
	ui_list_update(input.joystick_trigger);
	uint8_t lch = 0;
	uint8_t rch = 0;
	if (input.button_one_changed) {
		ltrigg = input.button_one_value;
		lch = 1;
	}
	if (input.button_two_changed) {
		rtrigg = input.button_two_value;
		rch = 1;
	}
	ui_draw_rackets(lch, rch);
}

/* Tick the simulator with given input and actor position */
void ui_simulator_update(controller_input_t *_input, uint8_t pos) {
	#ifndef DEBUG
	controller_input_t input = *(_input);
	sim_update_actor_angle(255 - input.slider_two_value);
	if (input.joystick_button_changed && input.joystick_button) {
		sim_trigger_solenoid();
	}
	sim_update_pos(pos / 2.8 + 20);
	#endif
}