/*
 * pwm.h
 *
 * Created: 11.10.2019 08:41:03
 *  Author: Ole Sivert
 */ 


#ifndef PWM_H_
#define PWM_H_

/* Enable PWM */
void pwm_init();

/* Set the pwm value */
void pwm_set_value(uint8_t pos);

#endif /* PWM_H_ */