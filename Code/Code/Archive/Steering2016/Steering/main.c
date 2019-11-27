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

#define LCD_UPDATE_TIME 10000		// [iterations]

int main(void) {
	int32_t lastLcdUpdateTime = 0;
	
	// INITIALIZE //
	// Initialize IO Pins
	io_init();
	// Initialize Timer1 for counter
	//timer_init();
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
	rs485_init();
	// Clear LEDs	
	Clear_LEDs();
	update_LED();
	// Enable global interrupts
	sei();	
	
	// Show the intro screen
	LCDIntro();	
	LCDColor(BACKLIGHT_COLOR);
	
	// START OF MAIN WHILE
	while(1) {	
		// Checks all buttons and update registers
		update_buttons();
		// The function rs485_sync() which syncs to RS485 registers is called by interrupt
		if(lastLcdUpdateTime <= 0) {
			// Update LCD screen
			LCDInit();
			lcd_update();
			lastLcdUpdateTime = LCD_UPDATE_TIME;
		}
		else{
			lastLcdUpdateTime--;
		}
		// Update LED lights
		// LED lights do not work reliably due to missing 16MHz crystal
	}
	// END OF MAIN WHILE
}
