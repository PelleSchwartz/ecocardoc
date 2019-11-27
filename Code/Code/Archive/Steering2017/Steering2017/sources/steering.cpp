#include "steering.h"
//steering.cpp contains (for now) setuploop and main loop used in arduino IDE

//timing variables
long statusTiming = 0, displayTiming = 0;

uint8_t color = 0; 
void steeringSetup() {
	io_init();	
	io_interrupt_init();

	uint32_t baudRate = 1000000;
	can_init(STEERING_ID);

	LED_init();


	screen_init();

	delay(500);
}

void mainLoop() {

	//publish button state. This is done every 100 ms, or at buttonpress. Leds are updated too.
	if (buttonsClicked() || ((statusTiming - millis() ) > 100)) {
		statusTiming = millis();
		CAN.sendStatus(50, getAllButtonState() );
		checkButtons();
		//update array of leds based on button inputs/states + motor RPM
		updateLeds(CAN.getMeasurement(RIO_RPM_L));
	}

  Serial.println(CAN.getMeasurement(MOTOR_SPEED)/100000);

	if (CAN.newMeasure() || ((millis() - displayTiming) > 200)) {
    displayTiming = millis();
		loadValues(CAN.getMeasurement(MOTOR_SPEED), CAN.getMeasurement(RIO_WATER_TEMP), CAN.getMeasurement(RIO_OIL_TEMP), CAN.getMeasurement(MOTOR_DIST), CAN.getMeasurement(RIO_GEAR), CAN.getMeasurement(RIO_LAMBDA), CAN.getMeasurement(RIO_RPM_L));
	  updateDisplay();
	}
		
	
}


