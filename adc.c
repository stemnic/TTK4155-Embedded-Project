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

void ADC_init(){
	//Enables interrupt on falling flank on INT
	MCUCR |= (ISC10 << 0);
	//Enables INT0
	GICR |= (1 << 6);
	calJoystick();
}

void calJoystick(){
	uint8_t xdata;
	uint8_t ydata;
	int xtotal = 0;
	int ytotal = 0;
	for (int i = 0; i < 100; i++) {
		readJoystick(&xdata, &ydata);
		xtotal += xdata;
		ytotal += ydata;
	}

	xMean = xtotal / 100;
	yMean = ytotal / 100;
}

void getJoystickPositions(int8_t* x_value, int8_t* y_value) {
	uint8_t rawX;
	uint8_t rawY;
	readJoystick(&rawX, &rawY);
	int16_t tempX = rawX - xMean;
	int16_t tempY = rawY - yMean;
	if (tempX > 127) tempX = 127;
	if (tempX < -127) tempX = -127;
	if (tempY > 127) tempY = 127;
	if (tempY < -127) tempY = -127;
	if (tempX < 2 && tempX > -2) tempX = 0;
	if (tempY < 2 && tempY > -2) tempY = 0;
	*x_value = (int8_t) tempX;
	*y_value = (int8_t) tempY;
}

void readJoystick(uint8_t* x_value, uint8_t* y_value) {
	volatile char *ext_adc = (char *) 0x1400;
	//makes the adc read CH1 (x axis)
	ext_adc[0] = ADC_CH1;
	waitingForADC = 1;
	while (waitingForADC)
	{
		//Dirty hack for wating for joystick values
		_delay_us(1);
	}
	*x_value = adcBuffer;
	ext_adc[0] = ADC_CH2;
	waitingForADC = 1;
	while (waitingForADC)
	{
		//Dirty hack for wating for joystick values
		_delay_us(1);
	}
	*y_value = adcBuffer;
}

void getSliderValues(uint8_t *slider_1, uint8_t *slider_2) {
	volatile char *ext_adc = (char *) 0x1400;
	ext_adc[0] = ADC_CH3;
	waitingForADC = 1;
	while (waitingForADC) {
		_delay_us(1);
	}
	*slider_1 = adcBuffer;

	ext_adc[0] = ADC_CH4;
	waitingForADC = 1;
	while (waitingForADC) {
		_delay_us(1);
	}
	*slider_2 = adcBuffer;
}

ISR (INT0_vect){
	volatile char *ext_adc = (char *) 0x1400;
	uint8_t adc_value = ext_adc[0];
	waitingForADC = 0;
	adcBuffer = adc_value;
}