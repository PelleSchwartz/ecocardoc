/*
 * DutyCycle.h
 *
 * Created: 19/4/2015 19:08:04
 *  Author: Henning
 */ 


#ifndef DUTYCYCLE_H_
#define DUTYCYCLE_H_
#include "asf.h"

void SetPWMDutyGear(uint16_t duty);
void SetPWMDutyGearMicro(uint16_t micro);
void SetPWMDutySpeed(uint16_t duty);
void SetPWMDutySpeedMicro(uint16_t micro);
void pwm_init(void);

// Sets the duty cycle of the OC4A PWM between 0 and 511.  //no between 0 and 2499 /Kim
void SetPWMDutyGear(uint16_t duty) {
	OCR4A = duty;
}

void SetPWMDutyGearMicro(uint16_t micro) {
	uint16_t duty = micro/8;
	OCR4A = duty;
}

void SetPWMDutySpeed(uint16_t duty) {
	OCR5A = duty;
}

void SetPWMDutySpeedMicro(uint16_t micro) {
	uint16_t duty = micro/8;
	OCR5A = duty;
}

// Initializes the PWM Output on OC4A
void pwm_init(void) {
	DDRH |= (1<<DDH3); //gear servo
	ICR4 = 2500;		// Set PWM Frequency 50Hz
	TCCR4A = (1<<COM4A1)|(0<<COM4A0)|(0<<WGM41)|(0<<WGM40);			// PWM, Phase and Frequency Correct, TOP at ICR1
	TCCR4B = (1<<WGM43)|(0<<WGM42)|(0<<CS42)|(1<<CS41)|(1<<CS40);	// CS: clkIO/64
	
	DDRL |= (1<<DDL3); //speed servo
	ICR5 = 2500;		// Set PWM Frequency 50Hz
	TCCR5A = (1<<COM5A1)|(0<<COM5A0)|(0<<WGM51)|(0<<WGM50);			// PWM, Phase and Frequency Correct, TOP at ICR1
	TCCR5B = (1<<WGM53)|(0<<WGM52)|(0<<CS52)|(1<<CS51)|(1<<CS50);	// CS: clkIO/64
}



#endif /* DUTYCYCLE_H_ */