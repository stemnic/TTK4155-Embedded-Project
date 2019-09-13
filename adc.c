/*
 * adc.c
 *
 * Created: 13.09.2019 09:18:14
 *  Author: Ole Sivert
 */
#include "adc.h"

uint8_t waitingForADC = 0;
uint8_t adcBuffer = 0;

int16_t xMean;
int16_t yMean;

volatile char *ext_adc = (char *) 0x1400;

int8_t getJoystickValueInternal(int value, int raw) {
	ext_adc[0] = value == JOYSTICK_X ? ADC_CH1 : ADC_CH2;
	waitingForADC = 1;
	
	while (waitingForADC) _delay_us(1);
	
	if (raw) return adcBuffer;
	
	int16_t tempValue = adcBuffer - (value == JOYSTICK_X ? xMean : yMean);
	if (tempValue > 127) tempValue = 127;
	if (tempValue < -127) tempValue = -127;
	if (tempValue < 2 && tempValue > -2) tempValue = 0;
	return (int8_t)tempValue;
}

int8_t getJoystickValue(int value) {
	return getJoystickValueInternal(value, 0);
}

void calJoystick(){
	int xtotal = 0;
	int ytotal = 0;
	for (int i = 0; i < 100; i++) {
		xtotal += (uint8_t)getJoystickValueInternal(JOYSTICK_X, 1);
		ytotal += (uint8_t)getJoystickValueInternal(JOYSTICK_Y, 1);
	}

	xMean = xtotal / 100;
	yMean = ytotal / 100;
}

void ADC_init(){
	//Enables interrupt on falling flank on INT
	MCUCR |= (ISC10 << 0);
	//Enables INT0
	GICR |= (1 << 6);
	calJoystick();
}

uint8_t getSliderValue(int value) {
	ext_adc[0] = value == SLIDER_1 ? ADC_CH3 : ADC_CH4;
	waitingForADC = 1;
	
	while (waitingForADC) _delay_us(1);

	return adcBuffer;
}

ISR (INT0_vect){
	uint8_t adc_value = ext_adc[0];
	waitingForADC = 0;
	adcBuffer = adc_value;
}