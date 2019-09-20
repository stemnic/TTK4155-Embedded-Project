/*
 * oled_buffer.h
 *
 * Created: 20/09/2019 09:38:57
 *  Author: Einar
 */ 


#ifndef OLED_BUFFER_H_
#define OLED_BUFFER_H_

#include <avr/io.h>
#include "../drivers/oled.h"


#define OLED_ADDR_OVERWRITE 0
#define OLED_ADDR_LAYER 1
#define OLED_ADDR_INVERT 2

void wipe_buffer();

void flush_buffer();

//64x128 
void draw_point_at(uint8_t row, uint8_t col, uint8_t addressingMode);
//8x128
void draw_string_at(uint8_t row, uint8_t col, char* str, uint8_t fontSize, uint8_t addressingMode);
//8x12
void draw_block_at(uint8_t row, uint8_t col, uint8_t addressingMode);
//8x128
void draw_data_at(uint8_t row, uint8_t col, uint8_t data, uint8_t addressingMode);
//8x12
void invert_block_at(uint8_t row, uint8_t col);
//64x128
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode);
void draw_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width, uint8_t addressingMode);
//8x12
void fill_box(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t addressingMode);




#endif /* OLED_BUFFER_H_ */