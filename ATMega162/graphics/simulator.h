/*
 * simulator.h
 *
 * Created: 18/10/2019 12:59:36
 *  Author: Einar
 */ 


#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#define SIM_MODE_SIMULATOR 0
#define SIM_MODE_RUN 1

void sim_update_actor_angle(uint8_t angle);
void sim_tick();
void sim_trigger_solenoid();
void sim_update_pos(uint8_t pos);
void sim_init(uint8_t mode);


#endif /* SIMULATOR_H_ */