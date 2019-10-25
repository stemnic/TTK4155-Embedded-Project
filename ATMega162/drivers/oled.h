/*
 * oled.h
 *
 * Created: 13.09.2019 13:23:05
 *  Author: Ole Sivert
 */ 


#ifndef OLED_H_
#define OLED_H_

#define DISPLAY_OFF 0xae
#define DISPLAY_ON 0xaf
#define MEMORY_ADDR_MODE 0x20
#define MEMORY_ADDR_MODE_PAGE 2
#define SEGMENT_REMAP 0xa1
#define PAGE_START_ADDR_PAGE_BASE 0xb0
#define RESET_PAGE_COLUMN 0x00
#define COM_REVERSE_SCAN_DIR 0xc8
#define PAGE_LOW_COL_START 0x00
#define PAGE_HIGH_COL_START 0x10
#define SCROLL_DISABLE 0x2e
#define SCROLL_ENABLE 0x2f

#define FONT8x8 8
#define FONT5x7 5
#define FONT4x6 4
#define NUMBERS9x16 9
#define RACKET 0


void oled_init();
void oled_clear();
void oled_write_string (char *chr_ptr, int fontSize);
void oled_write_char (char chr);
void oled_command(uint8_t command);
void oled_write_byte(uint8_t data);
uint8_t get_font_byte(char in, uint8_t col, uint8_t fontSize);
uint16_t get_font_dword(int index, uint8_t col, uint8_t img);
uint8_t get_img_byte(uint8_t img, uint8_t row, uint8_t col);


#endif /* OLED_H_ */