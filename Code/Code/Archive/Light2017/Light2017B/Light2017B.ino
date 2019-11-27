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
#include "canbus.h"
#include "src/control_LB.h"


void normalLight();
void partyAnimation();

// the setup function runs once when you press reset or power the board
void setup() {
	light_io_init();
	can_init(LIGHT_BACK_ID);
	Serial.begin(9600);
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (CAN.getSystemState(STEERING_ID, BUTTON_PARTY)) { //
		partyAnimation();
	}
	else {
		normalLight();
	}

	
}

void normalLight() {
	// Functions from control_LB.h

	//starter light
	starterLight((CAN.getSystemState(MOTOR_ID, STATE_RIO_STARTER)));
	//starterLight(1);
	//brake light
	brakeLight((CAN.getMeasurement(MOTOR_BRAKE)));
	// normal light
	backNormalLight(CAN.getSystemState(STEERING_ID, BUTTON_LIGHT));


	//backNormalLight(1);
	//turn signals
	turnSignal(CAN.getSystemState(STEERING_ID, BUTTON_BLINK_RIGHT), CAN.getSystemState(STEERING_ID, BUTTON_BLINK_LEFT));


	/*Serial.print("starter: ");
	Serial.println(CAN.getSystemState(MOTOR_ID, STATE_RIO_STARTER));
	Serial.print("lights: ");
	Serial.println(CAN.getSystemState(STEERING_ID, BUTTON_LIGHT));
	Serial.print("blink: ");
	Serial.print(CAN.getSystemState(STEERING_ID, BUTTON_BLINK_LEFT));
	Serial.println(CAN.getSystemState(STEERING_ID, BUTTON_BLINK_RIGHT));

	Serial.print("autogear: ");
	Serial.println(CAN.getSystemState(STEERING_ID, BUTTON_AUTOGEAR));*/

}

void partyAnimation() {
	static uint32_t timing1, timing2;
	static uint8_t led1, led2;

	//brake light
	if ((millis() - timing1) >= 300) {
		timing1 = millis();
		led1 = !led1;
		if (led1) {
			LED_on(BRAKE_LIGHT);
		}
		else {
			LED_off(BRAKE_LIGHT);
		}
	}
	//turn signal
	if ((millis() - timing2) >= 100) {
		timing2 = millis();
		led2 = !led2;
		if (led2) {
			LED_off(L_SIGNAL);
			LED_on(R_SIGNAL);
			starterLight(1);
		}
		else {
			LED_off(R_SIGNAL);
			LED_on(L_SIGNAL);
			starterLight(0);
		}
		;
	}
};