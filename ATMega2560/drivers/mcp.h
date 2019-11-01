/*
 * mcp2515.h
 *
 * Created: 27.09.2019 11:17:59
 *  Author: Ole Sivert
 */ 


#ifndef MCP_H_
#define MCP_H_

#define MCP_MODE_CMD 0
#define MCP_MODE_CAN 1

/* MCP_MODE_CMD does direct addressing using absolute addresses,
MCP_MODE_CAN directly accesses the data registers, and is more efficient */
/* Read a single byte from the given address */
uint8_t mcp_read(uint8_t mode, uint8_t addr);
/* Read the status using the READ STATUS command */
uint8_t mcp_read_status();
/* Read the filter status using the RX STATUS command */
uint8_t mcp_read_filter_status();
/* Read an array of data from len consecutive registers starting at addr */
void mcp_read_array(uint8_t mode, uint8_t addr, uint8_t *arr, uint8_t len);
/* Write to a single register */
void mcp_write(uint8_t mode, uint8_t addr, uint8_t data);
/* Write an array of data to len consecutive registers starting at addr */
void mcp_write_array(uint8_t mode, uint8_t addr, uint8_t *arr, uint8_t len);
/* Set the given buffer ready-to-send, more efficient than writing to the CTRL register. */
void mcp_rts(uint8_t tx_buffer);
/* Reset the device */
void mcp_reset();
/* Set the bits given by mask to values given in data in the register at addr */
void mcp_bit_modify(uint8_t mask, uint8_t addr, uint8_t data);


#endif /* MCP_H_ */