/*
 * Light2017B.ino
 * Created: 4/22/2017 1:58:01 PM
 * Author: Andreas Stybe, Søren Møller Christensen
 *
 */

/*==========*/
/* Includes */
/*==========*/

// Header files in order of usage
#include "light_global.h"
#include "light_io.h"
#include "canbus.h"
#include "control_LB.h"

// the setup function runs once when you press reset or power the board
void setup() {
	light_io_init();
	can_init(LIGHT_BACK_ID);
}

// the loop function runs over and over again until power down or reset
void loop() {
	// Functions from control_LB.h - if the integer from getButtonStatus
	// (canlib) is high, the LED will turn on (or blink) - else it will turn off.
	starterLight(int(CAN.getMeasurement(RIO_STARTER)));
	brakeLight(int(CAN.getMeasurement(FRONT_BRAKE)));
	normalLight(CAN.getButtonStatus(STEERING_ID, BUTTON_LIGHT));
	leftSignal(CAN.getButtonStatus(STEERING_ID, BUTTON_BLINK_LEFT));
	rightSignal(CAN.getButtonStatus(STEERING_ID, BUTTON_BLINK_RIGHT));
}
