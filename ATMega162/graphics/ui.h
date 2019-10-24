/*
 * ui.h
 *
 * Created: 20/09/2019 12:33:24
 *  Author: Einar
 */ 


#ifndef UI_H_
#define UI_H_

#define UI_MENU_MAIN 5
#define UI_MENU_SIM 0
#define UI_MENU_RUN 1

#include "../drivers/io.h"

void ui_list_select(int row);

void ui_list_init(char** options, uint8_t len);

void ui_buttons_init(char* str1, char* str2);

void ui_list_update(int8_t joystick_trigger);

void ui_button_trigger(uint8_t button, uint8_t on);

uint8_t get_list_pos();

void ui_draw_big_number(uint8_t num);

void ui_menu_update(controllerInput *_input);

void ui_simulator_update(controllerInput *_input, uint8_t pos);

#endif /* UI_H_ */