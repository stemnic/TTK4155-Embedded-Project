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
	int8_t xVal;
	int8_t yVal;
	uint8_t slider_1;
	uint8_t slider_2;
	uint8_t joystickButton = 0;
	uint8_t touch1Button = 0;
	uint8_t touch2Button = 0;
	for (;;)
	{
		getJoystickPositions(&xVal, &yVal);
		printf("Joystick: (%i, %i)\n", xVal, yVal);
		getSliderValues(&slider_1, &slider_2);
		printf("Sliders: %i and %i\n", slider_1, slider_2);
		joystickButton = joystickButton_Read();
		touch1Button = sliderButton1_Read();
		touch2Button = sliderButton2_Read();
		printf("touch(%i, %i)\n", touch1Button, touch2Button);

		_delay_ms(200);
	}
}


int main(void)
{
	char buffer[BUFFER_LEN];
	uart_init(buffer, BUFFER_LEN);
	SRAM_init();
	SRAM_test();
	ADC_init();
	
	exercise3();
	//exercise2();
	//exercise1v2();
}

