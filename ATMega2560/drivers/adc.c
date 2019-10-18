/*
 * adc.c
 *
 * Created: 11.10.2019 11:54:58
 *  Author: Ole Sivert
 */ 

#include <avr/io.h>

void adc_init(){
	/* Enable clock to write ADC registers */
	PRR0 &= ~(1 << PRADC);

	ADMUX = (0x01 << REFS0)   /* AVCC with external capacitor at AREF pin */
	| (0 << ADLAR)    /* Left Adjust Result: disabled */
	| (0x00 << MUX0); /* ADC Single Ended Input pin 0 */

	ADCSRA = (1 << ADEN)        /* ADC: enabled */
	| (0 << ADATE)     /* Auto Trigger: disabled */
	| (0 << ADIE)      /* ADC Interrupt: disabled */
	| (0b111 << ADPS0); /* 2 */
	ADCSRB = (0x00 << ADTS0)    /* Free Running mode */
	| (0 << ACME)      /* Analog Comparator Multiplexer: disabled */
	| (0 << MUX5)
	;

	DIDR2 = 1 << ADC15D; /* Disable digital input buffer for ADC15 */
}


void adc_start_conversion(){
	//Implement possiblility to change channels in the future
	ADCSRA |= (1 << ADSC);
}


int adc_is_conversion_done(){
	return ((ADCSRA & (1 << ADIF)));
}


uint16_t  adc_get_conversion_result(void){
	return (ADCL | ADCH << 8);
}

uint16_t  adc_get_ir_conversion(){
	uint16_t  res;
	
	adc_start_conversion();
	while (!adc_is_conversion_done());
	res = adc_get_conversion_result();
	ADCSRA |= (1 << ADIF);
	return res;
}

uint8_t adc_get_resolution(){
	return 10;
}