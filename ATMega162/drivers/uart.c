/*
 * uart.c
 *
 * Created: 30/08/2019 12:43:18
 *  Author: Einar
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 4915200
#define UBBR (F_CPU/16/BAUD-1)

/* Initialize the UART driver, setting baud rate and data format */
void uart_init() {
	/* Set baud rate */
	UBRR0H = (uint8_t)(UBBR >> 8);
	UBRR0L = (uint8_t)UBBR;
	/* Enable receiver and transmitter and rx flag*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
	sei();
}

/* Send a character over UART */
void uart_transmit(char data) {
	while (!(UCSR0A & (1<<UDRE0))) {}	//waits for ready
	UDR0=data;
}

/* Send a character over UART, version used for STDIO */
int uart_transmit_stdio(char data, FILE* file){
	uart_transmit(data);
	return 0;
}

/* Send an array of characters over UART */
void uart_print(const char string[]) {
	for(uint16_t i=0; string[i]!='\0'; i++){
		uart_transmit(string[i]);
	}
}

/* Receive a single character over UART, used for STDIO */
int uart_receive_char() {
	UCSR0B = UCSR0B & ~(1 << RXCIE0);
	while (!(UCSR0A & (1 << RXC0))) {}
	char dat = UDR0;
	UCSR0B = UCSR0B | (1 << RXCIE0);
	return (int)dat;
}

