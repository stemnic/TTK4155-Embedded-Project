/*
 * ui.c
 *
 * Created: 20/09/2019 12:33:10
 *  Author: Einar
 */ 

#include "oled_buffer.h"
#include "ui.h"
#include "../drivers/io.h"
#include "../drivers/oled.h"

uint8_t listSel = 0;
uint8_t listLen = 0;

void ui_list_select(int row) {
	for (int j = 0; j < 12; j++) {
		invert_block_at(row, j);
	}
}

void ui_list_init(char** options, uint8_t len) {
	for (int i = 0; i < len; i++) {
		draw_string_at(i, 0, options[i], FONT8x8, OLED_ADDR_LAYER);
	}
	listLen = len;
	listSel = 0;
	ui_list_select(listSel);
}

void ui_buttons_init(char* str1, char* str2) {
	fill_box(5, 0, 7, 6, OLED_ADDR_OVERWRITE);
	fill_box(5, 9, 7, 16, OLED_ADDR_OVERWRITE);
	draw_string_at(6, 6, str1, FONT8x8, OLED_ADDR_INVERT);
	draw_string_at(6, 80, str2, FONT8x8, OLED_ADDR_INVERT);
}

void ui_button_trigger(uint8_t button, uint8_t on) {
	switch (button) {
	case BUTTON_1:
		draw_box(41, 1, 62, 54, 2, on ? OLED_ADDR_INVERT : OLED_ADDR_LAYER);
		break;
	case BUTTON_2:
		draw_box(41, 73, 62, 126, 2, on ? OLED_ADDR_INVERT : OLED_ADDR_LAYER);
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