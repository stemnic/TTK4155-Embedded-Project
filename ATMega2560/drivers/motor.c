/*
 * motor.c
 *
 * Created: 18.10.2019 12:38:13
 *  Author: Ole Sivert
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "TWI_Master.h"
#include "motor.h"

int16_t motor_right_point = 0;
int16_t motor_left_point = 0;

void motor_init(){
	TWI_Master_Initialise();
	sei();
	
	motor_value(0);
	DDRH |= (1 << DIR_MOTOR_PIN) | (1 << nOE_ENCODER_PIN) | (1 << nRST_ENCODER_PIN) | (1 << SEL_H_L_ENCODER_PIN) | (1 << EN_MOTOR_PIN);
	PORTH |= (1 << nRST_ENCODER_PIN); // Active low reset
	PORTH |= (1 << EN_MOTOR_PIN); // Enable Motor
	
	DDRK |= 0x00;
	//PK0-7
	motor_calibrate();
}

void motor_calibrate(){
	motor_direction(MOTOR_RIGHT);
	motor_value(100);
	_delay_ms(1000);
	motor_encoder_reset();
	motor_value(0);
	motor_right_point = motor_encoder_value();
	
	motor_direction(MOTOR_LEFT);
	motor_value(100);
	_delay_ms(1000);
	motor_value(0);
	motor_left_point = motor_encoder_value();
	printf("Motor calibration L: %i, R: %i\n", motor_left_point, motor_right_point);
}

int16_t motor_encoder_value(){
	int16_t value = 0;
	PORTH &= ~(1 << nOE_ENCODER_PIN); // !OE low
	PORTH &= ~(1 << SEL_H_L_ENCODER_PIN); // Get MSB
	_delay_us(20);
	
	value |= (PINK << 8); //Reads MSB of 16bit counter
	PORTH |= (1 << SEL_H_L_ENCODER_PIN); // Get LSB
	_delay_us(20);
	
	value |= PINK;
	PORTH |= (1 << nOE_ENCODER_PIN); // !OE high
	
	return value;
}

void motor_encoder_reset(){
	PORTH &= ~(1 << nRST_ENCODER_PIN);
	_delay_us(10);
	PORTH |= (1 << nRST_ENCODER_PIN);
}

void motor_direction(uint8_t dir){
	
	if (dir) {
		PORTH |= (1 << DIR_MOTOR_PIN);
	} else {
		PORTH &= ~(1 << DIR_MOTOR_PIN);
	}
	
}

void motor_value(uint8_t value){
	unsigned char message[3] = { MOTOR_ADC_WRITE, 0, 0};
	if (value > 120) {
		value = 120;
	}
	message[2] = value;
	TWI_Start_Transceiver_With_Data(message, 3);
}

void motor_set_value(int8_t value) {
	if (value < 0) {
		motor_direction(MOTOR_LEFT);
	} else {
		motor_direction(MOTOR_RIGHT);
	}
	//printf("Set motor value: %i\n", abs(value));
	motor_value(abs(value)*2);
}

#define K 0.01
#define KI 0.05
#define KP 4

int16_t acc = 0;
int16_t target_pos = 4000;

void motor_set_target_pos(uint8_t _pos) {
	int16_t scale = (motor_left_point - motor_right_point)/255;
	target_pos = ((int16_t)_pos)*scale;
	//printf("New target pos: %i\n", target_pos);
}

void motor_regulator_tick() {
	int16_t pos = motor_encoder_value();
	//printf("Current pos: %i, target pos: %i\n", pos, target_pos);
	int16_t error = -(target_pos - pos);
	acc += K * error;
	int16_t regraw = (int16_t)(K*KP * ((float) error)) + (int16_t)(KI * ((float) acc));
	//printf("raw reg: %i, error: %i\n", regraw, error);
	int8_t regval = 0;
	if (regraw > 127) {
		regval = 127;
	} else if (regraw < -127) {
		regval = -127;
	} else {
		regval = (int8_t)regraw;
	}
	motor_set_value(regval);
}