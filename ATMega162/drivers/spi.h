/*
 * spi.h
 *
 * Created: 27.09.2019 08:33:23
 *  Author: Ole Sivert
 */ 

#ifndef SPI_H_
#define SPI_H_


void spi_init();

void spi_send_data(char *cData, int cDataLen);


#endif /* SPI_H_ */