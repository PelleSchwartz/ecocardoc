/*
 * control_LF.h
 *
 * Created: 21/4/2017
 * Author: Søren Møller Christensen
 * Andreas Stybe
 *
 */

#include "light_global.h"
#include "light_io.h"

#ifndef LIGHT_F_CONTROL_
#define LIGHT_F_CONTROL_

//OUTPUT and INPUT pins defined in light_io.h
#define WIPER	OA2
#define FAN		OB2
#define HORN	OA3
#define BRAKE	IN1 //input
#define LED_LIGHT_LEFT	OA1  // not sure if correct
#define LED_LIGHT_RIGHT	OA1	// not sure if correct
#define LED_SIGNAL_LEFT		OA4	// not sure if correct
#define LED_SIGNAL_RIGHT	OB1	// not sure if correct

void wiper(uint8_t wipe);
void fan(uint8_t fan);
void horn(uint8_t horn);
float readBrake(void);
void frontNormalLight(uint8_t nLight);
void frontLeftSignal(uint8_t lS);
void frontRightSignal(uint8_t rS);

#endif /* LIGHT_F_CONTROL_ */
