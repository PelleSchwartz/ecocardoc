

#include "inout.h"


//buttons in steeringwheel (extra) debounce...
volatile long blinkTiming = 0;

//bit 0-15 are used for the 16 buttons. Either on / off. Bit 16+17 is used for gear.. 0 = low gear, 1 = neutral, 2 = high gear. 
volatile uint32_t buttonState = 1 << BUTTON_AUTOGEAR; //default in Autogear.


volatile uint8_t buttonChange;





void io_init(void) {
	//pinmodes

		//for leds
	pinMode(21, OUTPUT);

	//buttons
	pinMode(BUTTON_FAN_pin, INPUT);
	pinMode(BUTTON_NEUTRAL_pin, INPUT);
	pinMode(BUTTON_LIGHT_pin, INPUT);
	pinMode(BUTTON_AUTOMODE_pin, INPUT);
	pinMode(BUTTON_RESET_pin, INPUT);
	pinMode(BUTTON_EKSTRA_pin, INPUT);
	pinMode(BUTTON_AUTOGEAR_pin, INPUT);
	pinMode(BUTTON_CL_pin, INPUT);
	pinMode(BUTTON_IDLE_pin, INPUT);
	pinMode(BUTTON_MODE_pin, INPUT);
	pinMode(BUTTON_PARTY_pin, INPUT);
	pinMode(BUTTON_WIPER_pin, INPUT);
	pinMode(BUTTON_BR_pin, INPUT);
	pinMode(BUTTON_BURN_pin, INPUT);
	pinMode(BUTTON_HORN_pin, INPUT);
	pinMode(BUTTON_BL_pin, INPUT);
	pinMode(BUTTON_GEARUP_pin, INPUT);
	pinMode(BUTTON_GEARDOWN_pin, INPUT);


}









void checkButtons() {


	//static unsigned char  StateGearUp = 1, StateGearDown = 1, StateNeutral = 1, StateGearAuto = 1, StateClosedLoop = 1, StateIdle = 1, StatePotUp = 1, StatePotDown = 1;


	//handles momentary press. Toggle handled in interrupt.

	//pot up/down should be handled in motor too. :(
		////pot up
		//if (!digitalRead(BUTTON_POTUP_pin)) {
		//	buttonState |= 1<<BUTTON_POTUP;
		//	
		//}
		//else {
		//	buttonState &= ~(1 << BUTTON_POTUP);
		//}

		////pot down
		//if (!digitalRead(BUTTON_POTDOWN_pin)) {
		//	buttonState |= 1<<BUTTON_POTDOWN;
		//}
		//else {
		//	buttonState &= ~(1<<BUTTON_POTDOWN);
		//}


		//gear up
	if (!digitalRead(BUTTON_GEARUP_pin) && !(buttonState & (1 << BUTTON_AUTOGEAR))) {
		buttonState |= (1 << BUTTON_GEARUP);
		buttonState &= ~(1 << BUTTON_NEUTRAL);
		buttonState &= ~(1 << BUTTON_GEARDOWN);
		//buttonState &= ~(1 << BUTTON_AUTOGEAR);
	}
	else if (!(buttonState & (1 << BUTTON_AUTOGEAR)))
	{
		buttonState &= ~(1 << BUTTON_GEARUP);
	}

	//gear down3
	if (!digitalRead(BUTTON_GEARDOWN_pin)) {
		buttonState |= (1 << BUTTON_GEARDOWN);
		buttonState &= ~(1 << BUTTON_NEUTRAL);
		buttonState &= ~(1 << BUTTON_GEARUP);
		//	buttonState &= ~(1 << BUTTON_AUTOGEAR);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_GEARDOWN);
	}


	//horn
	if (digitalRead(BUTTON_HORN_pin)) {
		buttonState |= (1 << BUTTON_HORN);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_HORN);
	}

	//burn
	if (digitalRead(BUTTON_BURN_pin)) {
		buttonState |= (1 << BUTTON_BURN);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_BURN);
	}




	//"HYOERSPEED" POWER!!!!
	//currently used for switching between displaymodes.
	if (!digitalRead(BUTTON_MODE_pin)) {
		buttonState |= (1 << BUTTON_HYPER);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_HYPER);
	}

}

void emergincySignal(void) {
	buttonState |= 1 << BUTTON_BLINK_LEFT;
	buttonState |= 1 << BUTTON_BLINK_RIGHT;
}

void ISR_BUTTON_0() {
	//fan
	buttonState ^= (1 << BUTTON_FAN);
	buttonChange = 1;

}

void ISR_BUTTON_1() {
	//neutral
	buttonState |= (1 << BUTTON_NEUTRAL);
	buttonState &= ~(1 << BUTTON_GEARUP);
	buttonState &= ~(1 << BUTTON_GEARDOWN);
	buttonState &= ~(1 << BUTTON_AUTOGEAR);

	buttonChange = 1;

}

void ISR_BUTTON_2() {
	//lightswitch
	buttonState ^= (1 << BUTTON_LIGHT);

	//if off, toggle high beam off
	if (!(buttonState)) {
		buttonState &= ~(1 << BUTTON_HIGHBEAM);
	}
	buttonChange = 1;

}

void ISR_BUTTON_3() {
	// "HIGHBEAM" (hijacked for autonomous use)
	buttonState ^= (1 << BUTTON_HIGHBEAM);
	buttonChange = 1;
}


void ISR_BUTTON_4() {
	//Reset
	buttonChange = 1;

	CAN.clearEmergency();

	//Reset timing
	resetTimer();
}

void ISR_BUTTON_5() {
	//Ekstra
	buttonChange = 1;

	//Reset display
	resetDisplay();
}

void ISR_BUTTON_6() {
	//Autogear. 
	buttonState ^= (1 << BUTTON_AUTOGEAR);
	buttonState &= ~(1 << BUTTON_GEARUP);
	buttonState &= ~(1 << BUTTON_GEARDOWN);
	buttonState &= ~(1 << BUTTON_NEUTRAL);

	/*
	int currentGear = (int)CAN.getMeasurement(SENSOR_GEAR);

	//if in automode
	if (buttonState & (1 << BUTTON_AUTOGEAR)) {
		buttonState &= ~(1 << BUTTON_NEUTRAL) & ~(1 << BUTTON_GEARUP) & ~(1 << BUTTON_GEARDOWN);
	}
	else
	{
		if (currentGear == 0) {
			buttonState &= ~(1 << BUTTON_GEARUP);
			buttonState &= ~(1 << BUTTON_GEARDOWN);
			buttonState |= (1 << BUTTON_NEUTRAL);
		}
		else {
			if (currentGear >= 2) {
				buttonState &= ~(1 << BUTTON_GEARDOWN);
				buttonState |= (1 << BUTTON_GEARUP);
			}
			else
			{
				buttonState |= BUTTON_GEARDOWN;
				buttonState &= ~(1 << BUTTON_GEARUP);
			}
		}
	}*/

	//Autogear logic handled in motorboard. 

	buttonChange = 1;

}

void ISR_BUTTON_7() {
	//closed loop
	//toggle
	buttonState ^= (1 << BUTTON_CL);
	buttonChange = 1;
}

void ISR_BUTTON_8() {
	//idle 
	//toggle in 2016 ?
	buttonState ^= (1 << BUTTON_IDLE);

	buttonState |= (1 << BUTTON_NEUTRAL);
	buttonState &= ~(1 << BUTTON_GEARUP);
	buttonState &= ~(1 << BUTTON_GEARDOWN);
	buttonState &= ~(1 << BUTTON_AUTOGEAR);

	buttonChange = 1;
}

void ISR_BUTTON_9() {
	//Mode
	buttonChange = 1;

	//display mode
	toggleMode();
}

void ISR_BUTTON_10() {
	//Flip buttonstate and trigger canbus publication
	//Party
	buttonState ^= (1 << BUTTON_PARTY);
	buttonChange = 1;
}

void ISR_BUTTON_11() {
	//wiper
	buttonState ^= (1L << BUTTON_WIPER);
	buttonChange = 1;
}



void ISR_BUTTON_24() {
	//horn
	buttonChange = 1;

}

void ISR_BUTTON_25() {
	//blink left
	if ((millis() - blinkTiming) > 300) {
		buttonState ^= (1 << BUTTON_BLINK_LEFT);
		buttonState &= ~(1 << BUTTON_BLINK_RIGHT);
		buttonChange = 1;
		blinkTiming = millis();
	}

}

void ISR_BUTTON_12() {


	//blink right
	if ((millis() - blinkTiming) > 300) {
		buttonState ^= (1 << BUTTON_BLINK_RIGHT);
		buttonState &= ~(1 << BUTTON_BLINK_LEFT);
		buttonChange = 1;
		blinkTiming = millis();
	}


}

void ISR_BUTTON_13() {

	//burn
	buttonChange = 1;
	startTimer();
}

void ISR_BUTTON_FR() {
	//gear up
	buttonChange = 1;
}

void ISR_BUTTON_FL() {
	//gear down.
	buttonChange = 1;
}


void io_interrupt_init(void) {
	attachInterrupt(digitalPinToInterrupt(BUTTON_FAN_pin), ISR_BUTTON_0, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_NEUTRAL_pin), ISR_BUTTON_1, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_LIGHT_pin), ISR_BUTTON_2, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_AUTOMODE_pin), ISR_BUTTON_3, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_RESET_pin), ISR_BUTTON_4, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_EKSTRA_pin), ISR_BUTTON_5, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_AUTOGEAR_pin), ISR_BUTTON_6, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_CL_pin), ISR_BUTTON_7, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_IDLE_pin), ISR_BUTTON_8, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_MODE_pin), ISR_BUTTON_9, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PARTY_pin), ISR_BUTTON_10, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_WIPER_pin), ISR_BUTTON_11, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_BR_pin), ISR_BUTTON_12, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_BURN_pin), ISR_BUTTON_13, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_HORN_pin), ISR_BUTTON_24, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_BL_pin), ISR_BUTTON_25, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_GEARUP_pin), ISR_BUTTON_FR, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_GEARDOWN_pin), ISR_BUTTON_FL, FALLING);
}

uint32_t getAllButtonState(void) {
	return buttonState;
}

uint8_t buttonsClicked(void) {
	buttonChange = 0;
	if (buttonChange) {

		return 1;
	}
	else {
		return 0;
	}
}

uint8_t getButtonState(uint8_t button) {
	return buttonState & (1 << button);
}




