#ifndef _IO_H

#include "FastLED.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <stdint.h>
#include "Screen.h"			//button input can change display mode. Done in interrupt.
#include "canbus.h"

#define BUTTON_CL_pin 20
#define BUTTON_IDLE_pin 37
#define BUTTON_AUTOGEAR_pin 36
#define BUTTON_NEUTRAL_pin 35
#define BUTTON_POTUP_pin 34
#define BUTTON_POTDOWN_pin 33
#define BUTTON_HIGHBEAM_pin 32
#define BUTTON_HYPER_pin 31
#define BUTTON_PARTY_pin 30
#define BUTTON_LIGHT_pin 28
#define BUTTON_FAN_pin 27
#define BUTTON_WIPER_pin 25
#define BUTTON_GEARUP_pin 26
#define BUTTON_25_pin 22
#define BUTTON_24_pin 23
#define BUTTON_GEARDOWN_pin 38
#define BUTTON_12_pin 5
#define BUTTON_13_pin 6

#define NUM_LEDS 26

void io_init(void);
void io_interrupt_init(void);
uint32_t getAllButtonState(void);
uint8_t getButtonState(uint8_t button);
void updateLeds(uint16_t RPM);
void LED_init();
uint8_t buttonsClicked(void);
void checkButtons();



#endif
