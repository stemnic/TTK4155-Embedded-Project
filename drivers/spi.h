/*
 * spi.h
 *
 * Created: 27.09.2019 08:33:23
 *  Author: Ole Sivert
 */ 

#include <avr/io.h>

#ifndef SPI_H_
#define SPI_H_


void spi_init();

void spi_sendData(char *cData, int cDataLen);


#endif /* SPI_H_ */