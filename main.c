/*
 * Byggern.c
 *
 * Created: 30/08/2019 12:42:47
 * Author : Einar
 */ 

#define F_CPU 4915200
#include <avr/io.h>
#include "uart.h"

#define BUFFER_LEN 512

int main(void)
{
	char buffer[BUFFER_LEN];
	uart_init(buffer, BUFFER_LEN);
    sei();
	char recBuffer[128];
	/* Replace with your application code */
	for (;;) {
		
		if (uart_receive(recBuffer, 128)) {
			uart_print(recBuffer);
		}
	}
}

