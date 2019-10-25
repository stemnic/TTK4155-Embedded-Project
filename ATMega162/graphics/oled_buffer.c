/*
 * oled_buffer.c
 *
 * Created: 20/09/2019 09:24:39
 *  Author: Einar
 */ 
#include <avr/io.h>
#include "../drivers/oled.h"
#include "oled_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
volatile char* ext_ram = (char *) 0x1800;

int changed = 1;

void wipe_buffer() {
	changed = 1;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 128; j++) {
			ext_ram[((i << 7) + j) << 1] = 0x00;
		}
	}
}

void flush_buffer() {
	if (!changed) return;
	oled_command(PAGE_LOW_COL_START);
	oled_command(PAGE_HIGH_COL_START);
	for (uint8_t i = 0; i < 8; i++) {
		oled_command(PAGE_START_ADDR_PAGE_BASE + i);
		for (uint8_t j = 0; j < 128; j++) {
			uint8_t val = ext_ram[((i << 7) + j) << 1];
			oled_write_byte(val);
		}
	}
}


void draw_data_at(uint8_t row, uint8_t col, uint8_t data, uint8_t len, uint8_t addresingMode) {
	if (row > 63 || col > 127) {
		return;
	}
	changed = 1;
	uint16_t addr1 = ((((row / 8) << 7) + col) << 1);
	uint16_t addr2 = ((((row / 8) + 1) << 7) + col) << 1;
	uint8_t offset = row % 8;
	uint8_t mask = 0xFF << offset;
	switch(addresingMode) {
	case OLED_ADDR_OVERWRITE:
		// Clear bits to be modified, then set with or
		ext_ram[addr1] = (ext_ram[addr1] & ~mask) | (data << offset);
		if (offset && len > (8 - offset) && row < 63) ext_ram[addr2] = (ext_ram[addr2] & mask) | (data >> (8 - offset));
		break;
	case OLED_ADDR_LAYER:
		ext_ram[addr1] |= (data << offset);
		if (offset && len > (8 - offset) && row < 63) ext_ram[addr2] |= (data >> (8 - offset));
		break;
	case OLED_ADDR_DISABLE:
		ext_ram[addr1] &= ~(data << offset);
		if (offset && len > (8 - offset) && row < 63) ext_ram[addr2] &= ~(data >> (8 - offset));
		break;
	case OLED_ADDR_INVERT:
		ext_ram[addr1] ^= (data << offset);
		if (offset && len > (8 - offset) && row < 63) ext_ram[addr2] ^= (data >> (8 - offset));
	}
}

void draw_char_at(uint8_t row, uint8_t col, char towrite, uint8_t fontSize, uint8_t addressingMode) {
	for (int i=col; i < col + fontSize; i++) {
		draw_data_at(row, i, get_font_byte(towrite, i - col, fontSize), 8, addressingMode);
	}
}

void draw_string_at(uint8_t row, uint8_t col, char* str, uint8_t fontSize, uint8_t addressingMode) {
	uint8_t i = 0;
	for (i = 0; str[i] != '\0'; i++) {
		draw_char_at(row, col + i*fontSize, str[i], fontSize, addressingMode);
	}
}

void draw_point_at(uint8_t row, uint8_t col, uint8_t addressingMode) {
	draw_data_at(row, col, 1, 1, addressingMode);
}

void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode) {
	int16_t dx = abs(((int16_t)x1) - ((int16_t)x0));
	int16_t dy = -abs(((int16_t)y1) - ((int16_t)y0));
	int16_t err = dx+dy;
	int16_t sx = x0 < x1 ? 1 : -1;
	int16_t sy = y0 < y1 ? 1 : -1;
	while (1) {
		draw_point_at(x0, y0, addressingMode);
		if (x0 == x1 && y0 == y1) break;
		int16_t e2 = err*2;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void draw_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width, uint8_t addressingMode) {
	int8_t sx = x0 < x1 ? 1 : -1;
	int8_t sy = y0 < y1 ? 1 : -1;
	while (width > 0) {
		draw_line(x0, y0, x0, y1, addressingMode);
		draw_line(x0, y0, x1, y0, addressingMode);
		draw_line(x0, y1, x1, y1, addressingMode);
		draw_line(x1, y0, x1, y1, addressingMode);
		width -= 1;
		x0 += sx;
		y0 += sy;
		x1 -= sx;
		y1 -= sy;
	}
}

void fill_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode) {
	int8_t sx = x0 < x1 ? 8 : -8;
	int8_t sy = y0 < y1 ? 1 : -1;
	uint8_t dx = abs(x1 - x0);
	uint8_t x = x0;
	while (dx > 0) {
		uint8_t len = dx < 8 ? dx : 8;
		for (int y = y0; y <= y1; y += sy) {
			draw_data_at(x, y, 0xFF << (8 - len - 1), len, addressingMode);
		}
		dx -= len;
		x += sx;
	}
}

void draw_rotated_box(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, int16_t angle, uint8_t ends, uint8_t sides, uint8_t shift, uint8_t addressingMode) {
	float rads = ((float)angle) / 512.0 * 2.0 * 22/7;
	float ax = cos(rads);
	float ay = -sin(rads);
	uint8_t x00 = x0 + ax*width/2 + ay*shift;
	uint8_t y00 = y0 - ay*width/2 + ax*shift;
	uint8_t x01 = x0 - ax*width/2 + ay*shift;
	uint8_t y01 = y0 + ay*width/2 + ax*shift;
	uint8_t x10 = x00 + ay*height;
	uint8_t y10 = y00 + ax*height;

	uint8_t x11 = x01 + ay*height;
	uint8_t y11 = y01 + ax*height;
	if (sides) {
		draw_line(x00, y00, x01, y01, addressingMode);
		draw_line(x11, y11, x10, y10, addressingMode);
	}
	if (ends) {
		draw_line(x01, y01, x11, y11, addressingMode);
		draw_line(x10, y10, x00, y00, addressingMode);
	}
}

void draw_large_num (uint8_t row, uint8_t col, uint8_t num, uint8_t addressingMode) {
	uint8_t cnt = 0;
	uint8_t len = num == 0 ? 1 : log10(num) + 1;
	do {
		uint8_t digit = num % 10;
		for (int i = 0; i < 9; i++) {
			uint16_t val = get_font_dword(digit, i, NUMBERS9x16);
			draw_data_at(row, (col + i) + 9 * (len - cnt), val & 0xFF, 8, addressingMode);
			draw_data_at(row + 8, (col + i) + 9 * (len - cnt), (val & 0xFF00) >> 8, 8, addressingMode);
		}
		num = num / 10;
		cnt++;
	} while (num > 0);
}

void draw_circle(uint8_t x0, uint8_t y0, uint8_t rad, uint8_t fill, uint8_t addressingMode) {
	int16_t x = rad, y = 0;
	int16_t p = 1 - rad;

	if (fill && rad > 0) {
		draw_line(x + x0, y + y0, -x + x0, y + y0, addressingMode);
		draw_line(y + x0, x + y0, y + x0, -x + y0, addressingMode);
	}
	else {
		draw_point_at(x + x0, y + y0, addressingMode);
		if (rad > 0) {
			draw_point_at(-x + x0, y + y0, addressingMode);
			draw_point_at(y + x0, -x + y0, addressingMode);
			draw_point_at(y + x0, x + y0, addressingMode);
		}
	} 


	while (x > y) {
		y++;
		if (p <= 0) {
			p += 2 * y + 1;
		} else {
			x--;
			p += 2 * y - 2 * x + 1;
		}
		if (x < y) break;

		if (fill) {
			draw_line(-x + x0, y + y0, x + x0, y + y0, addressingMode);
			draw_line(-x + x0, -y + y0, x + x0, -y + y0, addressingMode);
			if (x != y) {
				draw_line(y + x0, x + y0, -y + x0, x + y0, addressingMode);
				draw_line(y + x0, -x + y0, -y + x0, -x + y0, addressingMode);
			}
		} else {
			draw_point_at(x + x0, y + y0, addressingMode);
			draw_point_at(-x + x0, y + y0, addressingMode);
			draw_point_at(x + x0, -y + y0, addressingMode);
			draw_point_at(-x + x0, -y + y0, addressingMode);
			if (x != y) {
				draw_point_at(y + x0, x + y0, addressingMode);
				draw_point_at(-y + x0, x + y0, addressingMode);
				draw_point_at(y + x0, -x + y0, addressingMode);
				draw_point_at(-y + x0, -x + y0, addressingMode);
			}
		}
	}
}