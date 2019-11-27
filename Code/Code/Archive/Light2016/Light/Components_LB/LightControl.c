/*
 * LightControl.c
 *
 * Created: 2016-05-06 17:45:17
 *  Author: kry
 */ 

#include "../Components_Shared/global.h"
#include "../Hardware/leds.h"
#include "../Hardware/pwm.h"
#include "timer.h"

#define LED_R_SIGNAL 8
#define LED_L_SIGNAL 7
#define LED_R_STARTER 4
#define LED_L_STARTER 5
#define LED_BRAKE_LIGHT 6
#define LED_BACK_LIGHT 9
#define LED_ALWAYS_ON 11

static uint8_t LeftBlinkState = 0;
static uint8_t RightBlinkState = 0;
static uint32_t lastBlinkStateUpdateTime = 0;

void lightControl_Initiate(void) {
	timer_init();
	
	LEDnWriteMode(LED_R_SIGNAL);
	LEDnWriteMode(LED_L_SIGNAL);
	LEDnWriteMode(LED_R_STARTER);
	LEDnWriteMode(LED_L_STARTER);
	LEDnWriteMode(LED_BRAKE_LIGHT);
	LEDnWriteMode(LED_BACK_LIGHT);
	LEDnWriteMode(LED_ALWAYS_ON);
}

void lightControl_LightsOn(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_ALWAYS_ON);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_ALWAYS_ON);
	}
}

void lightControl_StarterLight(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_R_STARTER);
		LEDnOn(LED_L_STARTER);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_R_STARTER);
		LEDnOff(LED_L_STARTER);
	}
}

void lightControl_LeftSignal(uint8_t onOff) {
    if(onOff == LIGHT_ON && !LeftBlinkState) {
        LEDnOn(LED_L_SIGNAL);
        LeftBlinkState = 1;
    }
    else if(onOff == LIGHT_OFF && LeftBlinkState) {
        LEDnOff(LED_L_SIGNAL);
        LeftBlinkState = 0;
    }
}

void lightControl_RightSignal(uint8_t onOff) {
    if(onOff == LIGHT_ON && !RightBlinkState) {
        LEDnOn(LED_R_SIGNAL);
        RightBlinkState = 1;
    }
    else if(onOff == LIGHT_OFF && RightBlinkState) {
        LEDnOff(LED_R_SIGNAL);
        RightBlinkState = 0;
    }
}


void lightControl_BrakeLight(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_BRAKE_LIGHT);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_BRAKE_LIGHT);
	}
}

void lightControl_NormalLight(uint8_t onOff) {
	if(onOff == LIGHT_ON) {
		LEDnOn(LED_BACK_LIGHT);
	}
	else if(onOff == LIGHT_OFF) {
		LEDnOff(LED_BACK_LIGHT);
	}
}

void update_blink_state(void) {
	if(time - lastBlinkStateUpdateTime >= 700) {
		if(LeftBlinkState){
			LEDnOff(LED_L_SIGNAL);
		}
		if(RightBlinkState){
			LEDnOff(LED_R_SIGNAL);
		}
	}
	if(time - lastBlinkStateUpdateTime >= 1000) {
		if(LeftBlinkState){
			LEDnOn(LED_L_SIGNAL);
		}
		if(RightBlinkState){
			LEDnOn(LED_R_SIGNAL);
		}
		lastBlinkStateUpdateTime = time;
	}
}