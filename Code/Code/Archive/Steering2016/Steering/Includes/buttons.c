/*
 * buttons.c
 *
 * Created: 23/06/2016 02:26:17
 *  Author: Henning
 */

#include "global.h"
#include "buttons.h"

volatile bool button_reg[BUTTON_REG_SIZE] = {0};

void update_buttons(void) {	
	button_reg[BTN1] = TESTBIT(PINB, PINB0);
	button_reg[BTN2] = TESTBIT(PINB, PINB1);
	button_reg[BTN3] = TESTBIT(PINB, PINB2);
	button_reg[BTN4] = TESTBIT(PINB, PINB3);

	button_reg[BTN17] = TESTBIT(PINC, PINC0);
	button_reg[BTN5] = TESTBIT(PINC, PINC1);
	button_reg[BTN6] = TESTBIT(PINC, PINC2);
	button_reg[BTN8] = TESTBIT(PINC, PINC3);
	button_reg[BTN7] = TESTBIT(PINC, PINC4);
	button_reg[BTN18] = TESTBIT(PINC, PINC5);
	button_reg[BTN16] = TESTBIT(PINC, PINC6);
	button_reg[BTN15] = TESTBIT(PINC, PINC7);

	button_reg[BTN9] = TESTBIT(PINA, PINA2);
	button_reg[BTN10] = TESTBIT(PINA, PINA3);
	button_reg[BTN11] = TESTBIT(PINA, PINA4);
	button_reg[BTN12] = TESTBIT(PINA, PINA5);
	button_reg[BTN13] = TESTBIT(PINA, PINA6);
	button_reg[BTN14] = TESTBIT(PINA, PINA7);
}

bool check_button(uint8_t button) {
	return button_reg[button];
}