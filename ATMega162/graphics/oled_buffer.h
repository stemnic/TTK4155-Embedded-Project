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

void wipe_buffer();

void flush_buffer();

//64x128 
void draw_string_at(uint8_t row, uint8_t col, char* str, uint8_t fontSize, uint8_t addressingMode);
void draw_data_at(uint8_t row, uint8_t col, uint8_t data, uint8_t len, uint8_t addressingMode);
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode);
void draw_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width, uint8_t addressingMode);
void fill_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode);
void draw_large_num (uint8_t row, uint8_t col, uint8_t num, uint8_t addressingMode);
void draw_circle(uint8_t x0, uint8_t y0, uint8_t rad, uint8_t fill, uint8_t addressingMode);
void draw_point_at(uint8_t row, uint8_t col, uint8_t addressingMode);
void draw_rotated_box(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, int16_t angle, uint8_t ends, uint8_t sides, uint8_t shift, uint8_t addressingMode);
void draw_image_at(uint8_t x0, uint8_t y0, uint8_t img, uint8_t flip, uint8_t dimx, uint8_t dimy, uint8_t addressingMode);
uint8_t get_font_byte(char in, uint8_t col, uint8_t fontSize);
uint16_t get_font_dword(int index, uint8_t col, uint8_t img);
uint8_t get_img_byte(uint8_t img, uint8_t row, uint8_t col);



#endif /* OLED_BUFFER_H_ */