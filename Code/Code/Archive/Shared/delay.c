/*
 * delay.c
 *
 * Created: 11/06/2016 17:42:30
 *  Author: Henning
 */ 

#include "global.h"
#include <util/delay.h>

/* Delays */
void delay_ms(uint16_t count) {
	while(count--) {
		_delay_ms(1);
	}
}

void delay_us(uint16_t count) {
	while(count--) {
		_delay_us(1);
	}
}