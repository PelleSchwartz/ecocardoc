/*
 * light_io.cpp
 *
 * Author: Håkon Westh-Hansen
 * Created: 20/4/2017
 *
 */

/*==========*/
/* Includes */
/*==========*/

#include "light_io.h"

/*=========*/
/* Methods */
/*=========*/

void LED_on(uint8_t LED) {
	digitalWrite(LED, HIGH);
}

void LED_off(uint8_t LED) {
	digitalWrite(LED, LOW);
}

void LED_toggle(uint8_t LED) {
	digitalWrite(LED, !digitalRead(LED));
}

// duty = 127 for 50% on time and 64 for 25%
void LED_blink(uint8_t LED, uint16_t duty) {
	analogWrite(LED, duty);
}

void light_io_init() {
	// Off board LEDs
	pinMode(OA1, OUTPUT);
	pinMode(OA2, OUTPUT);
	pinMode(OA3, OUTPUT);
	pinMode(OA4, OUTPUT);
	pinMode(OB1, OUTPUT);
	pinMode(OB2, OUTPUT);
	pinMode(OB3, OUTPUT);
	pinMode(OB4, OUTPUT);
	// On board test LED
	pinMode(testLED, OUTPUT);
	// Inputs
	pinMode(IN1, INPUT);
	pinMode(IN2, INPUT);
	// Make sure that everything is turned off!
	LED_off(OA1);
	LED_off(OB1);
	LED_off(OA2);
	LED_off(OB2);
	LED_off(OA3);
	LED_off(OB3);
	LED_off(OA4);
	LED_off(OB4);
}



void io_test() {
	int n = 0;
	uint8_t testArray[] = {
		OA1, OB1, OA2, OB2, OA3, OB3, OA4, OB4
	};

	LED_on(OA1);
	/*LED_on(OB1);
	LED_on(OA2);
	LED_on(OB2);
	LED_on(OA3);
	LED_on(OB3);
	LED_on(OA4);
	LED_on(OB4);*/

	while(1);

	// this loop never terminates
	while(1) {
		if (n == sizeof(testArray) / sizeof(testArray[0])) {
			n = 0;
		}
		if (n == 0) {
			LED_toggle(n - 1);
		}
		LED_toggle(n);
		LED_toggle(testLED);
		n++;
		delay(2000);
	}
}
