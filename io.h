/*
 * io.h
 *
 * Created: 06.09.2019 10:13:01
 *  Author: Ole Sivert
 */ 


#ifndef IO_H_
#define IO_H_

#include <avr/io.h>

uint8_t joystickButton_Read();

uint8_t sliderButton1_Read();

uint8_t sliderButton2_Read();

#endif /* IO_H_ */