/*
 * adc.h
 *
 * Created: 13.09.2019 09:18:27
 *  Author: Ole Sivert
 */ 

#define F_CPU 4915200

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#ifndef ADC_H_
#define ADC_H_

#define ADC_CH1 4
#define ADC_CH2 5
#define ADC_CH3 6
#define ADC_CH4 7

void ADC_init();
void getJoystickPositions(int8_t* x_value, int8_t* y_value);
void getSliderValues(uint8_t *slider_1, uint8_t *slider_2);



#endif /* ADC_H_ */