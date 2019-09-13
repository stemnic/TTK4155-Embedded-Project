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

void oled_init(){
	oled_Command(DISPLAY_OFF);
	
	oled_Command(MEMORY_ADDR_MODE);
	oled_Command(MEMORY_ADDR_MODE_PAGE);
	
	oled_Command(SEGMENT_REMAP);
	oled_Command(COM_REVERSE_SCAN_DIR);
	
	oled_Command(PAGE_START_ADDR_PAGE_BASE+1);
	
	oled_Command(DISPLAY_ON);
}

void oled_Clear(){
	for (int page = 0; page < 8; page++)
	{
		oled_Command(PAGE_START_ADDR_PAGE_BASE + page);
		for (int col = 0; col < 128; col++)
		{
			oled_data[0] = 0x00;
		}
	}
}

int currentPage = 0;
int currentCol = 0;

void oled_write_string (char *chr_ptr, int fontSize){
	unsigned char PROGMEM font;
	switch(fontSize) {
		case FONT8x8:
			font = font8;
			break;
		case FONT5x7:
			font = font5;
			break;
		case FONT4x6:
			font = font4;
			break;
		default:
			return;
	}
	for (int num = 0; chr_ptr[num] != '\0'; num++) {
		if (chr_ptr[num] == '\n') {
			if (++currentPage > 7){
				currentPage = 0;
			}
			if (currentCol < 127) {
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
			switch(fontSize) {
				case FONT8x8:
					oled_data[0] =  pgm_read_byte(&(font8[chr_ptr[num]-32][col]));
					break;
				case FONT5x7:
					oled_data[0] =  pgm_read_byte(&(font5[chr_ptr[num]-32][col]));
					break;
				case FONT4x6:
					oled_data[0] =  pgm_read_byte(&(font4[chr_ptr[num]-32][col]));
					break;
				default:
					return;
			}
			currentCol++;
		}
	}
}

void oled_Command(uint8_t command){
	oled_cmd[0] = command;
}