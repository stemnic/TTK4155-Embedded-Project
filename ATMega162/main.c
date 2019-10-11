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

void exercise6() {
	can_set_device_mode(CAN_MODE_NORMAL);
	uint8_t buffer[4] = { 1, 2, 3, 4 };
	can_msg_t message;
	message.data = buffer;
	message.dataLen = 4;
	message.id = 0x1ff;
	printf("Sending test CAN Data\n");
	can_send_data(&message);
	while (1)
	{
		printf("Waiting for data from can client\n");
		can_receive_blocking(&message);
		printf("Receive msg with id: %i, len: %i\n", message.id, message.dataLen);
		for (int i = 0; i < message.dataLen; i++) {
			printf("  %i: %i", i, message.data[i]);
		}
		printf("\n");
		message.data[0]++;
		can_send_data(&message);
		printf("Can message was sent\n");
		_delay_ms(1000);
	}
}

void process_cycle_clock_init() {
	TCCR0 |= (0b100 << 0); // clk/256 will interrupt at an interval of 13ms
	TIMSK |= (1 << 1); // Overflow interrupt enable
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
	SRAM_test();
	ADC_init();
	oled_init();
	spi_init();
	can_init();
	
	wipe_buffer();
	char * liststr[4] = { "test1", "test2", "test3", "test4" };
	ui_list_init(liststr, 4);
	ui_buttons_init("yes", "no");
	// ui_draw_big_number(75);
	
	process_cycle_clock_init();
	
	can_set_device_mode(CAN_MODE_NORMAL);
	
	uint8_t data[4];
	can_msg_t msg;
	msg.id = 1;
	msg.data = data;
	msg.dataLen = 4;
	
	can_msg_t recmsg;
	recmsg.id = 2;
	recmsg.data = NULL;
	
	flush_buffer();
	uint8_t score = 0;
	while (1){
		if (adc_read){
			if (read_controller_status(&input)) {
				ui_list_update(input.joystick_trigger);
				if (input.button_one_changed) {
					ui_button_trigger(BUTTON_1, input.button_one_value);
				}
				if (input.button_two_changed) {
					ui_button_trigger(BUTTON_2, input.button_two_value);
				}
				if (input.joystick_button_changed && input.joystick_button) {
					printf("%s\n", liststr[get_list_pos()]);
				}
				flush_buffer();
				data[0] = input.joystick_x;
				data[1] = input.joystick_y;
				data[2] = input.joystick_button;
				data[3] = input.slider_two_value;
				can_send_data(&msg);
			}
			
			if (can_try_receive(&recmsg)) {
				if (recmsg.id == 2) {
					score++;
					printf("Score: %i\n", score);
				}
			}
			
			adc_read = 0;
		}
		_delay_ms(1);
	}
}

