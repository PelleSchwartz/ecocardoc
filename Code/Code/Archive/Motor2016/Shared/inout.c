/*
* inout.c
*
* Created: 11/06/2016 18:20:15
*  Author: Henning
*/

#include "global.h"
#include "adc.h"
#include "inout.h"
#include "LEDs.h"

uint8_t numberOfEdgeInterruptsPerRevolution;
uint32_t magicWheelConstant;

void io_init(void) {
	// Starter as output
	SETBIT(DDRA, DDA4);
	setStarterLow();
	// Horn as output
	SETBIT(DDRA, DDA2);
	setHornLow();
	// Gear sensor as input
	CLRBIT(DDRF, DDF0);
}

void wheel_sensor_init(uint8_t numberOfMagnetsPerRevolution, uint32_t mMagicWheelConstant) {	
	numberOfEdgeInterruptsPerRevolution = numberOfMagnetsPerRevolution * 2 - 1;
	magicWheelConstant = mMagicWheelConstant;
	LED2Toggle();
	// Wheel sensor as input
	CLRBIT(DDRK, DDK1);
	// Enable Pin Change Interrupt on PCINT23:16 for wheel sensor
	PCICR |= (1 << PCIE2);
	// Set PCINT17 as trigger for the interrupt for the wheel sensor
	PCMSK2 |= (1 << PCINT17);
}

void setHornHigh(void) {
	SETBIT(PORTA, PA2);	// "Horn" on PA2 (Starter light on Innovator)
}
void setHornLow(void) {
	CLRBIT(PORTA, PA2);	// "Horn" on PA2 (Starter light on Innovator)
}

void setStarterHigh(void) {
	SETBIT(PORTA, PA4);	// Starter on PA4
}
void setStarterLow(void) {
	CLRBIT(PORTA, PA4);	// Starter on PA4
}

uint8_t digitalReadGSensor(void) {
	//return adc_read(0);
	return TESTBIT(PINF, PINF0);
}

// Local fields for wheel sensor
static volatile uint8_t wheelState = 0;
static volatile uint32_t wheelCount = 0;		// Accumulate 32-bit int
static volatile uint16_t wheelPeriod = UINT16_MAX;	// Last measurement 16 bit for RIO (mu sec)
static volatile uint32_t wheelTime = 0;

// Interrupt routine for PCI23:16 for wheel sensor
ISR(PCINT2_vect) {
	if(wheelState >= numberOfEdgeInterruptsPerRevolution) {
		LED2Toggle();
		wheelState = 0;
		wheelCount++;
		// Calculate period
		wheelPeriod = time - wheelTime;
		wheelTime = time;
	}
	else {
		wheelState++;
	}
}

uint16_t getWheelSensorPeriod(void) {
	return wheelPeriod;
}

uint32_t getDistanceCompleted(void) {	
	return (wheelCount * magicWheelConstant) / 10000;
}

// Sets the duty cycle of the OC4A PWM between 0 and 2499
void SetPWMDutyGear(uint16_t duty) {
	OCR4A = duty;
}

void SetPWMDutySpeed(uint16_t duty) {
	OCR5A = duty;
}

// Initializes the PWM Output on OC4A
void pwm_init(void) {
	DDRH |= (1<<DDH3); //gear servo
	ICR4 = 2500;		// Set PWM Frequency 50Hz
	TCCR4A = (1<<COM4A1)|(0<<COM4A0)|(0<<WGM41)|(0<<WGM40);			// PWM, Phase and Frequency Correct, TOP at ICR1
	TCCR4B = (1<<WGM43)|(0<<WGM42)|(0<<CS42)|(1<<CS41)|(1<<CS40);	// CS: clkIO/64
	
	// Speedservo is currently not used
	//DDRL |= (1<<DDL3); //speed servo
	//ICR5 = 2500;		// Set PWM Frequency 50Hz
	//TCCR5A = (1<<COM5A1)|(0<<COM5A0)|(0<<WGM51)|(0<<WGM50);			// PWM, Phase and Frequency Correct, TOP at ICR1
	//TCCR5B = (1<<WGM53)|(0<<WGM52)|(0<<CS52)|(1<<CS51)|(1<<CS50);	// CS: clkIO/64
}

