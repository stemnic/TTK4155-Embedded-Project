/*
 * adc.c
 *
 * Created: 13.09.2019 09:18:14
 *  Author: Ole Sivert
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "adc.h"

uint8_t waitingForADC = 0;
uint8_t adcBuffer = 0;

int16_t xMean;
int16_t yMean;

volatile char *ext_adc = (char *) 0x1400;

/* Code to retrieve the value for JOYSTICK_X or JOYSTICK_Y.
If raw is 1, retrieve the raw value instead (used for calibration) */
int8_t get_joystick_value_internal(uint8_t value, uint8_t raw) {
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

/* Expose only the raw = 0 version of get_joystick_value_internal. */
int8_t get_joystick_value(uint8_t value) {
	return get_joystick_value_internal(value, 0);
}

/* Calculate the average value of the x and y positions of the joystick */
void calibrate_joystick() {
	int16_t xtotal = 0;
	int16_t ytotal = 0;
	for (uint8_t i = 0; i < 100; i++) {
		xtotal += get_joystick_value_internal(JOYSTICK_X, 1);
		ytotal += get_joystick_value_internal(JOYSTICK_Y, 1);
	}

	xMean = xtotal / 100;
	yMean = ytotal / 100;
}

void ADC_init() {
	//Enables interrupt on falling flank on INT
	MCUCR |= (ISC10 << 0);
	//Enables INT0
	GICR |= (1 << 6);
	calibrate_joystick();
}

/* Get the value of SLIDER_1 or SLIDER_2, as an 8-bit number
Writes CH3 or CH4 to memory-mapped IO, spins until an interrupt is received,
then retrieves the value stored in the interrupt */
uint8_t get_slider_value(uint8_t value) {
	ext_adc[0] = value == SLIDER_1 ? ADC_CH3 : ADC_CH4;
	waitingForADC = 1;
	
	while (waitingForADC) _delay_us(1);

	return adcBuffer;
}

/* Triggers on finished ADC conversion.
Simply buffers the read value from memory-mapped IO and clears a flag */
ISR (INT0_vect) {
	uint8_t adc_value = ext_adc[0];
	waitingForADC = 0;
	adcBuffer = adc_value;
}