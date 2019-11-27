// 
// 
// 

#include "LEDs.h"


CRGB leds[NUM_LEDS];

//function prototypes
void RPMtoLED(uint16_t RPM);
void buttonToLED(uint32_t temp);
void RPManimation(uint16_t RPM);
void RPManimation();
CRGB blendColors(CRGB currentColor, CRGB targetColor, uint8_t fade);

#define DEFAULT_BUTTON_COLOR CRGB(51, 102, 204)
#define DEFAULT_PRESSED_COLOR CRGB(255, 0, 0)

void updateLeds(uint32_t buttons, uint16_t RPM) {
	buttonToLED(buttons);
	RPMtoLED(RPM);
	FastLED.show();
}

void LED_init() {
	//FastLED init...
	FastLED.addLeds<NEOPIXEL, 21>(leds, NUM_LEDS);
}

void buttonToLED(uint32_t temp) {

	//timing for left/right blink
	static uint32_t timingBlink = 0;


	for (uint8_t i = 0; i < 14; i++) {
		if ((temp & 1 << i)) {
			if (i != BUTTON_BLINK_RIGHT)
			{
				leds[i] = blendColors(leds[i],  DEFAULT_PRESSED_COLOR, FADE_AMOUNT_BUTTONS); //
			}
			else {
				leds[i] = blendColors(leds[i], DEFAULT_BUTTON_COLOR, FADE_AMOUNT_BUTTONS);
			}

		}
		else {
			leds[i] = blendColors(leds[i], DEFAULT_BUTTON_COLOR, FADE_AMOUNT_BUTTONS);
		}

	}
	//button horn color 
	if (temp & 1 << BUTTON_HORN) {
		leds[24] = blendColors(leds[24], DEFAULT_PRESSED_COLOR, FADE_AMOUNT_BUTTONS);
	}
	else {
		leds[24] = blendColors(leds[24], DEFAULT_BUTTON_COLOR, FADE_AMOUNT_BUTTONS);
	}

	//button blink left
	leds[25] = blendColors(leds[25], DEFAULT_BUTTON_COLOR, FADE_AMOUNT_BUTTONS);




	//turnsignals/
	//instant on, fades out.. 
	if (millis() - timingBlink > 600)
	{

		timingBlink = millis();
		//turnsignal, right
		if (temp & (1 << BUTTON_BLINK_RIGHT)) {
			for (uint8_t i = 8; i <= 12; i++) {
				leds[i] = CRGB(255, 255, 0); //yellow
			}
		}
		//turnsignal, left
		if (temp & (1 << BUTTON_BLINK_LEFT)) {
			for (uint8_t i = 0; i <= 3; i++) {
				leds[i] = CRGB(255, 255, 0);
			}
			leds[25] = CRGB(255, 255, 0);
			//leds[26] = CRGB(255, 255, 0);
		}
	}
}


void RPMtoLED(uint16_t RPM) {
	//set rest of leds in a cool way B-)
	if (RPM == 0) {
		RPManimation();
	}
	else
	{
		RPManimation(RPM);
	}
}

void RPManimation(uint16_t RPM) {

	int16_t totalBrightness = (10 * 255 * RPM) / 5000;
	int hue = 100;


	for (int i = 23; i >= 14; i--)
	{
		if (totalBrightness > 255)
		{
			leds[i] = CHSV(hue, 255, 255);
		}
		else if (totalBrightness > 0) {
			leds[i] = CHSV(hue, 255, totalBrightness);
		}
		else {
			leds[i] = blendColors(leds[i], CRGB(0, 0, 0), FADE_AMOUNT_RPM);
			//leds[i] = CRGB(0, 0, 0);
		}

		totalBrightness -= 255;
		hue -= 10;

	}



}
void RPManimation() {
	static uint32_t timing = 0;
	static uint8_t index = 0;
	static int8_t indexDir = 1;

	if ((millis() - timing) > 50) {
		timing = millis();
		if (index > 8) {
			indexDir = -1;
		}
		else if (index <= 0) {
			indexDir = 1;
		}
		index += indexDir;
	}

	for (int i = 9; i >= 0; i--)
	{
		if (i == index) {
			//	leds[12 + i] = blendColors(leds[12 + i], CRGB(0, 0, 255));
			leds[23 - i] = CRGB(255, 0, 0);
		}
		else {
			leds[23 - i] = blendColors(leds[23 - i], CRGB(0, 0, 0), FADE_AMOUNT_KNIGHT);
		}
	}

}


CRGB blendColors(CRGB currentColor, CRGB targetColor, uint8_t fade) {
	CRGB returnC;
	if (currentColor == targetColor) {
		return currentColor;
	}

	returnC = currentColor;
	int rDiff, gDiff, bDiff;
	rDiff = currentColor.r - targetColor.r;
	gDiff = currentColor.g - targetColor.g;
	bDiff = currentColor.b - targetColor.b;

	if (abs(rDiff) > fade) {
		if (rDiff > 0) {
			returnC.r -= fade;
		}
		else {
			returnC.r += fade;
		}
	}
	else {
		returnC.r -= rDiff;
	}
	if (abs(gDiff) > fade) {
		if (gDiff > 0) {
			returnC.g -= fade;
		}
		else {
			returnC.g += fade;
		}
	}
	else {
		returnC.g -= gDiff;
	}
	if (abs(bDiff) > fade) {
		if (bDiff > 0) {
			returnC.b -= fade;
		}
		else {
			returnC.b += fade;
		}
	}
	else {
		returnC.b -= bDiff;
	}
	return returnC;
}