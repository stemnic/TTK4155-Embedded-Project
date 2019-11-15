/*
 * mcp2515.c
 *
 * Created: 27.09.2019 11:18:14
 *  Author: Audun
 */ 

#include <stdlib.h>
#include <avr/io.h>
#include "mcp.h"
#include "spi.h"

/* Read a byte from the given address */
uint8_t mcp_read(uint8_t mode, uint8_t addr) {
	if (mode == MCP_MODE_CMD) {
		uint8_t arr[3] = {0x03, addr};
		spi_send_data(arr, 3);
		return arr[2];
	}
	uint8_t arr[2] = {0x90 | addr << 1};
	spi_send_data(arr, 2);
	return arr[1];
}

/* Read the value of len consecutive registers from the given address into the array.
Dynamically allocates extra memory to fit the address, for passing into the lower level spi_send_data */
void mcp_read_array(uint8_t mode, uint8_t addr, uint8_t *arr, uint8_t len) { 
	if (mode == MCP_MODE_CMD) {
		uint8_t *buff = malloc(len + 2);
		buff[0] = 0x03;
		buff[1] = addr;
		spi_send_data(buff, len + 2);
		for (int i = 0; i < len; i++) {
			arr[i] = buff[i + 2];
		}
		free(buff);
		return;
	}
	uint8_t *buff = malloc(len + 1);
	buff[0] = 0x90 | addr << 1;
	spi_send_data(buff, len + 1);
	for (int i = 0; i < len; i++) {
		arr[i] = buff[i + 1];
	}
	free(buff);
}

/* Get the current status using the dedicated status command
Note that it returns the same value twice */
uint8_t mcp_read_status() {
	uint8_t arr[3] = {0xA0};
	spi_send_data(arr, 3);
	return arr[1];
}

/* Gets current filter status using the dedicated status command
Also returns twice */
uint8_t mcp_read_filter_status() {
	uint8_t arr[3] = {0xB0};
	spi_send_data(arr, 3);
	return arr[1];
}

/* Writes a single byte of data to the given address */
void mcp_write(uint8_t mode, uint8_t addr, uint8_t data) {
	if (mode == MCP_MODE_CMD) {
		uint8_t arr[3] = {0x02, addr, data};
		spi_send_data(arr, 3);
		return;
	}
	uint8_t arr[2] = {0x80 | addr, data};
	spi_send_data(arr, 2);
}

/* Writes to a series of consecutive registers starting at the given addr
Dynamically allocates extra memory to fit the address and command, used in the lower level spi_send_data */
void mcp_write_array(uint8_t mode, uint8_t addr, uint8_t *arr, uint8_t len) {
	if (mode == MCP_MODE_CMD) {
		uint8_t *buff = malloc(len + 2);
		for (int i = 0; i < len; i++) {
			buff[i + 2] = arr[i];
		}
		buff[0] = 0x02;
		buff[1] = addr;
		spi_send_data(buff, len + 2);
		free(buff);
		return;
	}
	uint8_t *buff = malloc(len + 1);
	for (int i = 0; i < len; i++) {
		buff[i + 1] = arr[i];
	}
	buff[0] = 0x40 | addr;
	spi_send_data(buff, len + 1);
	free(buff);
}

/* Send Ready To Send for the given buffer */
void mcp_rts(uint8_t tx_buffer) {
	uint8_t command = 0x80 | 1 << tx_buffer;
	spi_send_data(&command, 1);
}
/* Reset the mcp device */
void mcp_reset() {
	uint8_t rst = 0xc0;
	spi_send_data(&rst, 1);
}
/* Set the bits given by mask to the values given in data for the given address */
void mcp_bit_modify(uint8_t mask, uint8_t addr, uint8_t data) {
	uint8_t arr[4] = {0x05, addr, mask, data};
	spi_send_data(arr, 4);
}