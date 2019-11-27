/*
 * control_LB.cpp
 *
 * Created: 21/4/2017
 * Author: Søren Møller Christensen
 *
 */

/*==========*/
/* Includes */
/*==========*/

#include "control_LB.h"
#define BRAKE_THRESHOLD 7200// TODO: find threshold

//for timing
uint32_t blinkTiming = 0;

/*===========*/
/* Functions */
/*===========*/

void starterLight(uint8_t starter) {
	if (starter) {
		LED_on(L_STARTER);
		LED_on(R_STARTER);
	} else {
		LED_off(L_STARTER);
		LED_off(R_STARTER);
	}
}

void brakeLight(float brake) {
	if (BRAKE_THRESHOLD<=brake) {
		LED_on(BRAKE_LIGHT);
	} else {
		LED_off(BRAKE_LIGHT);
	}
}

void backNormalLight(uint8_t normal) {
	if (normal) {
		LED_on(BACK_LIGHT);
	} else {
		LED_off(BACK_LIGHT);
	}
}

void backLeftSignal(uint8_t lS) {

	if (lS) {
		if (millis() - blinkTiming > 600) {
			LED_toggle(L_SIGNAL);
		}
	}else {
		LED_off(L_SIGNAL);
	}
}

void backRightSignal(uint8_t rS) {

	if (rS) {
		if (millis() - blinkTiming > 600) {
			blinkTiming = millis();
			LED_toggle(R_SIGNAL);
		}
	} else {
		LED_off(R_SIGNAL);
	}
}

void turnSignal(uint8_t right, uint8_t left)
{
	if(!right){
		LED_off(R_SIGNAL);
	}
	if (!left) {
		LED_off(L_SIGNAL);
	}

	if(millis() - blinkTiming > 600 ){
		blinkTiming = millis();
		if(right){
			LED_toggle(R_SIGNAL);
		} 
		if (left) {
			LED_toggle(L_SIGNAL);
		}
	}
}
