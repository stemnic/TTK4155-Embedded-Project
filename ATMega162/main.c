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
#include "drivers/eeprom.h"
#include "graphics/oled_buffer.h"
#include "graphics/ui.h"
#include "graphics/simulator.h"
#include "compact_math.h"
#include <stdfix.h>


#define MOTOR_SPD_SCALE 15
#define MOTOR_SPD_SCALE_INV 0.067K

void process_cycle_clock_init() {
	TCCR0 |= (0b100 << 0); // clk/256 will interrupt at an interval of 13ms
	TIMSK |= (1 << 1); // Overflow interrupt enable
}

accum motor_pos = 127;


uint8_t get_motor_pos(controller_input_t* input) {
	uint8_t conv_motor_pos = 0;
	motor_pos += long_accum_mult(MOTOR_SPD_SCALE_INV, input->joystick_x);

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

uint8_t timer_int_tick = 0;

ISR (TIMER0_OVF_vect) {
	timer_int_tick++;
}

int main(void) {
	uart_init();
	fdevopen(uart_transmit_stdio, uart_receive_char);
	//fdevopen(oled_write_char,uart_receive_char);
	
	// Fix PB3 to be an input for WR bodge on PCB
	DDRB &= ~(1 << PB3 );
	
	SRAM_init();
	//SRAM_test();
	ADC_init();
	//printf("ADC init\n");
	oled_init();
	//printf("OLED init\n");
	spi_init();
	//printf("SPI init\n");
	can_init();
	//printf("CAN init\n");
	
	wipe_buffer();
	char * liststr[4] = { "Simulate", "Play", "High" };

	ui_menu_init(liststr, 3);
	
	process_cycle_clock_init();
	
	can_set_device_mode(CAN_MODE_NORMAL);
	
	controller_input_t input;

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

	uint16_t score_timer = 0;
	uint8_t score_counter = 0;
	uint8_t scoring_enabled = 0;

	uint8_t changes;


	while (1) {
		if (timer_int_tick) {
			uint8_t frames = timer_int_tick;
			timer_int_tick = 0;
			flush_buffer();
			changes = read_controller_status(&input);
			switch(ui_menu) {
				case UI_MENU_MAIN:
					ui_menu_tick(frames);
					if (changes) ui_menu_update(&input);
					if (input.joystick_button_changed && input.joystick_button) {
						uint8_t next = get_list_pos();
						if (next == UI_MENU_SIM || next == UI_MENU_RUN) {
							ui_menu = next;
							wipe_buffer();
							sim_init(next == UI_MENU_SIM ? SIM_MODE_SIMULATOR : SIM_MODE_RUN);
						} else if (next == UI_MENU_HIGH) {
							ui_menu = UI_MENU_HIGH;
							uint16_t sim_scores[5];
							uint16_t run_scores[5];
							wipe_buffer();
							get_scores(SIM_MODE_RUN, run_scores);
							get_scores(SIM_MODE_SIMULATOR, sim_scores);
							ui_scores_init(run_scores, sim_scores);
						}
					}
					break;
				case UI_MENU_SIM:
					if (changes || input.joystick_x) {
						uint8_t conv_motor_pos = get_motor_pos(&input);
						ui_simulator_update(&input, conv_motor_pos);
					}
					sim_tick(frames);
					if (input.button_one_changed && input.button_one_value) {
						ui_menu = UI_MENU_MAIN;
						wipe_buffer();
						ui_menu_init(liststr, 3);
					}
					break;
				case UI_MENU_RUN:
					if (game_over) {
						game_over++;
						if (game_over % 60 == 0) {
							draw_num(10, 40, 3 - game_over / 60 + 1, 1, OLED_ADDR_DISABLE);
							if (game_over == 180) {
								game_over = 0;
								sim_set_score(0);
							} else {
								draw_num(10, 40, 3 - game_over / 60, 1, OLED_ADDR_LAYER);
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
					sim_tick(frames);
					if (input.button_one_changed && input.button_one_value) {
						ui_menu = UI_MENU_MAIN;
						wipe_buffer();
						ui_menu_init(liststr, 3);
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
						if (!scoring_enabled && input.joystick_button) {
							scoring_enabled = 1;
						}
					}
					if (scoring_enabled) {
						if ((score_counter += frames) >= 60) {
							score_timer++;
							score_counter = 0;
							sim_set_score(score_timer);
						}
					}
					break;
				case UI_MENU_HIGH:
					if (input.button_one_changed && input.button_one_value) {
						ui_menu = UI_MENU_MAIN;
						wipe_buffer();
						ui_menu_init(liststr, 3);
						break;
					}
					if (input.button_two_changed && input.button_two_value) {
						ui_scores_button();
					}
			}


			if (can_try_receive(&recmsg)) {
				if (recmsg.id == 2 && ui_menu == UI_MENU_RUN && !game_over) {
					draw_num(10, 40, 3, 1, OLED_ADDR_LAYER);
					motor_pos = 127;
					ui_simulator_update(&input, 127);
					sim_set_score(3);
					game_over = 1;
					scoring_enabled = 0;
					score_counter = 0;
					// Persist score to EEPROM.
					update_scores(SIM_MODE_RUN, score_timer);

					score_timer = 0;
					printf("Game over!\n");
				}
			}
		}
		_delay_us(1);
	}
}

