/*
 * can.c
 *
 * Created: 27/09/2019 11:49:26
 *  Author: Einar
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "can.h"
#include "mcp.h"

uint8_t int_trigger = 0;
uint8_t buffer_waiting = 3 << CAN_RX0;

#define CANINTE 0x2B

#define CANINTF 0x2C
#define CAN_ERR 7

void can_set_device_mode(uint8_t mode) {
	mcp_bit_modify(0xE0, 0x0F, mode << 5);
}

void can_init() {
	// Enable interrupts on any transmit buffer empty, or receive buffer full, as well as error during rec/transmission
	can_set_device_mode(CAN_MODE_CONFIGURATION);
	mcp_write(MCP_MODE_CMD, CANINTE, (1 << CAN_RX0)|(1 << CAN_RX1)|(1 << CAN_TX0)|(1 << CAN_TX1)|(1 << CAN_TX2)|(1 << CAN_ERR));
	// Enable interrupt 1 on falling edge
	
	//GICR |= (1<<INT1);
	//MCUCR |= (0b10 << 2);
	
	// Enable all messages on all buffers
	mcp_write(MCP_MODE_CMD, 0x60, 0x60);
	mcp_write(MCP_MODE_CMD, 0x70, 0x60);
	mcp_write(MCP_MODE_CMD, CANINTF, 0);
}

void wait_for_trigger(uint8_t mask) {
	while ((buffer_waiting & mask) == mask) {
		// Wait for interrupt pin to go low
		while (PIND & (1 << PD3)) _delay_us(1);
		// Read active interrupt pins
		volatile uint8_t int_status = mcp_read(MCP_MODE_CMD, CANINTF);
		// Clear interrupt pins
		mcp_write(MCP_MODE_CMD, CANINTF, 0);
		// Record interrupt status locally. buffer_wating indicates whether the buffer in question is empty or not.
		buffer_waiting &= ~int_status;
		int_trigger = 0;
		if (int_status & (1 << CAN_ERR)) {
			uint8_t errFlag = mcp_read(MCP_MODE_CMD, 0x2d);
			if (errFlag & (0b111 << 5)) {
				mcp_write(MCP_MODE_CMD, 0x2d, 0);
			}
			printf("Error sending message: %i\n", errFlag);
			return;
		}
	}
}

void can_send_data(can_msg_t *data) {
	wait_for_trigger(7 << CAN_TX0);
	
	uint8_t buffNum;
	uint8_t addr;
	if (!(buffer_waiting & (1 << CAN_TX0))) {
		buffNum = CAN_TX0;
		addr = 0;
	} else if (!(buffer_waiting & (1 << CAN_TX1))) {
		buffNum = CAN_TX1;
		addr = 2;
	} else {
		buffNum = CAN_TX2;
		addr = 4;
	}
	uint8_t raw[13];
	// Id high/low bits, 11 bit
	raw[0] = (uint8_t)(data->id >> 3);
	raw[1] = (uint8_t)(data->id << 5);
	// Extended identifier bits
	raw[2] = 0;
	raw[3] = 0;
	// data length
	if (data->dataLen > 8) data->dataLen = 8;
	raw[4] = data->dataLen & 0x0F;
	// data
	for (int i = 0; i < data->dataLen; i++) {
		raw[i+5] = data->data[i];
	}
	mcp_write_array(MCP_MODE_CAN, addr, raw, data->dataLen + 5);
	mcp_rts(buffNum - CAN_TX0);
	buffer_waiting |= 1 << buffNum;
}

uint8_t can_receive_data(can_msg_t *data, uint8_t block) {
	if (!block) {
		if (!(PIND & (1 << PD3))) {
			uint8_t int_status = mcp_read(MCP_MODE_CMD, CANINTF);
			mcp_write(MCP_MODE_CMD, CANINTF, 0);
			buffer_waiting &= ~int_status;
		}
		if ((buffer_waiting & (3 << CAN_RX0)) == (3 << CAN_RX0)) {
			return 0;
		}
	}
	
	wait_for_trigger(3 << CAN_RX0);

	uint8_t buffNum;
	uint8_t addr;
	if (!(buffer_waiting & (1 << CAN_RX0))) {
		buffNum = CAN_RX0;
		addr = 0;
	} else {
		buffNum = CAN_RX1;
		addr = 2;
	}

	uint8_t raw[13];
	mcp_read_array(MCP_MODE_CAN, addr, raw, data->dataLen + 5);
	buffer_waiting |= 1 << buffNum;
	
	data->id = ((uint16_t)raw[0]) << 3 | ((uint16_t)raw[1]) >> 5;
	for (int i = 0; i < data->dataLen; i++) {
		data->data[i] = raw[i+5];
	}
	return 1;
}

uint8_t can_receive_blocking(can_msg_t *data) {
	return can_receive_data(data, 1);
}

uint8_t can_try_receive(can_msg_t *data) {
	return can_receive_data(data, 0);
}

ISR (INT1_vect) {
	int_trigger = 1;
}