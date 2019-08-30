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

#define BUFFER_LEN 512

int main(void)
{
	char buffer[BUFFER_LEN];
	
	uart_init(buffer, BUFFER_LEN);
	fdevopen(uart_transmit, uart_receive_char);
    sei();
	
	char recBuffer[128];
	/* Replace with your application code */
	for (;;) {
		//gets(recBuffer);
		//printf(recBuffer);
		if (uart_receive(recBuffer, 128)) {
			// uart_print(recBuffer);
			printf(recBuffer);
			printf("Got some stuff yo\n");
		}
		_delay_ms(1000);
	}
}

