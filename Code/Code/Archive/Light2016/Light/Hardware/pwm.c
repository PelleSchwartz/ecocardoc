/*
 * pwm.c
 *
 * Created: 03/05/2016 22:07:20
 *  Author: Kim
 */ 

#include "../Components_Shared/global.h"

void oc1_init(void) {
	//COM1A = 10 : Clear OC1A and OC1B on match.
	//WGM1 = 1000 : ICR1 = max value.
	//cs1 = 011 : intern clock divided by 64.
	TCCR1A = (0<<WGM11)|(0<<WGM10);	
	TCCR1B = (1<<WGM13)|(0<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10);	
	
	ICR1 = 2500; //Frequency = 8MHz / (ICR1 * 64)
	//at least i think its 8MHz (internal clock not external)
	 
}

void oc1a_activate(void) {
	TCCR1A |= (1<<COM1A1)|(0<<COM1A0);
	DDRB |= (1<<DDB1);
}
void oc1b_activate(void) {
	TCCR1A |= (1<<COM1B1)|(0<<COM1B0);
	DDRB |= (1<<DDB2);
}

void oc1a_set_pwm(uint16_t duty){
	OCR1A = duty;
}

void oc1b_set_pwm(uint16_t duty){
	OCR1B = duty;
}