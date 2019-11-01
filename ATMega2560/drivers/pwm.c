/*
 * pwm.c
 *
 * Created: 11.10.2019 08:40:49
 *  Author: Ole Sivert
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdfix.h>
#include <stdio.h>

uint16_t servo_postition_value = 3000;

/* Initialize the PWM driver
Sets 50Hz period, on a 16bit clock, in fast pwm mode */
void pwm_init(){
	
	PRR0 &= ~(1 << PRTIM1);
	
	DDRB |= (1 << PB6);
	TCCR1A |= (0 << COM1B0 | 1 << COM1B1 | 1 << WGM11 | 0 << WGM10);
	TCCR1B |= (0 << CS12 | 1 << CS11 | 0 << CS10 | 1 << WGM13 | 1 << WGM12);
	
	ICR1 = 0x9c3f; //Sets a 50Hz period  
	
	//OCR1B = 0x708; //Sets a 10% duty cycle
	
	OCR1B = servo_postition_value;
	
	sei();
	
	TIMSK1 = 0 << OCIE1B   /* Output Compare B Match Interrupt Enable: disabled */
	| 0 << OCIE1A /* Output Compare A Match Interrupt Enable: disabled */
	| 0 << ICIE1  /* Input Capture Interrupt Enable: disabled */
	| 1 << TOIE1; /* Overflow Interrupt Enable: enabled */
}

/* Set the position, scales by a pre-calculated value and offsets to give suitable results based on a value between 0 and 255. */
void pwm_set_value(uint8_t pos){
	float c = 9.375;
	servo_postition_value = 4200 - (((uint16_t)pos)*c);
}
/* The PWM module requires that we set the value to count to at each iteration,
updating right after reset is sensible in order to prevent data races */
ISR (TIMER1_OVF_vect){
	OCR1B = servo_postition_value;
}