/*
* Steering.c
*
* Created: 22/06/2016 21:42:54
* Author : Henning
*/

#include "global.h"
#include "inout.h"
#include "timer.h"
#include "buttons.h"
#include "rs485.h"
#include "rs485sync.h"
#include "lcd.h"
#include "spi.h"
#include "rgb.h"
#include "DOGXL-160-7.h"
#include "rs485protocol.h"
#include <avr/interrupt.h>

void rs485_sync(void) {
	// UPDATE TRANSMIT REGISTER
	// Sync applicable RIO data to RS485
	steering_out[STEERING_BURN] = check_button(BTN_BURN);
	steering_out[STEERING_IDLE] = check_button(BTN_IDLE);
	steering_out[STEERING_CLOSED_LOOP] = check_button(BTN_CLOSED_LOOP);
	steering_out[STEERING_AUTOGEAR] = check_button(BTN_AUTOGEAR);
	steering_out[STEERING_POT_UP] = check_button(BTN_POT_UP);
	steering_out[STEERING_POT_DOWN] = check_button(BTN_POT_DOWN);
	steering_out[STEERING_NEUTRAL] = check_button(BTN_NEUTRAL);
	steering_out[STEERING_GEAR_UP] = check_button(BTN_GEAR_UP);
	steering_out[STEERING_GEAR_DOWN] = check_button(BTN_GEAR_DOWN);
	// Sync applicable MOTOR data to RS485
	steering_out[STEERING_HYPERSPEED] = check_button(BTN_HYPERSPEED);
	steering_out[STEERING_PARTY_TIME] = check_button(BTN_PARTY_TIME);
	steering_out[STEERING_LIGHT_NORMAL] = check_button(BTN_LIGHT_NORMAL);
	steering_out[STEERING_LIGHT_HIGH_BEAM] = check_button(BTN_LIGHT_HIGH_BEAM);
	steering_out[STEERING_WIPER] = check_button(BTN_WIPER);
	steering_out[STEERING_FAN] = check_button(BTN_FAN);
	steering_out[STEERING_HORN] = check_button(BTN_HORN);
	steering_out[STEERING_BLINK_L] = check_button(BTN_BLINK_LEFT);
	steering_out[STEERING_BLINK_R] = check_button(BTN_BLINK_RIGHT);
}

int main(void) {
	// INITIALIZE //
	// Initialize IO Pins
	io_init();
	// Initialize Timer1 for counter
	timer_init();
	// Initialize SPI as Master
	SPI_MasterInit();
	delay_ms(50);
	// Initialize LCD screen
	LCDInit();
	LCDColor(BACKLIGHT_COLOR);
	delay_ms(50);
	LCDClean();
	delay_ms(5);
	// Initialize RS485
	//rs485_init();
	connection_initiate(STEERING_TX_REGSIZE, STEERING_RX_REGSIZE, RS485_ID_STEERING_WHEEL);
	// Enable global interrupts
	sei();
	
	// Show the intro screen
	//LCDIntro();
	
	// START OF MAIN WHILE
	while(1) {
		// Check all buttons and update registers
		update_buttons();
		// Sync to RS485 registers
		rs485_sync();
		// Update LCD screen
		//lcd_update();
		// Update LED lights
		// LED lights do not work reliably do to missing 16MHz crystal
		
		
		// DEBUG STUFF BELOW
		//test_kim();
		//Set_LED_Color(i, 255, 255, 255);
		//update_LED();
		//delay_ms(50);
		//char speed_str[3] = {0};
		//sprintf(speed_str, "%2i", i);
		//LCDWriteString(2, 40, speed_str, 3);
		//i++;
		
		//if(i>8) i = 1;
		//
		//delay_ms(1000);
	}
	// END OF MAIN WHILE
}
