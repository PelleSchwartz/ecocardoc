/*
 * Timer.c
 *
 * Created: 2016-06-28 00:55:38
 *  Author: kry
 */ 
#include "../Components_Shared/global.h"

void initiateTimer(void){
	OCR1A = 0x07CF; //1 ms
	
	//Mode 4 CTC
	TCCR1A |= (1 << WGM11);
	//Enable interrupt
	TIMSK1 |= (1 << OCIE1A); //enable interrupt
	//Prescalar 8
	TCCR1B |= (1 << CS11) ;
}