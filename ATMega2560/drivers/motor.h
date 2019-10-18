/*
 * motor.h
 *
 * Created: 18.10.2019 12:38:22
 *  Author: Ole Sivert
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

#define MOTOR_ADC_WRITE 0b01010000

#define DIR_MOTOR_PIN PH1
#define nOE_ENCODER_PIN PH5
#define nRST_ENCODER_PIN PH6
#define SEL_H_L_ENCODER_PIN PH3
#define EN_MOTOR_PIN PH4

#define MOTOR_RIGHT 1
#define MOTOR_LEFT 0

//typedef motor_direction 

void motor_init();

void motor_value(uint8_t value);

void motor_direction(uint8_t dir);

int16_t motor_encoder_value();

void motor_regulator_tick();

void motor_set_target_pos(uint8_t _pos);


#endif /* MOTOR_H_ */