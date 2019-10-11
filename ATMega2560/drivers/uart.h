/*
 * uart.h
 *
 * Created: 30/08/2019 12:43:30
 *  Author: Einar
 */ 

#ifndef UART_H_
#define UART_H_
#define BAUD 9600

// Initialize USART driver
void uart_init();

// Transmit a single character over uart, busy-waiting until sent.
void uart_transmit(char data);

int uart_transmit_stdio(char data, FILE* file);

// Transmit a null-terminated string over uart, busy-waiting until sent.
void uart_print(const char string[]);

// Dequeue the last received 
//int uart_receive(char* buff, uint16_t maxLen);

int uart_receive_char();

#endif /* UART_H_ */