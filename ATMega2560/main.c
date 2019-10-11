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

uint16_t score = 0;

void score_add(){
	score++;
	printf("Current score: %i\n", score);
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
	
	process_cycle_clock_init();
		
    // Replace with your application code 
	printf("Init Complete\n");
	uint8_t buffer[4];
	can_msg_t message;
	message.data = buffer;
	message.dataLen = 4;
	
	can_msg_t scoremsg;
	scoremsg.data = NULL;
	scoremsg.id = 2;
	scoremsg.dataLen = 0;
	
	uint8_t previous_ir_trigger = 0;
	while (1) {
		if (timer_int_trigger) {
			uint8_t ir_trigger = (adc_get_ir_conversion() < 500);
					
			if (ir_trigger != previous_ir_trigger) {
				//printf ("ir trigger: %i\n", ir_trigger);
				previous_ir_trigger = ir_trigger;
				if (ir_trigger == 1){
					can_send_data(&scoremsg);
					score_add();
				}
			}
			timer_int_trigger = 0;
		}

		
		if (can_try_receive(&message)) {
			if (message.id == 1) {
				pwm_set_position(message.data[3]);
			}
		}
		//printf("ir trigger: %i\n", ir_trigger);
		//printf("x: %i, y: %i, button: %i, slider: %i\n", (int8_t)message.data[0], (int8_t)message.data[1], message.data[2], (uint8_t)message.data[3]);
	}
}

