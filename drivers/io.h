/*
 * io.h
 *
 * Created: 06.09.2019 10:13:01
 *  Author: Ole Sivert
 */ 


#ifndef IO_H_
#define IO_H_

#include <avr/io.h>

#define BUTTON_1 1
#define BUTTON_2 2

typedef struct controllerInput{
	uint8_t joystick_button;
	int8_t joystick_x;
	int8_t joystick_y;
	uint8_t slider_one_value;
	uint8_t slider_one_button;
	uint8_t slider_two_value;
	uint8_t slider_two_button;
	int8_t joystick_trigger;
} controllerInput;

uint8_t joystickButton_Read();

uint8_t sliderButton1_Read();

uint8_t sliderButton2_Read();

uint8_t getButton(int button);

void getControllerButtons(controllerInput *controllerBuffer);

#endif /* IO_H_ */