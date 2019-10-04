/*
 * io.c
 *
 * Created: 06.09.2019 10:13:09
 *  Author: Ole Sivert
 */ 
#include <avr/io.h>
#include "io.h"

uint8_t joystickButton_Read(){
	unsigned char i = PINB;
	return !((uint8_t) (i >> 0) & 1);
}

uint8_t sliderButton1_Read(){
	unsigned char i = PINB;
	return (uint8_t) (i >> 1) & 1;
}

uint8_t sliderButton2_Read(){
	unsigned char i = PINB;
	return (uint8_t) (i >> 2) & 1;
}

uint8_t getButton(int button) {
	uint8_t i = PINB;
	return (i >> button) & 1;
}

void getControllerButtons(controllerInput *controllerBuffer){
	controllerBuffer->joystick_button = joystickButton_Read();
	controllerBuffer->slider_one_button = sliderButton1_Read();
	controllerBuffer->slider_two_button = sliderButton2_Read();
}