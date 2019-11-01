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

/* Code both to provide a graphical interface for the main game itself, and a simple "simulator" game,
with a bouncing ball that can be deflected, and a counter for score */

/* Update the actor */
void sim_update_actor_angle(uint8_t angle);
/* Simulator tick, updates state */
void sim_tick();
/* Trigger the simulated solenoid */
void sim_trigger_solenoid();
/* Update the position of the actor */
void sim_update_pos(uint8_t pos);
/* Init the simulator in given mode, SIM_MODE_SIMULATOR or SIM_MODE_RUN */
void sim_init(uint8_t mode);


#endif /* SIMULATOR_H_ */