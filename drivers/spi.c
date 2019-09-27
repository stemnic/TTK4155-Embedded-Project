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
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);//|(1<<SPIE);
	//CPOL=1 and CPHA=1 for SPI mode 3, trying SPI mode 0 |(1<<CPOL)|(1<<CPHA)
}

void spi_startcom(){
	PORTB &= ~(1<<DDRB_SS);
}

void spi_endcom(){
	PORTB |= (1<<DDRB_SS);
}


void spi_sendData(char *buff, int dataLen){
	/* Start transmission */
	int current = 0;
	SPDR = buff[0];
	spi_startcom();
	while (current < dataLen) {
		while (!(SPSR & (1 << SPIF))) _delay_us(1);
		buff[current] = SPDR;
			
		current++;
			
		if (current < dataLen) {
			SPDR = buff[current];
		}
	}
	spi_endcom();
	/*printf("SPI Data %i:", dataLen);
	for (int i = 0; i < dataLen; i++) {
		printf(" %i", (uint8_t)buff[i]);
	}
	printf("\n");*/
}

