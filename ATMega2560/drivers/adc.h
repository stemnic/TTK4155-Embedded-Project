/*
 * adc.h
 *
 * Created: 11.10.2019 11:54:51
 *  Author: Ole Sivert
 */ 


#ifndef ADC_H_
#define ADC_H_

#define ADC_CH_15 0b100111

void adc_init();

uint16_t  adc_get_ir_conversion();

#endif /* ADC_H_ */