/*
 * Light2017F.ino
 * Created:	4/24/2017 10:43:52 AM
 * Author: Søren Møller Christensen
 *
 */


#include "light_global.h"
#include "light_io.h"
#include "canbus.h"
#include "control_LF.h"

#define PRIORITY_BRAKE 0

// the setup function runs once when you press reset or power the board
void setup() {
	light_io_init();
	can_init(LIGHT_FRONT_ID);
}

// the loop function runs over and over again until power down or reset
void loop() {
	//sendMeasurement(uint8_t priority, uint8_t measurementType, uint16_t sensorID, float data)
	CAN.sendMeasurement(PRIORITY_BRAKE, VOLTAGE, FRONT_BRAKE, readBrake());
	leftSignal(CAN.getButtonStatus(STEERING_ID, BUTTON_BLINK_LEFT));
	rightSignal(CAN.getButtonStatus(STEERING_ID, BUTTON_BLINK_RIGHT));
	wiper(CAN.getButtonStatus(STEERING_ID, BUTTON_WIPER));
	fan(CAN.getButtonStatus(STEERING_ID, BUTTON_FAN));
	horn(CAN.getButtonStatus(STEERING_ID, BUTTON_HORN));
	normalLight(CAN.getButtonStatus(STEERING_ID, BUTTON_LIGHT));
}
