/*
 * oled.c
 *
 * Created: 13.09.2019 13:23:18
 *  Author: Ole Sivert
 */ 

#include <avr/io.h>
#include "oled.h"


volatile char *oled_cmd = (char *) 0x1000;
volatile char *oled_data = (char *) 0x1200;

/* Execute given command on the oled controller */
void oled_command(uint8_t command){
	oled_cmd[0] = command;
}

/* Initialize the OLED controller */
void oled_init(){
	// Disable the display before configuring
	oled_command(DISPLAY_OFF);
	
	// Set the addressing mode to page
	oled_command(MEMORY_ADDR_MODE);
	oled_command(MEMORY_ADDR_MODE_PAGE);
	
	// Flip the image
	oled_command(SEGMENT_REMAP);
	oled_command(COM_REVERSE_SCAN_DIR);
	// Disable scrolling
	oled_command(SCROLL_DISABLE);
	
	// Set the cursor to the first address.
	oled_command(PAGE_START_ADDR_PAGE_BASE);
	oled_command(PAGE_LOW_COL_START);
	oled_command(PAGE_HIGH_COL_START);
	
	// Turn on the screen
	oled_command(DISPLAY_ON);
	
	// Clear the screen
	oled_clear();
}

uint8_t currentPage = 0;
uint8_t currentCol = 0;

/* Write 0x00 to every byte of the screen, then reset the cursor */
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

/* Write a single byte of data to the screen */
void oled_write_byte(uint8_t data) {
	oled_data[0] = data;
}
