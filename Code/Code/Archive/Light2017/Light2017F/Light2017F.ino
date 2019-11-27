/*
 * Light2017F.ino
 * Created:	4/24/2017 10:43:52 AM
 * Author: Søren Møller Christensen
 *
 */



#include "canbus.h"
#include "src/control_LF.h"

#define PRIORITY_BRAKE 70
#define CAN_FRONT_SYNC_PERIOD 50
#define WHEEL_DIAMETER 0.5588f		// [m]
#define NUMBER_OF_EDGES_PER_REVOLUTION 30
#define WHEEL_CIRCUMFERENCE WHEEL_DIAMETER * PI
#define WHEEL_SENSOR_PIN 23

#define speedUpdate 250



uint32_t canTiming = 0, frontSpeedUpdate = 0;
uint32_t raceBlinkTiming = 0;
volatile uint32_t wheelcount = 0;

void partyAnimation(void);
void normalMode();

// the setup function runs once when you press reset or power the board
void setup() {
	//initialize inputs/outputs
	light_io_init();
	// Set analog resolution
	analogReadResolution(16);
	//initialize can
	can_init(LIGHT_FRONT_ID);

	//wheelsensor
	//attachInterrupt(digitalPinToInterrupt(WHEEL_SENSOR_PIN), ISR_WHEEL, CHANGE);
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (CAN.getSystemState(STEERING_ID, BUTTON_HORN)) {
		raceBlinkTiming = millis();
		horn(1);
	}
	else {
		horn(0);
	}

	if (CAN.getSystemState(STEERING_ID, BUTTON_PARTY) || (millis() - raceBlinkTiming) < 2000) {
		partyAnimation();

	}
	else {
		normalMode();
	}

	/*if ((millis() - frontSpeedUpdate) > speedUpdate) {
		frontSpeedUpdate = millis();
		CAN.sendMeasurement(20, 0, MOTOR_SPEED, getSpeed());
	}*/

}


void partyAnimation(void) {
	static uint32_t timing1 = 0, timing2 = 0, timing3 = 0, timing4 = 0; //2 timers for being annoying
	static uint8_t led1, led2, hornVar;
	if ((millis() - timing1) >= 170) {
		timing1 = millis();
		led1 = !led1;
		if (led1) {
			LED_off(LED_LIGHT_LEFT);
			LED_off(LED_LIGHT_RIGHT);
		}
		else {
			LED_on(LED_LIGHT_LEFT);
			LED_on(LED_LIGHT_RIGHT);
		}
	}





	if ((millis() - timing2) >= 3000) {
		timing2 = millis();
		timing3 = millis();
	}
	/*if ((millis() - timing3) <= 300) {
		horn(1);
	}
	else {
		horn(0);
	}*/
}


void normalMode(void) {
	if ((millis() - canTiming) > CAN_FRONT_SYNC_PERIOD) {
		canTiming = millis();
		// Send brake voltage
		CAN.sendMeasurement(PRIORITY_BRAKE, VOLTAGE, FRONT_BRAKE, readBrake());
		//blink left
		frontLeftSignal(CAN.getSystemState(STEERING_ID, BUTTON_BLINK_LEFT));
		//blink right
		frontRightSignal(CAN.getSystemState(STEERING_ID, BUTTON_BLINK_RIGHT));
		//wiper
		wiper(CAN.getSystemState(STEERING_ID, BUTTON_WIPER));
		//fan
		fan(CAN.getSystemState(STEERING_ID, BUTTON_FAN));
		//horn (active low)
		if (CAN.goneToShit(STEERING_ID)) {
			horn(0);
		}
		else {
			if (!CAN.getSystemState(STEERING_ID, BUTTON_HIGHBEAM)) {
				horn(CAN.getSystemState(STEERING_ID, BUTTON_HORN));
			}
			else {
				horn(CAN.getSystemState(AUTO_ID, AUTO_HORN));
			}
		}
		//normal light
		frontNormalLight(CAN.getSystemState(STEERING_ID, BUTTON_LIGHT));
	}
}

//
//void ISR_WHEEL() {
//	wheelcount++;
//}
//
//float getDistance() {
//	return (wheelcount * WHEEL_CIRCUMFERENCE) / NUMBER_OF_EDGES_PER_REVOLUTION;
//}
//float getSpeed() {
//	static uint32_t lastDist = 0;
//	uint32_t delta = getDistance() - lastDist;
//	lastDist = getDistance();
//	return (delta * 1000 / speedUpdate)*3.6;
//}
