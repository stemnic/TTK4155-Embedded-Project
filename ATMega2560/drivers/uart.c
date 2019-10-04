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

//UDR0 Data

#define UBBR (F_CPU/16/BAUD-1)

void uart_init() {
	/* Set baud rate */
	UBRR0H = (unsigned char)(UBBR >> 8);
	UBRR0L = (unsigned char)UBBR;
	/* Enable receiver and transmitter and rx flag*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	sei();
}

void uart_transmit(char data) {
	while (!(UCSR0A & (1<<UDRE0))) {}	//waits for ready
	UDR0=data;
}

int uart_transmit_stdio(char data, FILE* file){
	uart_transmit(data);
	return 0;
}

void uart_print(const char string[]) {
	for(uint16_t i=0; string[i]!='\0'; i++){
		uart_transmit(string[i]);
	}
}

int uart_receive_char() {
	UCSR0B = UCSR0B & ~(1 << RXCIE0);
	while (!(UCSR0A & (1 << RXC0))) {}
	char dat = UDR0;
	UCSR0B = UCSR0B | (1 << RXCIE0);
	return (int)dat;
}

/*int uart_receive(char* buff, uint16_t maxLen) {
	int cnt = 0;
	char last = '\0';
	if (bufferHead == bufferTail) return 0;
	do {
		if (bufferHead != bufferTail) {
		last = buffer[bufferHead];
		buff[cnt] = last;
		cnt++;
		bufferHead = (bufferHead + 1) % bufferLen;
		}
	} while (last != '\0' && cnt < maxLen);
	//buff[maxLen - 1] = '\0';
	return cnt;
}

ISR (USART0_RXC_vect) {
	char dat = UDR0;
	buffer[bufferTail] = dat;
	bufferTail = (bufferTail + 1) % bufferLen;
	if (bufferHead == bufferTail) {
		bufferHead = (bufferHead + 1) % bufferLen;
	}
}*/
