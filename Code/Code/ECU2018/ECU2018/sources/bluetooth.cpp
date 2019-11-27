/*
 * bluetooth.cpp
 *
 */

 /*==========*/
 /* Includes */
 /*==========*/

#include "Arduino.h"
#include "bluetooth.h"

/*===========*/
/* Functions */
/*===========*/

void blue_init() {
	BLUESERIAL.begin(BAUDBLUE);
}

/******************************************************************
 * Function: blue_set_new_device
 * Description: Gives the bluetooth module a new name. For this to
 *              work you need to set the LE friend in CMD mode.
 * Inputs: Desired name
 * Outputs: N/A
 ******************************************************************/
void blue_set_new_device(String name) {
	delay(10000);
	// Set name
	BLUESERIAL.print("AT+GAPDEVNAME=");
	BLUESERIAL.print(name);
	BLUESERIAL.print("\n\r");
	delay(1000);
	// Max power
	BLUESERIAL.print("AT+BLEPOWERLEVEL=4");
	delay(1000);
	// Disable CTS
	BLUESERIAL.print("AT+UARTFLOW=off");
	delay(1000);
	BLUESERIAL.print("ATZ\n\r");
}
