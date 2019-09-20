/*
 * Byggern.c
 *
 * Created: 30/08/2019 12:42:47
 * Author : Einar
 */ 

#define F_CPU 4915200

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include "io.h"
#include "sram.h"
#include "adc.h"
#include "oled.h"

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
		printf("Joystick: (%i, %i)\n", xVal, yVal);
		uint8_t slider_1 = getSliderValue(SLIDER_1);
		uint8_t slider_2 = getSliderValue(SLIDER_2);
		printf("Sliders: %i and %i\n", slider_1, slider_2);
		uint8_t button1 = getButton(BUTTON_1);
		uint8_t button2 = getButton(BUTTON_2);
		printf("touch(%i, %i)\n", button1, button2);

		_delay_ms(200);
	}
}

void exercise4(){
	
	for (;;)
	{
		//printf("Hello World\n");
		oled_write_string("Hello World\n", FONT8x8);
		_delay_ms(1000);
	}
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
	
	exercise4();
	//exercise3();
	//exercise2();
	//exercise1v2();
}

