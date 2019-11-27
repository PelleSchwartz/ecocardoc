/*
* inout.c
*
* Created: 11/06/2016 18:20:15
*  Author: Henning
*/

#include "global.h"
#include "inout.h"

/**
 * \brief Function to setup IO for terminal mode.
 *
 * Configure all I/O as input pull-up enabled to make sure all I/Os have
 * a defined level except LEDs (PB3:0) which is configured as outputs.
 * Light sensor and NTC sensor (PA6 and PA7): input pull-up disable.
 */
void io_init(void) {
	// A0 = output for RGB LEDs, rest = input for buttons
	DDRA |= (1<<DDA0);
	// B0-3 + B6 = input, rest = output, PB4 output for Datadir485
	DDRB |= (1<<DDB4) | (1<<DDB5) | (1<<DDB7); // 0b10110000
	// C all = input. REMEMBER TO DISABLE JTAG!!! STUPID F*CK :-)
	DDRC = 0b00000000;
	// D2 = input, rest = output
	DDRD = 0b11111011;
	
	//Pull ups
	PORTB |= (1 << PORTB5) | (1 << PORTB7);
	PORTD |= (1 << PORTD0) | (1 << PORTD1);	
}