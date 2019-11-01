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

/* Covers general user interface code, and the code for the main menu.
The main menu features a list of options centered between two big rackets, with an animated
bouncing ball between */

/* Select the element of the main-menu list at given index */
void ui_list_select(uint8_t row);
/* Init the ui-list with a given list of strings, with length len */
void ui_list_init(char** options, uint8_t len);
/* Draw two buttons at the bottom of the screen */
void ui_buttons_init(char* str1, char* str2);
/* Trigger the given button, drawing a box at the edge, currently unused */
void ui_list_update(int8_t joystick_trigger);
/* Update the list with the given value of joystick_trigger, 1 to increment selection, -1 to decrement */
void ui_button_trigger(uint8_t button, uint8_t on);
/* Return the current selected list index */
uint8_t get_list_pos();
/* Draw a big number in the UI, for debug */
void ui_draw_big_number(uint8_t num);
/* Update menu graphics with controller input object */
void ui_menu_update(controllerInput *_input);
/* Tick the simulator with given input and actor position */
void ui_simulator_update(controllerInput *_input, uint8_t pos);
/* Initialize the menu with a given list of menu entries of length len */
void ui_menu_init(char **list, uint8_t len);
/* Tick for the UI-menu, updates the ball graphic */
void ui_menu_tick();

#endif /* UI_H_ */