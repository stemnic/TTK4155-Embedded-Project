/*
 * spi.c
 *
 * Created: 27.09.2019 08:32:58
 *  Author: Ole Sivert
 */ 

#include "spi.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#define DDRB_SS 4
#define DDRB_MOSI 5
#define DDRB_MISO 6
#define DDRB_SCK 7

void spi_init(){
	/* Set MOSI, SS and SCK output, all others input */
	DDRB |= (1<<DDRB_MOSI)|(1<<DDRB_SCK)|(1<<DDRB_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPIE);
	//CPOL=1 and CPHA=1 for SPI mode 3, trying SPI mode 0 |(1<<CPOL)|(1<<CPHA)
	sei();
}

void spi_startcom(){
	PORTB &= ~(1<<DDRB_SS);
}

void spi_endcom(){
	PORTB |= (1<<DDRB_SS);
}

int dataLen = 0;
int current = 0;
char *buff;
int transdone = 0;

void spi_sendData(char *cData, int cDataLen){
	/* Start transmission */
	buff = cData;
	dataLen = cDataLen;
	current = 0;
	transdone = 0;
	SPDR = buff[0];
	spi_startcom();
	while (!transdone){
		_delay_us(1);
	}
	spi_endcom();
	printf("SPI Data:");
	for (int i = 0; i < cDataLen; i++) {
		printf(" %i", (uint8_t)buff[i]);
	}
	printf("\n");
}
/*	for(int i=0; i<cDataLen; i++){
		SPDR = cData;
		// Wait for transmission complete
		while(!(SPSR & (1<<SPIF)));
	}
}*/

ISR (SPI_STC_vect){
	buff[current] = SPDR;
		
	current++;
		
	if (current < dataLen) {
		SPDR = buff[current];
	} else {
		transdone = 1;
	}
}

