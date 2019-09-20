/*
 * ui.h
 *
 * Created: 20/09/2019 12:33:24
 *  Author: Einar
 */ 


#ifndef UI_H_
#define UI_H_

void ui_list_select(int row);

void ui_list_init(char** options, uint8_t len);

void ui_buttons_init(char* str1, char* str2);

void ui_list_update(int8_t joystick_trigger);

void ui_button_trigger(uint8_t button, uint8_t on);

#endif /* UI_H_ */