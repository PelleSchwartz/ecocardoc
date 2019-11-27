﻿/*
 * LEDs.c
 *
 * Created: 16-03-2015 14:31:28
 *  Author: Kim
 *
 *
 * Created for playing with the LED's.
 */ 

#ifndef LEDs_H_
#define LEDs_H_

void initiateLEDs(void);
void LED1On(void);
void LED2On(void);
void LED3On(void);
void LED4On(void);
void LED5On(void);
void LED6On(void);

void LED1Off(void);
void LED2Off(void);
void LED3Off(void);
void LED4Off(void);
void LED5Off(void);
void LED6Off(void);

void LED1Toggle(void);
void LED2Toggle(void);
void LED3Toggle(void);
void LED4Toggle(void);
void LED5Toggle(void);
void LED6Toggle(void);

#include "asf.h"


void initiateLEDs(void){
	//Set port L pin 7 and 6 as output for LED 1 and 2.
	DDRL |= 0xC0;
	
	//Set port C pin 0 and 1 as output for LED 3 and 4.
	DDRC |= 0x03;
	
	//Set port D pin 6 and 8 as output for LED 5 and 6.
	DDRD |= 0xC0;
}

//LED 1
void LED1On(void){
	PORTL |= (1 << 7);
}
void LED1Off(void){
	PORTL &= ~(1 << 7);
}
void LED1Toggle(void){
	PORTL ^= (1 << 7);
}


//LED 2
void LED2On(void){
	PORTL |= (1 << 6);
}
void LED2Off(void){
	PORTL &= ~(1 << 6);
}
void LED2Toggle(void){
	PORTL ^= (1 << 6);
}

//LED 3
void LED3On(void){
	PORTC |= (1 << 0);
}
void LED3Off(void){
	PORTC &= ~(1 << 0);
}
void LED3Toggle(void){
	PORTC ^= (1 << 0);
}

//LED 4
void LED4On(void){
	PORTC |= (1 << 1);
}
void LED4Off(void){
	PORTC &= ~(1 << 1);
}
void LED4Toggle(void){
	PORTC ^= (1 << 1);
}

//LED 5
void LED5On(void){
	PORTD |= (1 << 6);
}
void LED5Off(void){
	PORTD &= ~(1 << 6);
}
void LED5Toggle(void){
	PORTD ^= (1 << 6);
}

//LED 6
void LED6On(void){
	PORTD |= (1 << 7);
}
void LED6Off(void){
	PORTD &= ~(1 << 7);
}
void LED6Toggle(void){
	PORTD ^= (1 << 7);
}

#endif /* LEDs_H_ */