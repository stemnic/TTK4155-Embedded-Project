/*
 * mcp2515.h
 *
 * Created: 27.09.2019 11:17:59
 *  Author: Ole Sivert
 */ 


#ifndef MCP2515_H_
#define MCP2515_H_

#define MCP_MODE_CMD 0
#define MCP_MODE_CAN 1

uint8_t mcp_read(int mode, uint8_t addr);
uint8_t mcp_read_status();
void mcp_write(int mode, uint8_t addr, uint8_t data);
void mcp_rts();
void mcp_reset();
void mcp_bit_modify(uint8_t mask, uint8_t addr, uint8_t data);


#endif /* MCP2515_H_ */