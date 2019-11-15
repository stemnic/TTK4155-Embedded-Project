/*
 * oled_buffer.c
 *
 * Created: 20/09/2019 09:24:39
 *  Author: Einar
 */ 
#include <avr/io.h>
#include "../drivers/oled.h"
#include "../compact_math.h"
#include "oled_buffer.h"
#include "geometry.h"
#include "fonts.h"
#include <stdio.h>
volatile char *ext_ram = (char*)0x1800;

/* 1 if the contents of the buffer has been changed since last flush */
uint8_t changed = 1;

/* Wipe the SRAM buffer */
void wipe_buffer() {
	changed = 1;
	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 128; j++) {
			ext_ram[((i << 7) + j) << 1] = 0x00;
		}
	}
}

/* Write the buffer to the OLED controller */
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

/* Write a byte of data up to len (the lowest len bits), using the given addressing mode
This does full resolution, so a single byte may modify two bytes in SRAM. addressing_mode is as follows:
OLED_ADDR_OVERWRITE: Set the bits, ignoring previous values
OLED_ADDR_LAYER: OR the bits into OLED
OLED_ADDR_DISABLE: Disable the bits, using data as a mask
OLED_ADDR_INVERT: Invert the bits, using data as a mask */
void draw_data_at(uint8_t x, uint8_t y, uint8_t data, uint8_t len, uint8_t addressing_mode) {
	if (x > 63 || y > 127) {
		return;
	}
	changed = 1;
	uint16_t addr1 = ((((x / 8) << 7) + y) << 1);
	uint16_t addr2 = ((((x / 8) + 1) << 7) + y) << 1;
	uint8_t offset = x % 8;
	uint8_t mask = 0xFF << offset;
	switch(addressing_mode) {
	case OLED_ADDR_OVERWRITE:
		// Clear bits to be modified, then set with or
		ext_ram[addr1] = (ext_ram[addr1] & ~mask) | (data << offset);
		if (offset && len > (8 - offset) && x < 63) ext_ram[addr2] = (ext_ram[addr2] & mask) | (data >> (8 - offset));
		break;
	case OLED_ADDR_LAYER:
		ext_ram[addr1] |= (data << offset);
		if (offset && len > (8 - offset) && x < 63) ext_ram[addr2] |= (data >> (8 - offset));
		break;
	case OLED_ADDR_DISABLE:
		ext_ram[addr1] &= ~(data << offset);
		if (offset && len > (8 - offset) && x < 63) ext_ram[addr2] &= ~(data >> (8 - offset));
		break;
	case OLED_ADDR_INVERT:
		ext_ram[addr1] ^= (data << offset);
		if (offset && len > (8 - offset) && x < 63) ext_ram[addr2] ^= (data >> (8 - offset));
	}
}
/* Draw a character at the given position */
void draw_char_at(uint8_t x, uint8_t y, char to_write, uint8_t font_size, uint8_t addressing_mode) {
	for (uint8_t i = y; i < y + font_size; i++) {
		draw_data_at(x, i, get_font_byte(to_write, i - y, font_size), 8, addressing_mode);
	}
}
/* Draw a string of characters at the given position */
void draw_string_at(uint8_t x, uint8_t y, char *str, uint8_t font_size, uint8_t addressing_mode) {
	for (uint8_t i = 0; str[i] != '\0'; i++) {
		draw_char_at(x, y + i * font_size, str[i], font_size, addressing_mode);
	}
}

/* Draw a single bit at the given position */
void draw_point_at(uint8_t x, uint8_t y, uint8_t addressingMode) {
	draw_data_at(x, y, 1, 1, addressingMode);
}

/* Draw a line from (x0, y0) to (x1, y1) */
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode) {
	int16_t dx = abs_diff(x1, x0);
	int16_t dy = -abs_diff(y1, y0);
	int16_t err = dx + dy;
	int16_t sx = x0 < x1 ? 1 : -1;
	int16_t sy = y0 < y1 ? 1 : -1;
	while (1) {
		draw_point_at(x0, y0, addressingMode);
		if (x0 == x1 && y0 == y1) break;
		int16_t e2 = err * 2;
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

/* Draw a box with corners (x0, y0) and (x1, y1), and given line width */
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

/* Draw a filled box with corners (x0, y0) and (x1, y1) */
void fill_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode) {
	int8_t sx = x0 < x1 ? 8 : -8;
	int8_t sy = y0 < y1 ? 1 : -1;
	uint8_t dx = abs_diff(x1, x0);
	uint8_t x = x0;
	while (dx > 0) {
		uint8_t len = dx < 8 ? dx : 8;
		for (uint8_t y = y0; y <= y1; y += sy) {
			draw_data_at(x, y, 0xFF, len, addressingMode);
		}
		dx -= len;
		x += sx;
	}
}

/* Draw a rotated box, with center of one side at (x0, y0), length of that side width, length of perpendicular sides height,
rotated by given angle such that at 0, height is horizontal in positive x direction, offset by shift.
If ends is 1, draw ends, if sides is 1, draw sides */
void draw_rotated_box(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t angle, uint8_t ends, uint8_t sides, uint8_t shift, uint8_t addressing_mode) {
	rectangle rect = geo_build_rect(x0, y0, width, height, angle, shift);
	if (sides) {
		draw_line(rect.ax, rect.ay, rect.bx, rect.by, addressing_mode);
		draw_line(rect.dx, rect.dy, rect.cx, rect.cy, addressing_mode);
	}
	if (ends) {
		draw_line(rect.bx, rect.by, rect.cx, rect.cy, addressing_mode);
		draw_line(rect.dx, rect.dy, rect.ax, rect.ay, addressing_mode);
	}
}

/* Draw a 16x9 number */
void draw_num (uint8_t x0, uint8_t y0, uint16_t num, uint8_t large, uint8_t addressing_mode) {
	uint8_t cnt = 0;
	uint8_t len = num == 0 ? 1 : num_len(num);
	do {
		uint8_t digit = num % 10;
		if (large) {
			for (uint8_t i = 0; i < 9; i++) {
				uint16_t val = get_font_dword(digit, i, NUMBERS9x16);
				draw_data_at(x0, (y0 + i) + 9 * (len - cnt), val & 0xFF, 8, addressing_mode);
				draw_data_at(x0 + 8, (y0 + i) + 9 * (len - cnt), (val & 0xFF00) >> 8, 8, addressing_mode);
			}
		} else {
			draw_char_at(x0, y0 + 8 * (len - cnt), '0' + digit, FONT8x8, addressing_mode);
		}
		num = num / 10;
		cnt++;
	} while (num > 0);
}

/* Draw a circle with center (x0, y0), radius rad. If fill is 1, fill it */
void draw_circle(uint8_t x0, uint8_t y0, uint8_t rad, uint8_t fill, uint8_t addressing_mode) {
	int16_t x = rad, y = 0;
	int16_t p = 1 - rad;

	if (fill && rad > 0) {
		draw_line(x + x0, y + y0, -x + x0, y + y0, addressing_mode);
		draw_line(y + x0, x + y0, y + x0, -x + y0, addressing_mode);
	}
	else {
		draw_point_at(x + x0, y + y0, addressing_mode);
		if (rad > 0) {
			draw_point_at(-x + x0, y + y0, addressing_mode);
			draw_point_at(y + x0, -x + y0, addressing_mode);
			draw_point_at(y + x0, x + y0, addressing_mode);
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
			draw_line(-x + x0, y + y0, x + x0, y + y0, addressing_mode);
			draw_line(-x + x0, -y + y0, x + x0, -y + y0, addressing_mode);
			if (x != y) {
				draw_line(y + x0, x + y0, -y + x0, x + y0, addressing_mode);
				draw_line(y + x0, -x + y0, -y + x0, -x + y0, addressing_mode);
			}
		} else {
			draw_point_at(x + x0, y + y0, addressing_mode);
			draw_point_at(-x + x0, y + y0, addressing_mode);
			draw_point_at(x + x0, -y + y0, addressing_mode);
			draw_point_at(-x + x0, -y + y0, addressing_mode);
			if (x != y) {
				draw_point_at(y + x0, x + y0, addressing_mode);
				draw_point_at(-y + x0, x + y0, addressing_mode);
				draw_point_at(y + x0, -x + y0, addressing_mode);
				draw_point_at(-y + x0, -x + y0, addressing_mode);
			}
		}
	}
}

/* Draw given image coded as an array of 8-bit values with top left corner at (x0, y0).
dimx, dimy must be the full dimensions of the image, as drawing only part creates issues, since each byte is flipped.
If flip is 1, flip the image around the y-axis */
void draw_image_at(uint8_t x0, uint8_t y0, uint8_t img, uint8_t flip, uint8_t dimx, uint8_t dimy, uint8_t addressing_mode) {
	for (uint8_t i = 0; i < dimy / 8; i++) {
		for (uint8_t j = 0; j < dimx; j++) {
			if (flip) {
				draw_data_at(i * 8 + y0, j + x0, get_img_byte(img, dimx - j - 1, dimy / 8 - i - 1), 8, addressing_mode);
			} else {
				draw_data_at(i * 8 + y0, j + x0, get_img_byte(img, j, dimy / 8 - i - 1), 8, addressing_mode);
			}
		}
	}
}
/* Retrieve a single byte of the given font from progmem */
uint8_t get_font_byte(char in, uint8_t col, uint8_t font_size) {
	switch (font_size) {
		case FONT8x8:
			return pgm_read_byte(&(font8[in - 32][col]));
		default:
			return 0;
	}
}

/* Retrieve a single 16 bit integer from progmem, at [index][col] of array of 16 bit integers defined by img */
uint16_t get_font_dword(uint8_t index, uint8_t col, uint8_t img) {
	switch (img) {
		case NUMBERS9x16:
			return pgm_read_dword(&(font16[index][col]));
		default:
			return 0;
	}
}

/* Retrieve a single byte of an image from progmem, at [row][col] */
uint8_t get_img_byte(uint8_t img, uint8_t row, uint8_t col) {
	switch (img) {
		case RACKET:
			return pgm_read_byte(&(racket[row][col]));
		default:
			return 0;
	}
}