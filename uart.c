/*
 * uart.c
 *
 * Created: 30/08/2019 12:43:18
 *  Author: Einar
 */ 

#include "uart.h"
#define F_CPU 4915200
#define UBBR (F_CPU/16/BAUD-1)

char* buffer;
int bufferLen = 0;
int bufferHead = 0;
int bufferTail = 0;


void uart_init(char* _buffer, uint16_t len) {
	buffer = _buffer;
	bufferLen = len;
	/* Set baud rate */
	UBRR0H = (unsigned char)(UBBR >> 8);
	UBRR0L = (unsigned char)UBBR;
	/* Enable receiver and transmitter and rx flag*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
}

void uart_transmit(unsigned char data) {
	while (!(UCSR0A & (1<<UDRE0))) {}	//waits for ready
	UDR0=data;
}

void uart_print(const char string[]) {
	for(uint16_t i=0; string[i]!='\0'; i++){
		uart_transmit(string[i]);
	}
}

char uart_receive_char() {
	UCSR0B = UCSR0B & ~(1 << RXCIE0);
	while (!(UCSR0A & (1 << RXC0))) {}
	char dat = UDR0;
	UCSR0B = UCSR0B | (1 << RXCIE0);
	return dat;
}

int uart_receive(char* buff, uint16_t maxLen) {
	int cnt = 0;
	if (bufferHead == bufferTail) return 0;
	do {
		buff[cnt] = buffer[bufferHead];
		cnt++;
		bufferHead = (bufferHead + 1) % bufferLen;
	} while (bufferHead != bufferTail && buff[cnt-1] != '\0' && cnt < maxLen);
	buff[maxLen - 1] = '\0';
	return cnt;
}

ISR (USART0_RXC_vect) {
	char dat = UDR0;
	buffer[bufferTail] = dat;
	bufferTail = (bufferTail + 1) % bufferLen;
	if (bufferHead == bufferTail) {
		bufferHead = (bufferHead + 1) % bufferLen;
	}
}
