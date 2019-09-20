/*
 * oled_buffer.c
 *
 * Created: 20/09/2019 09:24:39
 *  Author: Einar
 */ 

#include "oled_buffer.h"

volatile char* ext_ram = (char *) 0x1800;

void wipe_buffer() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 128; j++) {
			ext_ram[i + (j << 3)] = 0x00;
		}
	}
}

void flush_buffer() {
	for (uint8_t i = 0; i < 8; i++) {
		oled_command(PAGE_START_ADDR_PAGE_BASE + i);
		for (uint8_t j = 0; j < 128; j++) {
			oled_write_byte(ext_ram[i + (j << 3)]);
		}
	}
}

void draw_data_at(uint8_t row, uint8_t col, uint8_t data, uint8_t addressingMode) {
	switch(addressingMode) {
	case OLED_ADDR_OVERWRITE:
		ext_ram[row + col*8] = data;
		break;
	case OLED_ADDR_LAYER:
		ext_ram[row + col*8] |= data;
		break;
	case OLED_ADDR_INVERT:
		ext_ram[row + col*8] = ext_ram[row + col * 8] & (~data);
	}
}

void draw_point_at(uint8_t row, uint8_t col, uint8_t addressingMode) {
	uint8_t irow = row / 8;
	uint8_t icol = col;
	uint8_t offset = row % 8;
	draw_data_at(irow, icol, 1 << offset, addressingMode);
}



void draw_block_at(uint8_t row, uint8_t col, uint8_t addressingMode) {
	for (int i = col*8; i < col*8 + 8; i++) {
		draw_data_at(row, i, (addressingMode == OLED_ADDR_INVERT) ? 0x00 : 0xFF, addressingMode);
	}
}

void draw_char_at(uint8_t row, uint8_t col, char towrite, uint8_t fontSize, uint8_t addressingMode) {
	for (int i=col; i < col + fontSize; i++) {
		draw_data_at(row, i, get_font_byte(towrite, i - col, fontSize), addressingMode);
	}
}

void draw_string_at(uint8_t row, uint8_t col, char* str, uint8_t fontSize, uint8_t addressingMode) {
	for (int i = 0; str[i] != '\0'; i++) {
		draw_char_at(row, col + i*fontSize, str[i], fontSize, addressingMode);
	}
}