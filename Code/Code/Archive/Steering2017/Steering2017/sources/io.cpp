

#include "io.h"

//bit 0-15 are used for the 16 buttons. Either on / off. Bit 16+17 is used for gear.. 0 = low gear, 1 = neutral, 2 = high gear. 
volatile uint32_t buttonState = 1 << 16; //default in neutral.


volatile uint8_t buttonChange;

CRGB leds[NUM_LEDS];



void io_init(void) {
//pinmodes

	//for leds
	pinMode(21, OUTPUT);

	//buttons
	pinMode(BUTTON_FAN_pin, INPUT);
	pinMode(BUTTON_NEUTRAL_pin, INPUT);
	pinMode(BUTTON_LIGHT_pin, INPUT);
	pinMode(BUTTON_HIGHBEAM_pin, INPUT);
	pinMode(BUTTON_POTUP_pin, INPUT);
	pinMode(BUTTON_POTDOWN_pin, INPUT);
	pinMode(BUTTON_AUTOGEAR_pin, INPUT);
	pinMode(BUTTON_CL_pin, INPUT);
	pinMode(BUTTON_IDLE_pin, INPUT);
	pinMode(BUTTON_HYPER_pin, INPUT);
	pinMode(BUTTON_PARTY_pin, INPUT);
	pinMode(BUTTON_WIPER_pin, INPUT);
	pinMode(BUTTON_12_pin, INPUT);
	pinMode(BUTTON_13_pin, INPUT);
	pinMode(BUTTON_24_pin, INPUT);
	pinMode(BUTTON_25_pin, INPUT);
	pinMode(BUTTON_GEARUP_pin, INPUT);
	pinMode(BUTTON_GEARDOWN_pin, INPUT);

	
}


void LED_init() {
	//FastLED init...
	FastLED.addLeds<NEOPIXEL, 21>(leds, NUM_LEDS);
}

void buttonToLED() {
	//temp. disable interrupt.
	cli();
	uint32_t temp = buttonState;
	sei();
	//set leds green if active
	for (uint8_t i = 0; i < 14; i++) {
		if ((temp & 1 << i)) {
			leds[i] = CRGB(0, 50, 0);
		}
		else {
			leds[i] = CRGB(50, 0, 0);
		}
	}
	for (uint8_t i = 24; i < 26; i++) {
		if ((temp & 1 << i)) {
			leds[i] = CRGB(0, 50, 0);
		}
		else {
			leds[i] = CRGB(50, 0, 0);
		}
	}


}

void RPMtoLED(uint16_t RPM) {
	//set rest of leds in a cool way B-)
}

void updateLeds(uint16_t RPM) {
	buttonToLED();
	RPMtoLED(RPM);
	FastLED.show();
}


void checkButtons() {


//static unsigned char  StateGearUp = 1, StateGearDown = 1, StateNeutral = 1, StateGearAuto = 1, StateClosedLoop = 1, StateIdle = 1, StatePotUp = 1, StatePotDown = 1;
	

//handles momentary press. Toggle handled in interrupt.

//pot up/down should be handled in motor too. :(
	//pot up
	if (!digitalRead(BUTTON_POTUP_pin)) {
		buttonState |= 1<<BUTTON_POTUP;
		
	}
	else {
		buttonState &= ~(1 << BUTTON_POTUP);
	}

	//pot down
	if (!digitalRead(BUTTON_POTDOWN_pin)) {
		buttonState |= 1<<BUTTON_POTDOWN;
	}
	else {
		buttonState &= ~(1<<BUTTON_POTDOWN);
	}
	

	//gear up
	if (!digitalRead(BUTTON_GEARUP_pin)) {
		buttonState |= (1 << BUTTON_GEARUP);
		buttonState &= ~(1 << BUTTON_NEUTRAL);
		buttonState &= ~(1 << BUTTON_GEARDOWN);
		buttonState &= ~(1 << BUTTON_AUTOGEAR);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_GEARUP);
	}

	//gear down
	if (!digitalRead(BUTTON_GEARDOWN_pin) && !(buttonState & (1<<BUTTON_GEARUP))) {
		buttonState |= (1 << BUTTON_GEARDOWN);
		buttonState &= ~(1 << BUTTON_NEUTRAL);
		buttonState &= ~(1 << BUTTON_GEARUP);
		buttonState &= ~(1 << BUTTON_AUTOGEAR);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_GEARDOWN);
	}


	//horn
	if (digitalRead(BUTTON_25_pin)) {
		buttonState |= (1 << BUTTON_HORN);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_HORN);
	}

	//burn
	if (digitalRead(BUTTON_12_pin)) {
		buttonState |= (1 << BUTTON_BURN);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_BURN);
	}

	//idle
	if (!digitalRead(BUTTON_IDLE_pin)) {
		buttonState |= (1 << BUTTON_IDLE);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_IDLE);
	}

	//"HYOERSPEED" POWER!!!!
	//currently used for switching between displaymodes.
	if (!digitalRead(BUTTON_HYPER_pin)) {
		buttonState |= (1 << BUTTON_HYPER);
	}
	else
	{
		buttonState &= ~(1 << BUTTON_HYPER);
	}

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
	if (  !(buttonState & (1 << BUTTON_LIGHT))) {
		buttonState &= ~(1 << BUTTON_HIGHBEAM);
	}
	buttonChange = 1;

}

void ISR_BUTTON_3() {
	//HIGHBEAM
	//if light is on
	if (buttonState & (1<<BUTTON_LIGHT))
	{
		buttonState ^= (1 << BUTTON_HIGHBEAM);
	}
	buttonChange = 1;

}


void ISR_BUTTON_4() {
	//pot up -> this isn't a toggle?
	//Steering 2016: single pulse
	buttonChange = 1;

}

void ISR_BUTTON_5() {
	//pot down -> this isn't a toggle?
	//Steering 2016: single pulse
	buttonChange = 1;

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
	buttonChange = 1;
}

void ISR_BUTTON_9() {
	//hyperspeed


	//buttonState ^= (1 << BUTTON_HYPER);
	buttonChange = 1;

	//display mode
	toggleMode();
}

void ISR_BUTTON_10() {
	//Flip buttonstate and trigger canbus publication

	buttonState ^= (1 << BUTTON_PARTY);
	buttonChange = 1;
}

void ISR_BUTTON_11() {
	//wiper
	buttonState ^= (1 << BUTTON_WIPER);
	buttonChange = 1;
}

void ISR_BUTTON_24() {
	//blink left
	buttonState ^= (1 << BUTTON_BLINK_RIGHT);
	buttonChange = 1;

}

void ISR_BUTTON_25() {
	//horn
	buttonChange = 1;
}

void ISR_BUTTON_12() {
	//burn
	buttonChange = 1;
}

void ISR_BUTTON_13() {
	//blink right
	buttonState ^= (1 << BUTTON_BLINK_RIGHT);
	buttonChange = 1;
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
	attachInterrupt(digitalPinToInterrupt(BUTTON_HIGHBEAM_pin), ISR_BUTTON_3, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_POTUP_pin), ISR_BUTTON_4, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_POTDOWN_pin), ISR_BUTTON_5, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_AUTOGEAR_pin), ISR_BUTTON_6, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_CL_pin), ISR_BUTTON_7, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_IDLE_pin), ISR_BUTTON_8, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_HYPER_pin), ISR_BUTTON_9, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PARTY_pin), ISR_BUTTON_10, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_WIPER_pin), ISR_BUTTON_11, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_12_pin), ISR_BUTTON_12, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_13_pin), ISR_BUTTON_13, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_24_pin), ISR_BUTTON_24, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_25_pin), ISR_BUTTON_25, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_GEARUP_pin), ISR_BUTTON_FR, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_GEARDOWN_pin), ISR_BUTTON_FL, FALLING);
}

uint32_t getAllButtonState(void) {
	return buttonState;
}

uint8_t buttonsClicked(void) {
	if (buttonChange) {
		buttonChange = 0;
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t getButtonState(uint8_t button) {
	return buttonState & (1<<button);
}

