/*
 * buttons.h
 *
 * Created: 06-05-2015 20:47:50
 *  Author: Nicklas
 */ 

#ifndef BUTTONS_H_
#define BUTTONS_H_

//Button Reg, comments are for old code, before buton swaps
enum {
	BTN1,	//
	BTN2,
	BTN3,
	BTN4,	//Light
	BTN5,	//neutral
	BTN6,	//
	BTN7,	//Left blink
	BTN8,	//Right blink
	BTN9,	//Kers something
	BTN10,	//Kers something
	BTN11,	//Horn
	BTN12,	//Fan
	BTN13,	//wiper
	BTN14,	//unused
	BTN15,	//Start motor
	BTN16,	//Stop motor
	BTN17,	//gear 1
	BTN18,	//
	// Leave last untouched
	BUTTON_REG_SIZE
};

enum { //change here to swap button functionality.
	BTN_Pot_up		= BTN1,
	BTN_Pot_down	= BTN2,
	BTN_Light_3st	= BTN3,
	BTN_LightS		= BTN4,
	BTN_Neutral		= BTN5,
	BTN_Wiper		= BTN14,
	BTN_BlinkLeft	= BTN7,
	BTN_Horn		= BTN8,
	BTN_AutoGear	= BTN9,
	BTN_ClosedLoop	= BTN10,
	BTN_Idle		= BTN11,
	BTN_HYPERSPEED	= BTN12,
	BTN_PARTY_TIME	= BTN13,
	BTN_Fan			= BTN6,
	BTN_Start		= BTN15, //Burn?
	BTN_BlinkRight	= BTN16,
	BTN_Gear1		= BTN17,
	BTN_Gear2		= BTN18
};

#endif /* BUTTONS_H_ */