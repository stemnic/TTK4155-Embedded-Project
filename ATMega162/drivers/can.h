/*
 * can.h
 *
 * Created: 27/09/2019 11:40:00
 *  Author: Einar
 */ 


#ifndef CAN_H_
#define CAN_H_

#define CAN_MODE_NORMAL 0
#define CAN_MODE_SLEEP 1
#define CAN_MODE_LOOPBACK 2
#define CAN_MODE_LISTEN 3
#define CAN_MODE_CONFIGURATION 4

#define CAN_TX0 2
#define CAN_TX1 3
#define CAN_TX2 4
#define CAN_RX0 0
#define CAN_RX1 1


typedef struct can_msg {
	uint16_t id;
	uint8_t *data;
	uint8_t dataLen;
} can_msg_t;

/* Sets the CAN device mode to one of NORMAL, SLEEP, LOOPBACK, LISTEN and CONFIGURATION */
void can_set_device_mode(uint8_t mode);
/* Initializes the CAN device, enabling interrupts on TX0, TX1, TX2, RX0, RX1 and ERR
This sets the device to CONFIGURATION, which will partially reset the device */
void can_init();
/* Send a can_msg on the first available buffer. If none are available, waits until one is free */
void can_send_data(can_msg_t *data);
/* Receives a can_msg, if none of the buffers are full, waits until one is */
void can_receive_blocking(can_msg_t *data);
/* Tries to receive a can_msg, returns 1 if a message was found in a buffer, and 0 otherwise, does not block */
uint8_t can_try_receive(can_msg_t *data);



#endif /* CAN_H_ */