/*
 * spi.h
 *
 * Created: 27.09.2019 08:33:23
 *  Author: Ole Sivert
 */ 

#ifndef SPI_H_
#define SPI_H_

/* Initialize the SPI system on the ATMega. */
void spi_init();

/* Send and receive len bytes over SPI */
void spi_send_data(uint8_t *buff, uint8_t len);


#endif /* SPI_H_ */