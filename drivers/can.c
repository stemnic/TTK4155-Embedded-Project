/*
 * can.c
 *
 * Created: 27/09/2019 11:49:26
 *  Author: Einar
 */ 
#include "can.h"
#include "mcp.h"
#include <avr/interrupt.h>

uint8_t int_status;
uint8_t int_trigger = 0;

#define CANINTE 0x2B

#define CANINTF 0x2C
#define CAN_ERR 7

void can_set_device_mode(uint8_t mode) {
	mcp_bit_modify(0xE0, 0x0F, mode << 5);
}

void wait_for_interupt(uint8_t mask) {
	while (!(int_status & mask)) {
		while (!int_trigger) _delay_us(1);
		int_status = mcp_read(MCP_MODE_CMD, CANINTF);
		int_trigger = 0;
	}
}

void can_init() {
	// Enable interrupts on any transmit buffer empty, or receive buffer full, as well as error during rec/transmission
	mcp_write(MCP_MODE_CMD, CANINTE, (1 << CAN_RX0)|(1 << CAN_RX1)|(1 << CAN_TX0)|(1 << CAN_TX1)|(1 << CAN_TX2)|(1 << CAN_ERR));
	// Enable interrupt 1 on falling edge
	GICR |= (1<<INT1);
	MCUCR |= (0b10 << 2);
	// Enable all messages on all buffers
	mcp_write(MCP_MODE_CMD, 0x60, 0x60);
	mcp_write(MCP_MODE_CMD, 0x70, 0x60);
}

void can_send_data(can_msg_t *data, uint8_t buffNum) {
	uint8_t addr = 0;
	switch(buffNum) {
	case CAN_TX0:
		addr = 0;
		break;
	case CAN_TX1:
		addr = 2;
		break;
	case CAN_TX2:
		addr = 4;
		break;
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
	for (int i = 0; i < data->dataLen; i++) {
		raw[i+5] = data->data[i];
	}
	mcp_write_array(MCP_MODE_CAN, addr, raw, data->dataLen + 5);
	mcp_rts(buffNum - CAN_TX0);
	wait_for_interupt(1 << buffNum);
	mcp_bit_modify(1 << buffNum, CANINTF, 0);
	int_status &= ~(1 << buffNum);
}

void can_receive_data(can_msg_t *data) {
	wait_for_interupt(3 << CAN_RX0);
	uint8_t buffNum = (int_status & (1 << CAN_RX0)) ? CAN_RX0 : CAN_RX1;
	uint8_t addr = 0;
	switch (buffNum) {
	case CAN_RX0:
		addr = 0;
		break;
	case CAN_RX1:
		addr = 2;
		break;
	}

	uint8_t raw[13];
	mcp_read_array(MCP_MODE_CAN, addr, raw, data->dataLen + 5);
	int_status &= ~(1 << buffNum);
	
	data->id = ((uint16_t)raw[0]) << 3 | ((uint16_t)raw[1]) >> 5;
	for (int i = 0; i < data->dataLen; i++) {
		data->data[i] = raw[i+5];
	}
}

ISR (INT1_vect) {
	int_trigger = 1;
}