#ifndef _SCREEN_H



#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "displayGraphics.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#define	NORMAL_DISP 0
#define	GPS_DISP 1
#define	DEBUG_DISP 2
#define	NUM_MODES 3

#define letterHeight 25
#define indent 180
#define debugTextSize 1
#define debugTextOffset 45
void screen_init();
void updateDisplay();
void toggleMode();
void resetDisplay();
void resetTimer();
void startTimer();
void loadValues(float newSpeed, float newWaterTemp, float newOilTemp, float newDist, float newGear, float newLambda, float newRpm, float newBatteryVoltage, float newKmL, uint8_t newError);
void loadAutoState(uint8_t autoButton);

extern Adafruit_TFTLCD tft;


#endif // !_SCREEN_H


