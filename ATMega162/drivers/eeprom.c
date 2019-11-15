/*
 * eeprom.c
 *
 * Created: 08/11/2019 08:56:39
 *  Author: Einar
 */ 

#include <avr/eeprom.h>
#include "../graphics/simulator.h"
#include "eeprom.h"

void get_scores(uint8_t mode, uint16_t *buff) {
	uint8_t offset = mode == SIM_MODE_RUN ? 0 : 10;
	for (int i = 0; i < 5; i++) {
		buff[i] = eeprom_read_byte((uint8_t*)(i * 2 + offset));
		buff[i] |= ((uint16_t)eeprom_read_byte((uint8_t*)(i * 2 + offset + 1))) << 8;
	}
}

void write_score(uint8_t mode, uint16_t *buff) {
	uint8_t offset = mode == SIM_MODE_RUN ? 0 : 10;
	for (int i = 0; i < 5; i++) {
		eeprom_write_byte((uint8_t*)(i * 2 + offset), ((uint8_t)buff[i] & 0xFF));
		eeprom_write_byte((uint8_t*)(i * 2 + offset + 1), (uint8_t)((buff[i] >> 8) & 0xFF));
	}
}

uint8_t update_scores(uint8_t mode, uint16_t score) {
	uint16_t buff[5];
	get_scores(mode, buff);
	uint16_t temp = score;
	for (int i = 0; i < 5; i++) {
		if (temp > buff[i]) {
			uint16_t swap = buff[i];
			buff[i] = temp;
			temp = swap;
		}
	}
	write_score(mode, buff);
	return temp != score;
}

void wipe_scores() {
	for (int i = 0; i < 20; i++) {
		eeprom_write_byte((uint8_t*)i, 0);
	}
}