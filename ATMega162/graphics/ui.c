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
		draw_data_at(row * 8, j, 0xFF, 1, OLED_ADDR_INVERT);
	}
}

void ui_list_init(char** options, uint8_t len) {
	for (int i = 0; i < len; i++) {
		draw_string_at(i*8, 0, options[i], FONT8x8, OLED_ADDR_LAYER);
	}
	listLen = len;
	listSel = 0;
	ui_list_select(listSel);
}

void ui_buttons_init(char* str1, char* str2) {
	fill_box(40, 0, 56, 48, OLED_ADDR_OVERWRITE);
	fill_box(40, 72, 56, 128, OLED_ADDR_OVERWRITE);
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