/*
 * oled.h
 *
 * Created: 13.09.2019 13:23:05
 *  Author: Ole Sivert
 */ 


#ifndef OLED_H_
#define OLED_H_

#include <avr/io.h>

#define DISPLAY_OFF 0xae
#define DISPLAY_ON 0xaf
#define MEMORY_ADDR_MODE 0x20
#define MEMORY_ADDR_MODE_PAGE 2
#define SEGMENT_REMAP 0xa1
#define PAGE_START_ADDR_PAGE_BASE 0xB0
#define RESET_PAGE_COLUMN 0x00
#define COM_REVERSE_SCAN_DIR 0xc8

#define FONT8x8 8
#define FONT5x7 5
#define FONT4x6 4


void oled_init();
void oled_Clear();
void oled_write_string (char *chr_ptr, int fontSize);



#endif /* OLED_H_ */