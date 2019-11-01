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

/* Initialize the motor */
void motor_init();

/* Set the value of the motor directly */
void motor_value(uint8_t value);

/* Set the direction of the motor, 0 is reverse */
void motor_direction(uint8_t dir);

/* Get the raw value of the motor encoder */
uint16_t motor_encoder_value();

/* Tick the motor, calculating new output and setting the motor */
void motor_regulator_tick();

/* Set the target position of the regulator */
void motor_set_target_pos(uint8_t _pos);


#endif /* MOTOR_H_ */