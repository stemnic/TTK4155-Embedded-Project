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
#include <stdlib.h>

uint8_t listSel = 0;
uint8_t listLen = 0;
int16_t ballx = -50;
int8_t dirball = 1;
uint8_t ballcnt = 0;

uint8_t *lengths;

uint8_t last_ltrigg = 0;
uint8_t last_rtrigg = 0;
uint8_t ltrigg = 0;
uint8_t rtrigg = 0;

void ui_list_select(int row) {
	for (int j = 0; j < lengths[row]*8 + 4; j++) {
		draw_data_at(row * 8 + 32, 63 - lengths[row]*4 - 2 + j, 0xFF, 1, OLED_ADDR_INVERT);
	}
}

void ui_list_init(char** options, uint8_t len) {
	lengths = malloc(len);
	for (uint8_t i = 0; i < len; i++) {
		uint8_t len = 0;
		while (options[i][len] != '\0') len++;
		lengths[i] = len;
		draw_string_at(i*8 + 32, 63-len*4, options[i], FONT8x8, OLED_ADDR_LAYER);
	}

	listLen = len;
	listSel = 0;
	ui_list_select(listSel);
}

void ui_buttons_init(char* str1, char* str2) {
	fill_box(40, 0, 63, 55, OLED_ADDR_OVERWRITE);
	fill_box(40, 72, 63, 127, OLED_ADDR_OVERWRITE);
	draw_string_at(48, 6, str1, FONT8x8, OLED_ADDR_DISABLE);
	draw_string_at(48, 80, str2, FONT8x8, OLED_ADDR_DISABLE);
}

void ui_button_trigger(uint8_t button, uint8_t on) {
	switch (button) {
	case BUTTON_1:
		draw_box(41, 1, 62, 54, 2, on ? OLED_ADDR_DISABLE : OLED_ADDR_LAYER);
		break;
	case BUTTON_2:
		draw_box(41, 73, 62, 126, 2, on ? OLED_ADDR_DISABLE : OLED_ADDR_LAYER);
		break;
	}
}

void ui_list_update(int8_t joystick_trigger) {
	if (joystick_trigger != 0 && (joystick_trigger != 1 || listSel > 0) && (joystick_trigger != -1 || listSel < listLen - 1)) {
		ui_list_select(listSel);
		ui_list_select(listSel -= joystick_trigger);
	}
}

uint8_t get_list_pos() {
	return listSel;
}

void ui_draw_big_number(uint8_t num) {
	draw_large_num(16, 90, num, OLED_ADDR_LAYER);
}

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

void ui_menu_tick() {
	ballcnt = 0;
	draw_circle(5+(ballx*ballx)/125, 63+ballx, 3, 1, OLED_ADDR_DISABLE);
	ballx += dirball*(ballx < -30 || ballx > 30 ? 3 : 2);
	draw_circle(5+(ballx*ballx)/125, 63+ballx, 3, 1, OLED_ADDR_LAYER);
	if (ballx >= 50) dirball = -1;
	if (ballx <= -50) dirball = 1;
	if (ballx < -30) ui_draw_rackets(1, 0);
	if (ballx > 30) ui_draw_rackets(0, 1);
}

void ui_menu_init(char **list, uint8_t len) {
	ui_list_init(list, 2);
	ui_draw_rackets(1, 1);
}



void ui_menu_update(controllerInput *_input) {
	controllerInput input = *(_input);
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

void ui_simulator_update(controllerInput *_input, uint8_t pos) {
	controllerInput input = *(_input);
	sim_update_actor_angle(255 - input.slider_two_value);
	if (input.joystick_button_changed && input.joystick_button) {
		sim_trigger_solenoid();
	}
	sim_update_pos(pos / 2.8 + 20);
}