/*
 * can.h
 *
 * Created: 27/09/2019 11:40:00
 *  Author: Einar
 */ 


#ifndef CAN_H_
#define CAN_H_

#define F_CPU 4915200

#include <avr/io.h>
#include <util/delay.h>

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

void can_set_device_mode(uint8_t mode);
void can_init();
void can_send_data(can_msg_t *data, uint8_t buffNum);
void can_receive_data(can_msg_t *data);



#endif /* CAN_H_ */