/*
 * LightControl.c
 *
 * Created: 2016-05-16 12:32:59
 *  Author: kry
 */ 

#include "../Components_Shared/global.h"
#include "../Hardware/leds.h"
#include "../Hardware/pwm.h"
#include "PortControl.h"

#define LED_WIPER	14 // oc1a if PWM
#define LED_FAN		1
#define LED_HORN	12
#define LED_BRAKE	13 //in

#define LED_LIGHT_LEFT	4
#define LED_LIGHT_RIGHT	4

#define LED_SIGNAL_LEFT		3
#define LED_SIGNAL_RIGHT	5

void portControl_Initiate(void) {
	/* If wiper needs PWM */
	//oc1_init();
	//oc1a_activate();
	
	LEDnWriteMode(LED_WIPER);
	LEDnWriteMode(LED_FAN);
	LEDnWriteMode(LED_HORN);
	LEDnReadMode(LED_BRAKE);
	
	LEDnWriteMode(LED_LIGHT_LEFT);
	LEDnWriteMode(LED_LIGHT_RIGHT);
	
	LEDnWriteMode(LED_SIGNAL_LEFT);
	LEDnWriteMode(LED_SIGNAL_RIGHT);
}

void portControl_Wiper(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_WIPER);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_WIPER);
	}
}

void portControl_Fan(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_FAN);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_FAN);
	}
}

void portControl_Horn(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_HORN);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_HORN);
	}
}

uint8_t portControl_ReadBrake(void) {
	return (!LEDnRead(LED_BRAKE));
}

void portControl_NormalLight(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_LIGHT_LEFT);
		LEDnOn(LED_LIGHT_RIGHT);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_LIGHT_LEFT);
		LEDnOff(LED_LIGHT_RIGHT);
	}
}

void portControl_LeftSignal(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_SIGNAL_LEFT);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_SIGNAL_LEFT);
	}
}

void portControl_RightSignal(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_SIGNAL_RIGHT);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_SIGNAL_RIGHT);
	}
}