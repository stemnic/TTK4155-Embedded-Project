/*
 * simulator.h
 *
 * Created: 18/10/2019 12:59:36
 *  Author: Einar
 */ 


#ifndef SIMULATOR_H_
#define SIMULATOR_H_

void sim_update_actor_angle(uint8_t angle);
void sim_tick();
void sim_trigger_solenoid();
void sim_update_pos(uint8_t pos);


#endif /* SIMULATOR_H_ */