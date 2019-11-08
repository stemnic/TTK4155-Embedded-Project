/*
 * eeprom.h
 *
 * Created: 08/11/2019 09:13:20
 *  Author: Einar
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void get_scores(uint8_t mode, uint16_t *buff);

void write_score(uint8_t mode, uint16_t *buff);

uint8_t update_scores(uint8_t mode, uint16_t score);

void wipe_scores();

#endif /* EEPROM_H_ */