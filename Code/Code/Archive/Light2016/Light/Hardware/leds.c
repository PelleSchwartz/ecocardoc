/*
* leds.c
*
* Created: 01/5/2016
*  Author: Kim
*/
#include "../Components_Shared/global.h"

struct _LED {
	volatile uint8_t * const dataDirection;
	volatile uint8_t * const port;
	volatile uint8_t * const pin;
	uint8_t const portNr;
	};

/*
An array containing all the LED's information.
Their index correspond to their no. on the board.
*/
const static struct _LED _LEDs[]  = {
	{&DDRD, &PORTD, &PIND, 6}, //LED_0
	{&DDRD, &PORTD, &PIND, 7}, //LED_1
	{&DDRD, &PORTD, &PIND, 4}, //LED_2
	{&DDRD, &PORTD, &PIND, 5}, //LED_3
	{&DDRD, &PORTD, &PIND, 2}, //LED_4
	{&DDRD, &PORTD, &PIND, 3}, //LED_5
	
	{&DDRC, &PORTC, &PINC, 3}, //LED_6
	{&DDRC, &PORTC, &PINC, 2}, //LED_7
	{&DDRC, &PORTC, &PINC, 1}, //LED_8
	{&DDRC, &PORTC, &PINC, 0}, //LED_9
	
	{&DDRB, &PORTB, &PINB, 2}, //LED_10
	{&DDRB, &PORTB, &PINB, 0}, //LED_11
	
	{&DDRC, &PORTC, &PINC, 4}, //LED_12
	{&DDRC, &PORTC, &PINC, 5}, //LED_13
	
	{&DDRB, &PORTB, &PINB, 1}, //LED_14
};

void LEDnWriteMode(uint8_t n) {
	*(_LEDs[n].dataDirection) |= (1<<(_LEDs[n].portNr));
};

void LEDnReadMode(uint8_t n) {
	*(_LEDs[n].dataDirection) &= ~(1<<(_LEDs[n].portNr));
};

void LEDnOn(uint8_t n) {
	*(_LEDs[n].port) |= (1<<(_LEDs[n].portNr));
};

void LEDnOff(uint8_t n) {
	*(_LEDs[n].port) &= ~(1<<(_LEDs[n].portNr));
};

void LEDnToggle(uint8_t n) {
	*(_LEDs[n].port) ^= (1<<(_LEDs[n].portNr));
};

uint8_t LEDnRead(uint8_t n) {
	if(*(_LEDs[n].dataDirection) & (1<<(_LEDs[n].portNr))){
		return (*(_LEDs[n].port) & (1<<(_LEDs[n].portNr)))>>(_LEDs[n].portNr);
	}
	else{
		return (*(_LEDs[n].pin) & (1<<(_LEDs[n].portNr)))>>(_LEDs[n].portNr);
	}
};