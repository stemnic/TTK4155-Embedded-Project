/*
 * io.c
 *
 * Created: 06.09.2019 10:13:09
 *  Author: Ole Sivert
 */ 
#include <avr/io.h>
#include "io.h"
#include "adc.h"
#include "../compact_math.h"

/* Returns the value of the given button */
uint8_t get_button(uint8_t button) {
	uint8_t i = PINB;
	return (i >> button) & 1;
}

/* Method to read the current status of buttons, sliders and joystick into the given controllerInput struct.
Sets all values in the struct, including flags to test if the value has been changed.
Returns 1 if any value has been changed since last read */
uint8_t read_controller_status(controller_input_t *buffer) {
	uint8_t changes = 0;

	uint8_t js_button = !get_button(JOYSTICK_BUTTON);
	uint8_t button_one = get_button(BUTTON_1);
	uint8_t button_two = get_button(BUTTON_2);

	if (buffer->joystick_button != js_button) {
		changes = 1;
		buffer->joystick_button = js_button;
		buffer->joystick_button_changed = 1;
	} else {
		buffer->joystick_button_changed = 0;
	}

	if (buffer->button_one_value != button_one) {
		changes = 1;
		buffer->button_one_value = button_one;
		buffer->button_one_changed = 1;
	} else {
		buffer->button_one_changed = 0;
	}

	if (buffer->button_two_value != button_two) {
		changes = 1;
		buffer->button_two_value = button_two;
		buffer->button_two_changed = 1;
	} else {
		buffer->button_two_changed = 0;
	}
	
	int8_t joystick_x = get_joystick_value(JOYSTICK_X);
	if (joystick_x != buffer->joystick_x) {
		changes = 1;
		buffer->joystick_x = joystick_x;
	}

	int8_t joystick_y = get_joystick_value(JOYSTICK_Y);
	if (joystick_y != buffer->joystick_y) {
		changes = 1;
		buffer->joystick_y = joystick_y;
	}
	
	int joystick_trigger = 0;
	if (buffer->joystick_y > 40) joystick_trigger = 1;
	if (buffer->joystick_y < -40) joystick_trigger = -1;
	if (buffer->joystick_trigger_last != joystick_trigger && joystick_trigger != 0) {
		changes = 1;
		buffer->joystick_trigger = joystick_trigger;
	} else {
		if (buffer->joystick_trigger != 0) {
			changes = 1;
		}
		buffer->joystick_trigger = 0;
	}
	buffer->joystick_trigger_last = joystick_trigger;
	
	uint8_t slider_one = get_slider_value(SLIDER_1);
	uint8_t slider_two = get_slider_value(SLIDER_2);
	if (abs_diff(buffer->slider_one_value, slider_one) > 5) {
		changes = 1;
		buffer->slider_one_value = slider_one;
	}
	if (abs_diff(buffer->slider_two_value, slider_two) > 5) {
		changes = 1;
		buffer->slider_two_value = slider_two;
	}
	return changes;
}