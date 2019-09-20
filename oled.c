/*
 * oled.c
 *
 * Created: 13.09.2019 13:23:18
 *  Author: Ole Sivert
 */ 

#include "oled.h"
#include "fonts.h"


volatile char *oled_cmd = (char *) 0x1000;
volatile char *oled_data = (char *) 0x1200;

void oled_write(char chr){
	oled_write_string(&chr, FONT8x8);
}

void oled_Command(uint8_t command){
	oled_cmd[0] = command;
}

void oled_init(){
	oled_Command(DISPLAY_OFF);
	
	oled_Command(MEMORY_ADDR_MODE);
	oled_Command(MEMORY_ADDR_MODE_PAGE);
	
	oled_Command(SEGMENT_REMAP);
	oled_Command(COM_REVERSE_SCAN_DIR);
	oled_Command(SCROLL_DISABLE);
	
	oled_Command(PAGE_START_ADDR_PAGE_BASE);
	oled_Command(PAGE_LOW_COL_START);
	oled_Command(PAGE_HIGH_COL_START);
	
	oled_Command(DISPLAY_ON);
	
	oled_Clear();
}

int currentPage = 0;
int currentCol = 0;

void oled_Clear(){
	for (int page = 0; page < 8; page++)
	{
		oled_Command(PAGE_START_ADDR_PAGE_BASE + page);
		for (int col = 0; col < 128; col++)
		{
			oled_data[0] = 0x00;
		}
	}
	oled_Command(PAGE_START_ADDR_PAGE_BASE);
	currentPage = 0;
	currentCol = 0;
}

void oled_write_char_internal (char chr, int fontSize){
	if (chr == '\n') {
		if (++currentPage > 7){
			currentPage = 0;
		}
		if (currentCol < 128) {
			for (;currentCol < 128; currentCol++) {
				oled_data[0] = 0x00;
			}
		}
		oled_Command(PAGE_START_ADDR_PAGE_BASE + currentPage);
		currentCol = 0;
		//oled_Command(RESET_PAGE_COLUMN);
		return;
	}else if(currentCol == 128) {
		if (++currentPage > 7){
			currentPage = 0;
		}
		oled_Command(PAGE_START_ADDR_PAGE_BASE + currentPage);
		currentCol = 0;
	}
	for (int col=0; col < fontSize; col++){
		switch(fontSize) {
			case FONT8x8:
			oled_data[0] =  pgm_read_byte(&(font8[chr-32][col]));
			break;
			case FONT5x7:
			oled_data[0] =  pgm_read_byte(&(font5[chr-32][col]));
			break;
			case FONT4x6:
			oled_data[0] =  pgm_read_byte(&(font4[chr-32][col]));
			break;
			default:
			return;
		}
		currentCol++;
	}
}

void oled_write_char (char chr){
	oled_write_char_internal(chr, FONT8x8);
}

void oled_write_string (char *chr_ptr, int fontSize){
	for (int num = 0; chr_ptr[num] != '\0'; num++) {
		if (chr_ptr[num] == '\n') {
			if (++currentPage > 7){
				currentPage = 0;
			}
			if (currentCol < 128) {
				for (;currentCol < 128; currentCol++) {
					oled_data[0] = 0x00;
				}
			}
			oled_Command(PAGE_START_ADDR_PAGE_BASE + currentPage);
			currentCol = 0;
			//oled_Command(RESET_PAGE_COLUMN);
			continue;
		}
		for (int col=0; col < fontSize; col++){
			oled_data[0] = get_font_byte(chr_ptr[num], col, fontSize);
			currentCol++;
		}
	}
}

uint8_t get_font_byte(char in, uint8_t col, uint8_t fontSize) {
	switch (fontSize) {
		case FONT8x8:
			return pgm_read_byte(&(font8[in - 32][col]));
		case FONT5x7:
			return pgm_read_byte(&(font5[in - 32][col]));
		case FONT4x6:
			return pgm_read_byte(&(font4[in - 32][col]));
		default:
			return 0;
	}
}

void oled_write_byte(uint8_t data) {
	oled_data[0] = data;
}