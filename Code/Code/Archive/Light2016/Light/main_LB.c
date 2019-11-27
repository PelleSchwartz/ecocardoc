/*
 * main_LB.c
 *
 * Created: 05/05/2016 13:17:21
 *  Author: Kim
 */ 

#include "Components_Shared/global.h"
#include <avr/io.h>
#include <util/delay.h>

#include "Components_LB\Receiver.h"
#include "Components_LB\LightControl.h"
#include "leds.h"

int main(void) {
	// INITIALIZE
	lightControl_Initiate();
	receiver_initiate();		
	
	DDRD &= ~(1 << DDD0);
	
	// START OF MAIN WHILE
	while(1) {
		lightControl_LightsOn(1);
		update_blink_state();
		
		// Update light
		lightControl_StarterLight(received_Starter());
		lightControl_BrakeLight(received_Brake());
		lightControl_NormalLight(received_Normal());
		lightControl_LeftSignal(received_LeftBlink());
		lightControl_RightSignal(received_RightBlink());
	}
	// END OF MAIN WHILE
	return 0;
}

