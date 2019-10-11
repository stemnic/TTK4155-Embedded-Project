/*
 * spi.c
 *
 * Created: 27.09.2019 08:32:58
 *  Author: Ole Sivert
 */ 
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include "spi.h"

#define DDRB_SS 4
#define DDRB_MOSI 5
#define DDRB_MISO 6
#define DDRB_SCK 7

void spi_init(){
	/* Set MOSI, SS and SCK output, all others input */
	DDRB |= (1<<DDRB_MOSI)|(1<<DDRB_SCK)|(1<<DDRB_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);//|(1<<SPIE);
	//CPOL=1 and CPHA=1 for SPI mode 3, trying SPI mode 0 |(1<<CPOL)|(1<<CPHA)
}

void spi_start_communication(){
	PORTB &= ~(1<<DDRB_SS);
}

void spi_end_communication(){
	PORTB |= (1<<DDRB_SS);
}


void spi_send_data(char *buff, int dataLen){
	/* Start transmission */
	int current = 0;
	SPDR = buff[0];
	
	spi_start_communication();

	while (current < dataLen) {
		while (!(SPSR & (1 << SPIF))) _delay_us(1);
		buff[current] = SPDR;
			
		current++;
			
		if (current < dataLen) {
			SPDR = buff[current];
		}
	}
	spi_end_communication();
}

