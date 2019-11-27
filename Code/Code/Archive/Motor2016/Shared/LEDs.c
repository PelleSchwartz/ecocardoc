/*
* LEDs.c
*
* Created: 23-03-2015 10:37:38
*  Author: Kim
*/

#include "global.h"
#include "delay.h"

void LED_init(void) {
	// Set port L pin 7 and 6 as output for LED 1 and 2.
	SETBIT(DDRL, PL7);
	SETBIT(DDRL, PL6);
	
	// Set port C pin 0 and 1 as output for LED 3 and 4.
	SETBIT(DDRC, PC0);
	SETBIT(DDRC, PC1);
	
	// Set port D pin 6 and 7 as output for LED 5 and 6.
	SETBIT(DDRD, PD6);
	SETBIT(DDRD, PD7);
}

// LED 1
void LED1On(void) {
	PORTL |= (1 << PL7);
}
void LED1Off(void) {
	PORTL &= ~(1 << PL7);
}
void LED1Toggle(void) {
	PORTL ^= (1 << PL7);
}


// LED 2
void LED2On(void) {
	PORTL |= (1 << PL6);
}
void LED2Off(void) {
	PORTL &= ~(1 << PL6);
}
void LED2Toggle(void) {
	PORTL ^= (1 << PL6);
}

// LED 3
void LED3On(void) {
	PORTC |= (1 << PC0);
}
void LED3Off(void) {
	PORTC &= ~(1 << PC0);
}
void LED3Toggle(void) {
	PORTC ^= (1 << PC0);
}

// LED 4
void LED4On(void) {
	PORTC |= (1 << PC1);
}
void LED4Off(void) {
	PORTC &= ~(1 << PC1);
}
void LED4Toggle(void) {
	PORTC ^= (1 << PC1);
}

// LED 5
void LED5On(void) {
	PORTD |= (1 << PD6);
}
void LED5Off(void) {
	PORTD &= ~(1 << PD6);
}
void LED5Toggle(void) {
	PORTD ^= (1 << PD6);
}

// LED 6
void LED6On(void) {
	PORTD |= (1 << PD7);
}
void LED6Off(void) {
	PORTD &= ~(1 << PD7);
}
void LED6Toggle(void) {
	PORTD ^= (1 << PD7);
}

void LEDBlink(int LED, int times) {
	for(int i=0; i<times; i++) {
		switch(LED) {
			case 1:
			LED1On();
			delay_ms(200);
			LED1Off();
			delay_ms(200);
			break;
			case 2:
			LED2On();
			delay_ms(200);
			LED2Off();
			delay_ms(200);
			break;
			case 3:
			LED3On();
			delay_ms(200);
			LED3Off();
			delay_ms(200);
			break;
			case 4:
			LED4On();
			delay_ms(200);
			LED4Off();
			delay_ms(200);
			break;
			case 5:
			LED5On();
			delay_ms(200);
			LED5Off();
			delay_ms(200);
			break;
			case 6:
			LED6On();
			delay_ms(200);
			LED6Off();
			delay_ms(200);
			break;
		}
	}
}