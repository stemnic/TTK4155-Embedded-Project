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

//void spi_sendData(char *cData, int cDataLen);


uint8_t mcp_read(int mode, uint8_t addr){
	if (mode == MCP_MODE_CMD){
		char arr[3] = {0x03, addr};
		spi_send_data(arr, 3);
		return arr[2];
	}
	char arr[2] = {0x90|addr<<1};
	spi_send_data(arr, 2);
	return arr[1];
}

void mcp_read_array(int mode, uint8_t addr, uint8_t *arr, uint8_t len){
	if (mode == MCP_MODE_CMD) {
		char *buff = malloc(sizeof(char) * (len + 2));
		buff[0] = 0x03;
		buff[1] = addr;
		spi_send_data(buff, len + 2);
		for (int i = 0; i < len; i++) {
			arr[i] = buff[i+2];
		}
		free(buff);
		return;
	}
	char *buff = malloc(sizeof(char) * (len + 1));
	buff[0] = 0x90|addr<<1;
	spi_send_data(buff, len + 1);
	for (int i = 0; i < len; i++) {
		arr[i] = buff[i+1];
	}
	free(buff);
}

uint8_t mcp_read_status(){
	char arr[3] = {0xA0};
	spi_send_data(arr, 3);
	return arr[1];
}

uint8_t mcp_read_filter_status() {
	char arr[3] = {0xB0};
	spi_send_data(arr, 3);
	return arr[1];
}

void mcp_write(int mode, uint8_t addr, uint8_t data){
	if (mode == MCP_MODE_CMD){
		char arr[3] = {0x02, addr, data};
		spi_send_data(arr, 3);
		return;
	}
	char arr[2] = {0x80|addr, data};
	spi_send_data(arr, 2);
}

void mcp_write_array(int mode, uint8_t addr, uint8_t *arr, uint8_t len) {
	if (mode == MCP_MODE_CMD) {
		char *buff = malloc(sizeof(char) * (len + 2));
		for (int i = 0; i < len; i++) {
			buff[i+2] = arr[i];
		}
		buff[0] = 0x02;
		buff[1] = addr;
		spi_send_data(buff, len + 2);
		free(buff);
		return;
	}
	char *buff = malloc(sizeof(char) * (len + 1));
	for (int i = 0; i < len; i++) {
		buff[i+1] = arr[i];
	}
	buff[0] = 0x40|addr;
	spi_send_data(buff, len + 1);
	free(buff);
}

void mcp_rts(int tx_buffer){
	char command = 0x80|1<<tx_buffer;
	spi_send_data(&command, 1);
}
void mcp_reset(){
	char rst = 0xc0;
	spi_send_data(&rst, 1);
}
void mcp_bit_modify(uint8_t mask, uint8_t addr, uint8_t data){
	char arr[4]={0x05, addr, mask, data};
	spi_send_data(arr, 4);
}