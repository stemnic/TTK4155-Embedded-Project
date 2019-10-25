/*
 * oled.c
 *
 * Created: 13.09.2019 13:23:18
 *  Author: Ole Sivert
 */ 

#include <avr/io.h>
#include "oled.h"
#include "fonts.h"


volatile char *oled_cmd = (char *) 0x1000;
volatile char *oled_data = (char *) 0x1200;

void oled_write(char chr){
	oled_write_string(&chr, FONT8x8);
}

void oled_command(uint8_t command){
	oled_cmd[0] = command;
}

void oled_init(){
	oled_command(DISPLAY_OFF);
	
	oled_command(MEMORY_ADDR_MODE);
	oled_command(MEMORY_ADDR_MODE_PAGE);
	
	oled_command(SEGMENT_REMAP);
	oled_command(COM_REVERSE_SCAN_DIR);
	oled_command(SCROLL_DISABLE);
	
	oled_command(PAGE_START_ADDR_PAGE_BASE);
	oled_command(PAGE_LOW_COL_START);
	oled_command(PAGE_HIGH_COL_START);
	
	oled_command(DISPLAY_ON);
	
	oled_clear();
}

int currentPage = 0;
int currentCol = 0;

void oled_clear(){
	for (int page = 0; page < 8; page++)
	{
		oled_command(PAGE_START_ADDR_PAGE_BASE + page);
		for (int col = 0; col < 128; col++)
		{
			oled_data[0] = 0x00;
		}
	}
	oled_command(PAGE_START_ADDR_PAGE_BASE);
	currentPage = 0;
	currentCol = 0;
}

uint8_t get_font_byte(char in, uint8_t col, uint8_t fontSize) {
	switch (fontSize) {
		case FONT8x8:
			return pgm_read_byte(&(font8[in - 32][col]));
		case FONT5x7:
			return pgm_read_byte(&(font8[in - 32][col]));
		case FONT4x6:
			return pgm_read_byte(&(font8[in - 32][col]));
		default:
			return 0;
	}
}

uint16_t get_font_dword(int index, uint8_t col, uint8_t img) {
	switch (img) {
		case NUMBERS9x16:
			return pgm_read_dword(&(font16[index][col]));
		default:
			return 0;
	}
}

uint8_t get_img_byte(uint8_t img, uint8_t row, uint8_t col) {
	switch (img) {
		case RACKET:
			return pgm_read_byte(&(racket[row][col]));
		default:
			return 0;
	}
}

void oled_write_byte(uint8_t data) {
	oled_data[0] = data;
}
