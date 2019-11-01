/*
 * sram.h
 *
 * Created: 06.09.2019 07:12:52
 *  Author: Ole Sivert
 */ 


#ifndef SRAM_H_
#define SRAM_H_

/* Test the consistency of sending/receiving data from external memory */
void SRAM_test(void);

/* Enable external memory, this is also required for memory-mapped IO */
void SRAM_init();


#endif /* SRAM_H_ */