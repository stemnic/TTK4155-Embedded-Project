/*
 * adc.h
 *
 * Created: 13.09.2019 09:18:27
 *  Author: Ole Sivert
 */ 

#define F_CPU 4915200

#ifndef ADC_H_
#define ADC_H_

#define ADC_CH1 4
#define ADC_CH2 5
#define ADC_CH3 6
#define ADC_CH4 7

#define JOYSTICK_X 0
#define JOYSTICK_Y 1
#define SLIDER_1 0
#define SLIDER_2 1

/* Initialize the ADC:
Enable interrupt on falling flank on INT0
Calibrate the joystick to zero initial position. */
void ADC_init();

/* Retrieve the value of either JOYSTICK_X or JOYSTICK_Y, calibrated to fit within [-127, 127],
with a dead-zone of [2, -2] and calibrated so that neutral yields 0. */
int8_t get_joystick_value(uint8_t value);
/* Get the value of SLIDER_1 or SLIDER_2, as an 8-bit number */
uint8_t get_slider_value(uint8_t value);



#endif /* ADC_H_ */