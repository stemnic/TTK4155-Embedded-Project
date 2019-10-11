/*
 * ATMega2560.c
 *
 * Created: 04/10/2019 10:13:13
 * Author : Einar
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "drivers/uart.h"
#include "drivers/mcp.h"
#include "drivers/spi.h"
#include "drivers/can.h"
#include "drivers/pwm.h"

int main(void)
{
	uart_init();
	fdevopen(uart_transmit_stdio, uart_receive_char);
	spi_init();
	can_set_device_mode(CAN_MODE_CONFIGURATION);
	can_init();
	
	can_set_device_mode(CAN_MODE_NORMAL);
		
    // Replace with your application code 
	printf("Init Complete\n");
	uint8_t buffer[4];
	can_msg_t message;
	message.data = buffer;
	message.dataLen = 4;
	/*while(1) {
		printf("%i, %i\n", mcp_read(MCP_MODE_CMD, 0x0E), F_CPU);
		printf("Hei\n");
		_delay_ms(100);
	}*/
	/*while (1) {
		printf("CANSTAT: %i\n", mcp_read(MCP_MODE_CMD, 0x0E));
		_delay_ms(100);
	}*/
	//can_send_data(&message);
    /*while (1) 
    {
		printf("Waiting for data\n");
		can_receive_data(&message);
		printf("Receive msg with id: %i, len: %i\n", message.id, message.dataLen);
		for (int i = 0; i < message.dataLen; i++) {
			printf("  %i: %i", i, message.data[i]);
		}
		printf("\n");
		message.data[0]++;
		can_send_data(&message);
		printf("Can message was sent\n");
		//_delay_ms(1000);
    }*/
	pwm_init();
	while (1) {
		can_receive_data(&message);
		printf("x: %i, y: %i, button: %i, slider: %i\n", (int8_t)message.data[0], (int8_t)message.data[1], message.data[2], (uint8_t)message.data[3]);
		pwm_set_position(message.data[3]);
	}
}
