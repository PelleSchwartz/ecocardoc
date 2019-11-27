// LEDs.h

#ifndef _LEDS_h
#define _LEDS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "FastLED.h"

#include "canbus.h"

#define NUM_LEDS 26
#define FADE_AMOUNT_BUTTONS 35
#define FADE_AMOUNT_RPM 50
#define FADE_AMOUNT_KNIGHT 45

void updateLeds(uint32_t buttons, uint16_t RPM);

void LED_init();

#endif
