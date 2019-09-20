/*
 * io.h
 *
 * Created: 06.09.2019 10:13:01
 *  Author: Ole Sivert
 */ 


#ifndef IO_H_
#define IO_H_

#include <avr/io.h>

#define BUTTON_1 1
#define BUTTON_2 2

uint8_t joystickButton_Read();

uint8_t sliderButton1_Read();

uint8_t sliderButton2_Read();

uint8_t getButton(int button);

#endif /* IO_H_ */