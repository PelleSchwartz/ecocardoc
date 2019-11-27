/*
* time.c
*
* Created: 11/06/2016 18:02:38
*  Author: Henning
*/

#include "global.h"
#include "timer.h"
#include <avr/interrupt.h>

void timer_init(void) {
	// This code sets up Timer1 for a 1ms @ 16Mhz or 8MHz Clock (mode 4)
	if(F_CPU == 8000000UL) {
		OCR1A = 0x03EF;
		OCR1B = 0x03EF;
	}
	else {
		OCR1A = 0x07CF;
		OCR1B = 0x07CF;
	}
	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR1A
	TIMSK1 |= (1 << OCIE1B); // Output compare interrupt enabled.
	// Set interrupt on compare match
	TCCR1B |= (1 << CS11) ;
	// Set prescaler to 8 and start the timer
}

ISR(TIMER1_COMPB_vect) {
	// Action to be done every 1 ms
	time++;
}