#include "sources/Screen.h"
#include <tunes.h>
#include "canbus.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_TFTLCD.h" // Hardware-specific library
#include "sources/inout.h"
#include "FlexCAN.h"
#include "sources/LEDs.h"

#define BUZZER_PIN 29

uint32_t statusTiming = 0, displayTiming = 0;
float temp_speed;
uint32_t temp_millis;

void setup() {
	io_init();
	io_interrupt_init();
	Serial.println("B");		// TODO Why is this required. Fix it!
	screen_init();
	Serial.println("A");	// TODO
	can_init(STEERING_ID);
	LED_init();
	Serial.println("C");	// TODO
	tunes_init(BUZZER_PIN);
	sing(BILLIE_JEAN_1_ID);
	delay(100);
}

void loop() {
	//publish button state. This is done every 50 ms, or at buttonpress. Leds are updated too.
	if (((millis() - statusTiming) > 50) || buttonsClicked()) {
		statusTiming = millis();
		CAN.sendStatus(50, getAllButtonState());
		checkButtons();
		//update array of leds based on button inputs/states + motor RPM
		updateLeds(getAllButtonState(), CAN.getMeasurement(RIO_RPM));
	}

	/*
		DEBUG ONLY
	*/

	//timed display update.
	if (((millis() - displayTiming) > 200)) {
		displayTiming = millis();

		float newSpeed = CAN.getMeasurement(MOTOR_SPEED);
		float newWaterTemp = CAN.getMeasurement(RIO_WATER_TEMP);
		float newOilTemp = CAN.getMeasurement(RIO_OIL_TEMP);
		float newDist = CAN.getMeasurement(MOTOR_DISTANCE);
		float newGear = CAN.getMeasurement(RIO_GEAR);
		float newLambda = CAN.getMeasurement(RIO_LAMBDA);
		float newRpm = CAN.getMeasurement(RIO_RPM);
		float newBatteryVoltage = CAN.getMeasurement(MOTOR_BAT_VOLTAGE);
		float newKmL = CAN.getMeasurement(RIO_KMPERL);
		uint8_t newError = CAN.checkEmergency();
		loadValues(
			(int) (newSpeed),
			newWaterTemp,
			newOilTemp,
			newDist,
			newGear,
			newLambda,
			newRpm,
			newBatteryVoltage,
			newKmL,
			newError
		);


		loadAutoState(getButtonState(BUTTON_HIGHBEAM));


		//DEBUG ONLY
//	loadValues(CAN.getMeasurement(RIO_KMH), CAN.getMeasurement(RIO_WATER_TEMP), CAN.getMeasurement(RIO_OIL_TEMP), CAN.getMeasurement(MOTOR_DISTANCE), 3, 0, 0, 15, 650, 1);
		updateDisplay();
	}


	//DEBUG ONLY
	if (tunes_is_ready()) {
		//sing(MARIO_MAIN_MELODY_ID);
	}

	//if (CAN.checkEmergency()) {
	//	emergincySignal(); //this just triggers blink left + right
	//}


	//partymode WUHU
	if (CAN.getSystemState(STEERING_ID, BUTTON_PARTY)) {
		if (tunes_is_ready()) {
			sing(SAX_MELODY_ID);
		}
	}

}
