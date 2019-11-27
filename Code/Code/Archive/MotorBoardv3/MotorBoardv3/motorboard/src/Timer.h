/*
 * Timer.h
 *
 * Created: 03-04-2015 19:00:27
 *  Author: Kim
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#define SEND_PERIOD 60 //ms
#include "adc.h"
// stolen from old code.
//
void timer_init(void);

void timer_init(void){
	// this code sets up timer1 for a 1ms  @ 16Mhz Clock (mode 4)
	OCR1A = 0x07CF;
	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR1A
	TIMSK1 |= (1 << OCIE1A); //output compare interrupt enabled.
	//Set interrupt on compare match
	TCCR1B |= (1 << CS11) ;
	// set prescaler to 8 and start the timer
}

ISR (TIMER1_COMPA_vect)
{
	static uint16_t counter = 0, jackCount = 0;
	counter++;
	jackCount++;
	BLINK_TIMER++;
	if(jackCount == SEND_PERIOD){
		jackUpdateFlag = 1;
		jackCount = 0;
	}
	if(counter==100){
		timer0_interupt_flag = 1;
	}
	if(counter==150){
		counter = 0;
	}
	if(BLINK_TIMER==500){
		BLINK_TIMER = 0;
		timer0_interupt_flag2 = 1;
	}
	// action to be done every 1 ms
}


#endif /* TIMER_H_ */