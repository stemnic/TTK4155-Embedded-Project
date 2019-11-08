/*
 * oled_buffer.h
 *
 * Created: 20/09/2019 09:38:57
 *  Author: Einar
 */ 


#ifndef OLED_BUFFER_H_
#define OLED_BUFFER_H_

#include <stdint.h>

#define OLED_ADDR_OVERWRITE 0
#define OLED_ADDR_LAYER 1
#define OLED_ADDR_INVERT 2
#define OLED_ADDR_DISABLE 3

#define FONT8x8 8
#define NUMBERS9x16 9
#define RACKET 0

/* We use a buffer in SRAM for the OLED screen, it is easier to do clever graphical manipulations in readable memory,
and it is faster to only write to OLED at fixed intervals. */

/* Wipe the SRAM buffer */
void wipe_buffer();
/* Write the buffer to the OLED controller */
void flush_buffer();

/* Write a byte of data up to len (the lowest len bits), using the given addressing mode
This does full resolution, so a single byte may modify two bytes in SRAM. addressing_mode is as follows:
OLED_ADDR_OVERWRITE: Set the bits, ignoring previous values
OLED_ADDR_LAYER: OR the bits into OLED
OLED_ADDR_DISABLE: Disable the bits, using data as a mask
OLED_ADDR_INVERT: Invert the bits, using data as a mask */
void draw_data_at(uint8_t x0, uint8_t y0, uint8_t data, uint8_t len, uint8_t addressing_mode);

/* Draw a string of characters at the given position */
void draw_string_at(uint8_t x0, uint8_t y0, char* str, uint8_t fontSize, uint8_t addressing_mode);
/* Draw a 16x9 number, with any number of digits */
void draw_num (uint8_t x0, uint8_t y0, uint16_t num, uint8_t large, uint8_t addressing_mode);
/* Draw given image coded as an array of 8-bit values with top left corner at (x0, y0).
dimx, dimy must be the full dimensions of the image, as drawing only part creates issues, since each byte is flipped.
If flip is 1, flip the image around the y-axis */
void draw_image_at(uint8_t x0, uint8_t y0, uint8_t img, uint8_t flip, uint8_t dimx, uint8_t dimy, uint8_t addressing_mode);

/* Draw a single bit at the given position */
void draw_point_at(uint8_t x0, uint8_t y0, uint8_t addressing_mode);

/* Draw a line from (x0, y0) to (x1, y1) */
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressing_mode);
/* Draw a box with corners (x0, y0) and (x1, y1), and given line width */
void draw_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width, uint8_t addressing_mode);
/* Draw a filled box with corners (x0, y0) and (x1, y1) */
void fill_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressing_mode);

/* Draw a circle with center (x0, y0), radius rad. If fill is 1, fill it */
void draw_circle(uint8_t x0, uint8_t y0, uint8_t rad, uint8_t fill, uint8_t addressing_mode);

/* Draw a rotated box, with center of one side at (x0, y0), length of that side width, length of perpendicular sides height,
rotated by given angle such that at 0, height is horizontal in positive x direction, offset by shift.
If ends is 1, draw ends, if sides is 1, draw sides */
void draw_rotated_box(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t angle, uint8_t ends, uint8_t sides, uint8_t shift, uint8_t addressing_mode);

/* Retrieve a single byte of the given font from progmem */
uint8_t get_font_byte(char in, uint8_t col, uint8_t fontSize);
/* Retrieve a single 16 bit integer from progmem, at [index][col] of array of 16 bit integers defined by img */
uint16_t get_font_dword(uint8_t index, uint8_t col, uint8_t img);
/* Retrieve a single byte of an image from progmem, at [row][col] */
uint8_t get_img_byte(uint8_t img, uint8_t row, uint8_t col);



#endif /* OLED_BUFFER_H_ */