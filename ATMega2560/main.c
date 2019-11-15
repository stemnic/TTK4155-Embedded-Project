/*
 * ATMega2560.c
 *
 * Created: 04/10/2019 10:13:13
 * Author : Einar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "drivers/uart.h"
#include "drivers/mcp.h"
#include "drivers/spi.h"
#include "drivers/can.h"
#include "drivers/pwm.h"
#include "drivers/adc.h"
#include "drivers/motor.h"
#include "drivers/avrshock2.h"
#include <stdfix.h>

uint16_t score = 0;

#define CAN_MSG_GAMEOVER 2
#define CAN_MSG_DATA 1
#define CAN_MSG_PS2DATA 3
#define CAN_MSG_MODE_RUN 4
#define CAN_MSG_MODE_PS2 5
#define CAN_MSG_MODE_NONE 6

#define GAME_MODE_OFF 0
#define GAME_MODE_NORMAL 1
#define GAME_MODE_PS2 2

/*
 PS2 Dualshock mode are based on https://github.com/dhustkoder/avrshock2
*/

static const char* const button_names[] = {
	"SELECT", "L3", "R3", "START", "UP", "RIGHT", "DOWN", "LEFT",
	"L2", "R2", "L1", "R1", "TRIANGLE", "CIRCLE", "CROSS", "SQUARE"
};

static const char* const axis_names[] = {
	"RX", "RY",
	"LX", "LY"
};

accum motor_pos = 127;

#define MOTOR_SPD_SCALE_INV 0.067K

uint8_t get_motor_pos(int8_t joystick_x) {
	uint8_t conv_motor_pos = 0;
	motor_pos += MOTOR_SPD_SCALE_INV * (accum)joystick_x;

	if (motor_pos > 255) {
		motor_pos = 255;
		conv_motor_pos = 255;
		} else if (motor_pos < 0) {
		motor_pos = 0;
		conv_motor_pos = 0;
		} else {
		conv_motor_pos = (uint8_t)motor_pos;
	}
	return conv_motor_pos;
}

void solenoid_init(){
	PORTL |= (1 << PL6);
	DDRL |= (1 << PL6);
}

uint8_t solenoid_status = 0;

void solenoid_fire(){
	PORTL &= ~(1 << PL6);
	solenoid_status = 0;
}

void solenoid_reset() {
	if (solenoid_status++ == 5) {
		PORTL |= (1 << PL6);
	}
}

void process_cycle_clock_init() {
		PRR0 &= ~(1 << PRTIM0);
		TCCR0B = 0                                          /* TC8 Mode 0 Normal */
		| (1 << CS02) | (0 << CS01) | (1 << CS00); /* IO clock divided by 1024 */
		
		TIMSK0 = 0 << OCIE0B   /* Output Compare B Match Interrupt Enable: disabled */
		| 0 << OCIE0A /* Output Compare A Match Interrupt Enable: disabled */
		| 1 << TOIE0; /* Overflow Interrupt Enable: enabled */
}

uint8_t timer_int_trigger = 0;

ISR (TIMER0_OVF_vect){
	timer_int_trigger = 1;
}

int main(void) {
	uart_init();
	fdevopen(uart_transmit_stdio, uart_receive_char);
	spi_init();
	can_set_device_mode(CAN_MODE_CONFIGURATION);
	can_init();
	
	can_set_device_mode(CAN_MODE_NORMAL);
	
	pwm_init();
	adc_init();
	
	solenoid_init();
	
	process_cycle_clock_init();
	
	motor_init();
	
	avrshock2_init();
	avrshock2_set_mode(AVRSHOCK2_MODE_DIGITAL, false);
	
	spi_switch_mode(0);

	uint8_t mode = GAME_MODE_OFF;
	
	avrshock2_button_t buttons = 0;
	avrshock2_axis_t axis[AVRSHOCK2_AXIS_NAXIS];
		
	printf("Init Complete\n");
	uint8_t buffer[4];
	can_msg_t message;
	message.data = buffer;
	message.dataLen = 4;
	
	can_msg_t scoremsg;
	scoremsg.data = NULL;
	scoremsg.id = CAN_MSG_GAMEOVER;
	scoremsg.dataLen = 0;

	uint8_t ps2data[3];
	can_msg_t ps2msg;
	ps2msg.data = ps2data;
	ps2msg.dataLen = 3;
	ps2msg.id = CAN_MSG_PS2DATA;

	
	uint8_t previous_ir_trigger = 0;

	uint8_t previous_trigger_buttons = 0;

	uint8_t game_over = 0;
	uint8_t game_over_counter = 0;

	while (1) {
		if (timer_int_trigger) {
			uint8_t ir_trigger = (adc_get_ir_conversion() < 500);
			if (ir_trigger != previous_ir_trigger) {
				//printf ("ir trigger: %i\n", ir_trigger);
				previous_ir_trigger = ir_trigger;
				if (ir_trigger == 1 && (mode == GAME_MODE_NORMAL || mode == GAME_MODE_PS2)) {
					can_send_data(&scoremsg);
					if (mode == GAME_MODE_PS2) {
						game_over = 1;
						motor_pos = 127;
						motor_set_target_pos(127);
						pwm_set_value(127);
					}
				}
			}
			timer_int_trigger = 0;
			solenoid_reset();
			if (mode == GAME_MODE_PS2 && !game_over) {
				spi_switch_mode(1);
				if (avrshock2_poll(&buttons, axis)) {
					//printf("Controller mode: %.2X\n", (unsigned)avrshock2_get_mode());
					/* digital */
					//for (int i = 0; i < AVRSHOCK2_BUTTON_NBUTTONS; ++i)
					//	printf("BUTTON %s: %d\n", button_names[i], (buttons&(1<<i)) ? 1 : 0);
					/* axis */
					//for (int i = 0; i < AVRSHOCK2_AXIS_NAXIS; ++i)
					//	printf("AXIS %s: %d\n", axis_names[i], (int)axis[i]);
					//printf("\n\n");
				}
				spi_switch_mode(0);
				uint16_t l_trigger = (buttons & (1 << 8));
				uint16_t r_trigger = (buttons & (1 << 9));
				uint16_t l_trigger_2 = (buttons & (1 << 10));
				uint16_t r_trigger_2 = (buttons & (1 << 11));
				uint8_t trigger = ((l_trigger || r_trigger || l_trigger_2 || r_trigger_2) && !previous_trigger_buttons) ? 1 : 0;
				previous_trigger_buttons = l_trigger || r_trigger || l_trigger_2 || r_trigger_2;
				int16_t l_axis = axis[2] - 128;;
				if (l_axis < -127) l_axis = -127;
				if (l_axis > 127) l_axis = 127;
				if (l_axis < 2 && l_axis > -2) l_axis = 0;
				int8_t cont_input = (int8_t)l_axis * -1;

				uint8_t servo_input = axis[0];

				uint8_t conv_motor_pos = get_motor_pos(cont_input);

				motor_set_target_pos(conv_motor_pos);
				pwm_set_value(servo_input);
				if (trigger) {
					solenoid_fire();
				}
				ps2data[0] = 255 - conv_motor_pos;
				ps2data[1] = servo_input;
				ps2data[2] = trigger;
				can_send_data(&ps2msg);
			}
			if (game_over) {
				game_over_counter++;
				if (game_over_counter >= 180) {
					game_over = 0;
					game_over_counter = 0;
				}
			}


			motor_regulator_tick();
		}
		
		while (can_try_receive(&message)) {
			if (message.id == CAN_MSG_DATA && mode == GAME_MODE_NORMAL) {
				motor_set_target_pos(message.data[1]);
				
				pwm_set_value(message.data[3]);
				if (message.data[2]){
					solenoid_fire();
				}
			} else if (message.id == CAN_MSG_MODE_NONE) {
				mode = GAME_MODE_OFF;
			} else if (message.id == CAN_MSG_MODE_RUN) {
				mode = GAME_MODE_NORMAL;
			} else if (message.id == CAN_MSG_MODE_PS2) {
				mode = GAME_MODE_PS2;
			}
		}
		
		//printf("ir trigger: %i\n", ir_trigger);
		//printf("x: %i, y: %i, button: %i, slider: %i\n", (int8_t)message.data[0], (int8_t)message.data[1], message.data[2], (uint8_t)message.data[3]);
		
		// Example from library

		_delay_us(1);
	}
}

