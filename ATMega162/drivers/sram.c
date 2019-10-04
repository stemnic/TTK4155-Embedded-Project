/*
 * sram.c
 *
 * Created: 06.09.2019 07:12:41
 *  Author: Ole Sivert
 */ 


#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sram.h"

void SRAM_init(){
	#ifdef DEBUG_LOG
	printf("Enabling EXTMEM\n");
	#endif // _DEBUG_LOG
	MCUCR |= (1<<SRE); //Enable EXTMEM
	SFIOR |= (1<<XMM2);
	
	//SFIOR |= (1<<XMBK); //Enable BusKeep

}

void SRAM_test(void){
	volatile char *ext_ram = (char *) 0x1800; 
	// Start address for the SRAM
	uint16_t ext_ram_size= 0x800;
	uint16_t write_errors= 0;
	uint16_t retrieval_errors= 0;
	#ifdef DEBUG_LOG
	printf("Starting SRAM test...\n");
	#endif // _DEBUG_LOG
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			#ifdef DEBUG_LOG
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			#endif // _DEBUG_LOG
			write_errors++;
		} else {
			//printf("Correct write match: ext_ram[%4d] = %02X\n", i, retreived_value);
		}
	}
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed); // reset the PRNG to the stateit had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			#ifdef DEBUG_LOG
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			#endif // _DEBUG_LOG
			retrieval_errors++;
		} else {
			//printf("Correct retrieval match: ext_ram[%4d] = %02X\n", i, retreived_value);
		}
	}
	#ifdef DEBUG_LOG
	printf("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
	#endif // _DEBUG_LOG
}
	