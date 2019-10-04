/*
 * ATMega2560.c
 *
 * Created: 04/10/2019 10:13:13
 * Author : Einar
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "drivers/uart.h"
#include "drivers/spi.h"
#include "drivers/can.h"

int main(void)
{
	uart_init();
	fdevopen(uart_transmit_stdio, uart_receive_char);
	spi_init();
	can_init();
	
	can_set_device_mode(CAN_MODE_LOOPBACK);
    /* Replace with your application code */
	printf("Init Complete\n");
	uint8_t buffer[4] = { 1, 2, 3, 4 };
	can_msg_t message;
	message.data = buffer;
	message.dataLen = 4;
	message.id = 0x1FF;
	while(1) {
		printf("%i\n", mcp_read_status());
	}
	can_send_data(&message);
    while (1) 
    {
		printf("Waiting for data");
		can_receive_data(&message);
		printf("Receive msg with id: %i, len: %i\n", message.id, message.dataLen);
		for (int i = 0; i < message.dataLen; i++) {
			printf("  %i: %i", i, message.data[i]);
		}
		printf("\n");
		message.data[0]++;
		can_send_data(&message);
		printf("Can message was sent");
    }
}

