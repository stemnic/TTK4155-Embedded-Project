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
#include <stdlib.h>
#include "TWI_Master.h"
#include "motor.h"

uint16_t motor_right_point = 0;
uint16_t motor_left_point = 0;
uint16_t motor_range = 0;
float motor_scale = 0;
int16_t acc = 0;
int16_t target_pos = 4000;

void motor_encoder_reset(){
	PORTH &= ~(1 << nRST_ENCODER_PIN);
	_delay_us(10);
	PORTH |= (1 << nRST_ENCODER_PIN);
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
	if (motor_left_point > motor_right_point) {
		motor_range = motor_left_point - motor_right_point;
	} else {
		motor_range = (1 << 15) - motor_right_point + motor_left_point;
	}
	motor_scale = motor_range / 256;
}

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



uint16_t motor_encoder_value(){
	uint16_t value = 0;
	PORTH &= ~(1 << nOE_ENCODER_PIN); // !OE low
	PORTH &= ~(1 << SEL_H_L_ENCODER_PIN); // Get MSB
	_delay_us(20);
	
	value |= (PINK << 8); //Reads MSB of 16bit counter
	PORTH |= (1 << SEL_H_L_ENCODER_PIN); // Get LSB
	_delay_us(20);
	
	value |= PINK;
	PORTH |= (1 << nOE_ENCODER_PIN); // !OE high
	
	// Is the value somehow out of range?
    if ((motor_left_point > motor_right_point && (value > motor_left_point || value < motor_right_point))
		|| (motor_left_point < motor_right_point && (value < motor_right_point && value > motor_left_point)))
    {
	    // Value is out of range on the right side
	    if (value > motor_left_point + motor_range
			|| (value < motor_right_point && (motor_right_point < motor_range || value > motor_right_point - motor_range)))
	    {
		    motor_right_point = value;
		    motor_left_point = ((motor_right_point + motor_range) % (1 << 15));
		    // Else out of range on the left side
	    }
	    else
	    {
		    // Safe subtraction, to make sure the value wraps around to 32k.
		    motor_left_point = value;
		    if (motor_left_point >= motor_range)
		    {
			    motor_right_point = (motor_left_point - motor_range);
		    }
		    else
		    {
			    motor_right_point = (1 << 15) - motor_range + motor_left_point;
		    }
	    }
    }
	// Return value normalized to be between 0 and motor_range
	if (motor_left_point > motor_right_point) {
		return value - motor_right_point;
	} else {
		if (value >= motor_right_point) {
			return value - motor_right_point;
		} else {
			return value + (1 << 15) - motor_right_point;
		}
	}
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



void motor_set_target_pos(uint8_t _pos) {
	target_pos = ((int16_t)_pos)*motor_scale;
	//printf("New target pos: %i\n", target_pos);
}

void motor_regulator_tick() {
	uint16_t pos = motor_encoder_value();
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