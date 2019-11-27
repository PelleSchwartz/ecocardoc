/*
 * buttons.h
 *
 * Created: 06-05-2015 20:47:50
 *  Author: Nicklas
 */ 

#ifndef BUTTONS_H_
#define BUTTONS_H_

void update_buttons(void);
bool check_button(uint8_t button);

// Constants for buttons. Do NOT change this.
enum {
	BTN1,
	BTN2,
	BTN3,
	BTN4,
	BTN5,
	BTN6,
	BTN7,
	BTN8,
	BTN9,
	BTN10,
	BTN11,
	BTN12,
	BTN13,
	BTN14,
	BTN15,
	BTN16,
	BTN17,
	BTN18,
	// Leave last untouched
	BUTTON_REG_SIZE
};

enum { // Change assignments here to change functionality.
	BTN_POT_UP		= BTN1,
	BTN_POT_DOWN	= BTN2,
	BTN_LIGHT_HIGH_BEAM = BTN3,
	BTN_LIGHT_NORMAL = BTN4,
	BTN_NEUTRAL		= BTN5,
	BTN_WIPER		= BTN14,
	BTN_BLINK_LEFT	= BTN7,
	BTN_HORN		= BTN8,
	BTN_AUTOGEAR	= BTN9,
	BTN_CLOSED_LOOP	= BTN10,
	BTN_IDLE		= BTN11,
	BTN_HYPERSPEED	= BTN12,
	BTN_PARTY_TIME	= BTN13,
	BTN_FAN			= BTN6,
	BTN_BURN		= BTN15,
	BTN_BLINK_RIGHT	= BTN16,
	BTN_GEAR_DOWN	= BTN17,
	BTN_GEAR_UP		= BTN18
};

#endif /* BUTTONS_H_ */