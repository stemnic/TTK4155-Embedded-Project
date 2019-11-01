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



/* Initialize the OLED controller, clearing the screen */
void oled_init();
/* Manually clear the screen */
void oled_clear();
/* Send a single command byte to the OLED controller */
void oled_command(uint8_t command);
/* Write a byte of data to the cursor */
void oled_write_byte(uint8_t data);


#endif /* OLED_H_ */