/*
 * io.h
 *
 * Created: 06.09.2019 10:13:01
 *  Author: Ole Sivert
 */ 


#ifndef IO_H_
#define IO_H_

#include <stdint.h>

#define JOYSTICK_BUTTON 0
#define BUTTON_1 1
#define BUTTON_2 2

typedef struct controllerInput{
	int8_t joystick_x; // 8
	int8_t joystick_y; // 8

	int8_t joystick_trigger; // 8 (for negative values)
	int8_t joystick_trigger_last;

	uint8_t slider_one_value; // 8
	uint8_t slider_two_value; // 8

	uint8_t button_one_value:1; // 1, total 6
	uint8_t button_one_changed:1;

	uint8_t button_two_value:1;
	uint8_t button_two_changed:1;

	uint8_t joystick_button:1;
	uint8_t joystick_button_changed:1;
} controllerInput;

uint8_t get_button(int button);

uint8_t read_controller_status(controllerInput *buffer);

#endif /* IO_H_ */