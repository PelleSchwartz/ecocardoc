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

int main(void) {
	io_init();
	// Initialize LCD screen
	LCDInit();
	LCDColor(BACKLIGHT_COLOR);
	delay_ms(50);
	LCDClean();
	delay_ms(5);
	sei();
	rs485_init();
	
	while(1) {
		
	}
}

//int main(void) {
	//// INITIALIZE //
	//// Initialize IO Pins
	//io_init();
	//// Initialize Timer1 for counter
	//timer_init();
	//// Initialize SPI as Master
	//SPI_MasterInit();
	//delay_ms(50);
	//// Initialize LCD screen
	//LCDInit();
	//LCDColor(BACKLIGHT_COLOR);
	//delay_ms(50);
	//LCDClean();
	//delay_ms(5);
	//// Initialize RS485
	////rs485_init();
	//connection_initiate(STEERING_TX_REGSIZE, STEERING_RX_REGSIZE, RS485_ID_STEERING_WHEEL);
	//// Enable global interrupts
	//sei();
	//
	//// Show the intro screen
	////LCDIntro();
	//
	//// START OF MAIN WHILE
	//while(1) {
		//// Check all buttons and update registers
		//update_buttons();
		//// Sync to RS485 registers
		//rs485_sync();
		//// Update LCD screen
		////lcd_update();
		//// Update LED lights
		//// LED lights do not work reliably do to missing 16MHz crystal
		//
		//
		//// DEBUG STUFF BELOW
		////test_kim();
		////Set_LED_Color(i, 255, 255, 255);
		////update_LED();
		////delay_ms(50);
		////char speed_str[3] = {0};
		////sprintf(speed_str, "%2i", i);
		////LCDWriteString(2, 40, speed_str, 3);
		////i++;
		//
		////if(i>8) i = 1;
		////
		////delay_ms(1000);
	//}
	//// END OF MAIN WHILE
//}
