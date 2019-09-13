/*
 * io.c
 *
 * Created: 06.09.2019 10:13:09
 *  Author: Ole Sivert
 */ 

#include "io.h"

uint8_t joystickButton_Read(){
	unsigned char i = PINB;
	return (uint8_t) (i >> 0) & 1;
}

uint8_t sliderButton1_Read(){
	unsigned char i = PINB;
	return (uint8_t) (i >> 1) & 1;
}

uint8_t sliderButton2_Read(){
	unsigned char i = PINB;
	return (uint8_t) (i >> 2) & 1;
}