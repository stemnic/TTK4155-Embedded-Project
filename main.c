/*
 * Byggern.c
 *
 * Created: 30/08/2019 12:42:47
 * Author : Einar
 */ 

#define F_CPU 4915200

#include <avr/io.h>
#include <stdio.h>
#include "drivers/uart.h"
#include "drivers/io.h"
#include "drivers/sram.h"
#include "drivers/adc.h"
#include "drivers/oled.h"
#include "graphics/oled_buffer.h"

#define BUFFER_LEN 128

void exercise1v1() {
	char recBuffer[128];
	for (;;) {
		gets(recBuffer);
		printf(recBuffer);
	}
}

void exercise1v2() {
	char recBuffer[128];
	for (;;) {
		if (uart_receive(recBuffer, 128)) {
			// uart_print(recBuffer);
			printf(recBuffer);
			printf("Got some stuff yo\n");
		}
	}
}

void exercise2() {
	for (;;) {
		printf("Hello world!\n");
		SRAM_test();
		_delay_ms(1000);
	}
}

void exercise3(){
	for (;;)
	{
		int8_t xVal = getJoystickValue(JOYSTICK_X);
		int8_t yVal = getJoystickValue(JOYSTICK_Y);
		int8_t btnVal = joystickButton_Read();
		printf("Joystick: (%i, %i) (%i)\n", xVal, yVal, btnVal);
		uint8_t slider_1 = getSliderValue(SLIDER_1);
		uint8_t slider_2 = getSliderValue(SLIDER_2);
		printf("Sliders: %i and %i\n", slider_1, slider_2);
		uint8_t button1 = getButton(BUTTON_1);
		uint8_t button2 = getButton(BUTTON_2);
		printf("touch(%i, %i)\n", button1, button2);

		_delay_ms(200);
	}
}

void exercise4_1(){
	
	for (;;)
	{
		//printf("Hello World\n");
		oled_write_string("Hello World\n", FONT8x8);
		_delay_ms(1000);
	}
}

void exercise4_2(){
	wipe_buffer();
	wipe_buffer();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 8; j++) {
			draw_block_at(i, j, OLED_ADDR_OVERWRITE);
		}
	}
	draw_string_at(1, 12, "Hello", FONT8x8, OLED_ADDR_INVERT);
	draw_box(4, 4, 60, 124, 2, OLED_ADDR_INVERT);
	flush_buffer();
	flush_buffer();
}

void process_cycle_clock_init(){
	TCCR0 |= (0b100 << 0); // clk/256 will interrupt at an interval of 13ms
	TIMSK |= (1 << 1); // Overflow interrupt enable
}

controllerInput input;
uint8_t adc_read = 0;

ISR (TIMER0_OVF_vect){
	flush_buffer();
	getControllerButtons(&input);
	adc_read = 1;
}

void process_cycle(){
	
}

int main(void)
{
	char buffer[BUFFER_LEN];
	uart_init(buffer, BUFFER_LEN);
	fdevopen(uart_transmit_stdio, uart_receive_char);
	//fdevopen(oled_write_char,uart_receive_char);
	
	SRAM_init();
	SRAM_test();
	ADC_init();
	oled_init();
	
	process_cycle_clock_init();
	
	//exercise4_2();
	
	while (1){
		if (adc_read){
			//Can't be called as an interrupt
			input.joystick_x = getJoystickValue(JOYSTICK_X);
			input.joystick_y = getJoystickValue(JOYSTICK_Y);
			input.slider_one_value = getSliderValue(SLIDER_1);
			input.slider_two_value = getSliderValue(SLIDER_2);
			adc_read = 0;
		}
	}
	
	//exercise3();
	//exercise2();
	//exercise1v2();
}

