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
#include "light_global.h"
#define BRAKE_THRESHOLD 1// TODO: find threshold

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

void brakeLight(uint8_t brake) {
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
		LED_on(L_SIGNAL);
	} else {
		LED_off(L_SIGNAL);
	}
}

void backRightSignal(uint8_t rS) {
	if (rS) {
		LED_on(R_SIGNAL);
	} else {
		LED_off(R_SIGNAL);
	}
}
