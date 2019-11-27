/*
 * control_LF.B
 *
 * Created: 21/4/2017
 * Author: Søren Møller Christensen
 * Andreas Stybe
 *
 */

/*==========*/
/* Includes */
/*==========*/

#include "control_LF.h"
#include "light_global.h"

/*===========*/
/* Functions */
/*===========*/

void wiper(uint8_t wipe) {
	if (wipe) {
		LED_on(WIPER);
	} else {
		LED_off(WIPER);
	}
}

void fan(uint8_t fanc) {
	if (fanc) {
		LED_on(FAN);
	} else {
		LED_off(FAN);
	}
}

void horn(uint8_t horn) {
	if (horn) {
		LED_on(HORN);
	} else {
		LED_off(HORN);
	}
}

float readBrake(void) {
	float a = analogRead(BRAKE);
	return a;
}

void frontNormalLight(uint8_t nLight) {
	if (nLight) {
		LED_on(LED_LIGHT_LEFT);
		LED_on(LED_LIGHT_RIGHT);
	}
	else {
		LED_off(LED_LIGHT_LEFT);
		LED_off(LED_LIGHT_RIGHT);
	}
}

void frontLeftSignal(uint8_t lS) {
	if (lS) {
		LED_on(lS);
	}
	else {
		LED_off(lS);
	}
}

void frontRightSignal(uint8_t rS) {
	if (rS) {
		LED_on(rS);
	}
	else {
		LED_off(rS);
	}
}
*/
