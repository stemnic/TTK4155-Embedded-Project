/*
 * io.c
 *
 * Created: 06.09.2019 10:13:09
 *  Author: Ole Sivert
 */ 

#include "io.h"

void SRAM_init(){
	printf("Enableing EXTMEM");
	MCUCR |= (1<<SRE); //Enable EXTMEM
	SFIOR |= (1<<XMM2);
	
	//SFIOR |= (1<<XMBK); //Enable BusKeep

}