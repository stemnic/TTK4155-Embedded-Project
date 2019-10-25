/*
 * Byggern.c
 *
 * Created: 30/08/2019 12:42:47
 * Author : Einar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "drivers/uart.h"
#include "drivers/io.h"
#include "drivers/sram.h"
#include "drivers/adc.h"
#include "drivers/oled.h"
#include "drivers/spi.h"
#include "drivers/can.h"
#include "graphics/oled_buffer.h"
#include "graphics/ui.h"
#include "graphics/simulator.h"


#define MOTOR_SPD_SCALE 15

void process_cycle_clock_init() {
	TCCR0 |= (0b100 << 0); // clk/256 will interrupt at an interval of 13ms
	TIMSK |= (1 << 1); // Overflow interrupt enable
}

double motor_pos = 127;

uint8_t get_motor_pos(controllerInput* input) {
	uint8_t conv_motor_pos = 0;
	motor_pos += ((float)input->joystick_x) / MOTOR_SPD_SCALE;

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

controllerInput input;
uint8_t adc_read = 0;

ISR (TIMER0_OVF_vect){
	adc_read = 1;
}

int main(void) {
	uart_init();
	fdevopen(uart_transmit_stdio, uart_receive_char);
	//fdevopen(oled_write_char,uart_receive_char);
	
	SRAM_init();
	//SRAM_test();
	ADC_init();
	oled_init();
	spi_init();
	can_init();
	
	wipe_buffer();
	char * liststr[4] = { "Simulate", "Play" };

	ui_menu_init(liststr, 2);
	
	process_cycle_clock_init();
	
	can_set_device_mode(CAN_MODE_NORMAL);
	
	uint8_t data[6];
	can_msg_t msg;
	msg.id = 1;
	msg.data = data;
	msg.dataLen = 6;
	
	can_msg_t recmsg;
	recmsg.id = 2;
	recmsg.data = NULL;
	
	uint8_t ui_menu = UI_MENU_MAIN;

	flush_buffer();

	uint8_t game_over = 0;


	uint8_t changes;
	while (1) {
		if (adc_read) {
			changes = read_controller_status(&input);
			switch(ui_menu) {
				case UI_MENU_MAIN:
					ui_menu_tick();
					if (changes) ui_menu_update(&input);
					if (input.joystick_button_changed && input.joystick_button) {
						uint8_t next = get_list_pos();
						if (next == UI_MENU_SIM || next == UI_MENU_RUN) {
							ui_menu = next;
							wipe_buffer();
							sim_init(next == UI_MENU_SIM ? SIM_MODE_SIMULATOR : SIM_MODE_RUN);
						}
					}
					break;
				case UI_MENU_SIM:
					if (changes || input.joystick_x) {
						uint8_t conv_motor_pos = get_motor_pos(&input);
						ui_simulator_update(&input, conv_motor_pos);
					}
					sim_tick();
					if (input.button_one_changed && input.button_one_value) {
						ui_menu = UI_MENU_MAIN;
						wipe_buffer();
						ui_menu_init(liststr, 2);
					}
					break;
				case UI_MENU_RUN:
					if (game_over) {
						game_over++;
						if (game_over % 60 == 0) {
							draw_large_num(10, 40, 3 - game_over / 60 + 1, OLED_ADDR_DISABLE);
							if (game_over == 180) {
								game_over = 0;
							} else {
								draw_large_num(10, 40, 3 - game_over / 60, OLED_ADDR_LAYER);
							}
						}
						data[0] = 0;
						data[1] = 127;
						data[2] = 0;
						data[3] = 127;
						data[4] = 1;
						data[5] = 0;
						can_send_data(&msg);
						break;
					}
					sim_tick();
					if (input.button_one_changed && input.button_one_value) {
						ui_menu = UI_MENU_MAIN;
						wipe_buffer();
						ui_menu_init(liststr, 2);
						data[0] = 0;
						data[1] = 127;
						data[2] = 0;
						data[3] = 127;
						data[4] = 1;
						data[5] = 0;
						can_send_data(&msg);
					} else if (changes || input.joystick_x) {
						uint8_t conv_motor_pos = get_motor_pos(&input);
						ui_simulator_update(&input, conv_motor_pos);
						data[0] = input.joystick_x;
						data[1] = 255 - conv_motor_pos;
						data[2] = input.joystick_button;
						data[3] = input.slider_two_value;
						data[4] = input.button_one_changed && input.button_one_value;
						data[5] = input.button_two_changed && input.button_two_value;
						can_send_data(&msg);
					}
			}


			if (can_try_receive(&recmsg)) {
				if (recmsg.id == 2 && ui_menu == UI_MENU_RUN && !game_over) {
					draw_large_num(10, 40, 3, OLED_ADDR_LAYER);
					motor_pos = 127;
					ui_simulator_update(&input, 127);
					game_over = 1;
					printf("Game over!\n");
				}
			}
			flush_buffer();
			adc_read = 0;
		}
		_delay_ms(1);
	}
}

