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

void ADC_init();

int8_t getJoystickValue(int value);
uint8_t getSliderValue(int value);



#endif /* ADC_H_ */