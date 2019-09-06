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

#define BUFFER_LEN 512

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


int main(void)
{
	char buffer[BUFFER_LEN];
	uart_init(buffer, BUFFER_LEN);
	SRAM_init();
	
	exercise2();
}

