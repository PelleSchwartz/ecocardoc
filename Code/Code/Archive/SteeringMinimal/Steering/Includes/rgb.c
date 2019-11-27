//
// Copyright (c) 2013 Danny Havenith
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/**
* Library for bit-banging data to WS2811 led controllers.
* This file contains a definition of the rgb and the send() function.
*/

#include "global.h"
#include "rgb.h"

#define WS2811_PORT		PORTA	// RGB is on PORTA
#define WS2811_SUBPORT	PA0		// On PA0
#define RGB_REGSIZE		78		// We have 26 LED's times 3 values = 78

//Global variables
/*
* Type that holds RGB values.
* The in-memory order of this type is actually GRB, but the constructor takes
* its values in RGB order.
*/
uint8_t rgb[RGB_REGSIZE] = {0x00};

/*
* This function sends the RGB-data in an array of rgb structs through
* the given io-pin.
*/
void send(unsigned char* values) {
	const uint8_t mask =_BV(WS2811_SUBPORT);
	uint8_t low_val = WS2811_PORT & (~mask);
	uint8_t high_val = WS2811_PORT | mask;
	uint16_t size = RGB_REGSIZE * sizeof(rgb[0]); // size in bytes


	// reset the controllers by pulling the data line low
	uint8_t bitcount = 7;
	WS2811_PORT = low_val;
	delay_us(40); // at 3 clocks per iteration, this is 320 ticks or 40us at 8Mhz

	// The labels in this piece of assembly code aren't very explanatory. The real documentation
	// of this code can be found in the spreadsheet ws2811@8Mhz.ods
	// A hint if you still want to follow the code below: The code for a regular bit (i.e. bits 7-1)
	// starts at label s00 with the current bit value already in the carry flag and it jumps halfway
	// to label cont06. The two-digit suffix of labels shows the "phase" of the signal at the time
	// of the execution, 00 being the first clock tick of the bit and 09 being the last.
	asm volatile(
	"start:  LDI %[bits], 7                          \n" // start code, load bit count
	"        LD __tmp_reg__, %a[dataptr]+            \n" // fetch first byte
	"cont06: NOP                                     \n"
	"cont07: NOP                                     \n"
	"        OUT %[portout], %[downreg]              \n" // Force line down, even if it already was down
	"cont09: LSL __tmp_reg__                         \n" // Load next bit into carry flag.
	"s00:    OUT %[portout], %[upreg]                \n" // Start of bit, bit value is in carry flag
	"        BRCS skip03                             \n" // only lower the line if the bit...
	"        OUT %[portout], %[downreg]              \n" // ...in the carry flag was zero.
	"skip03: SUBI %[bits], 1                         \n" // Decrease bit count...
	"        BRNE cont06                             \n" // ...and loop if not zero
	"        LSL __tmp_reg__                         \n" // Load the last bit into the carry flag
	"        BRCC Lx008                              \n" // Jump if last bit is zero
	"        LDI %[bits], 7                          \n" // Reset bit counter to 7
	"        OUT %[portout], %[downreg]              \n" // Force line down, even if it already was down
	"        NOP                                     \n"
	"        OUT %[portout], %[upreg]                \n" // Start of last bit of byte, which is 1
	"        SBIW %[bytes], 1                        \n" // Decrease byte count
	"        LD __tmp_reg__, %a[dataptr]+            \n" // Load next byte
	"        BRNE cont07                             \n" // Loop if byte count is not zero
	"        RJMP brk18                              \n" // Byte count is zero, jump to the end
	"Lx008:  OUT %[portout], %[downreg]              \n" // Last bit is zero
	"        LDI %[bits], 7                          \n" // Reset bit counter to 7
	"        OUT %[portout], %[upreg]                \n" // Start of last bit of byte, which is 0
	"        NOP                                     \n"
	"        OUT %[portout], %[downreg]              \n" // We know we're transmitting a 0
	"        SBIW %[bytes], 1                        \n" // Decrease byte count
	"        LD __tmp_reg__, %a[dataptr]+            \n"
	"        BRNE cont09                             \n" // Loop if byte count is not zero
	"brk18:  OUT %[portout], %[downreg]              \n"
	"                                                \n" // used to be a NOP here, but returning from the function takes long enough
	"                                                \n" // We're done.
	: /* no output */
	: /* inputs */
	[dataptr] "e" (values), 	// pointer to grb values
	[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
	[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
	[bytes]   "w" (size),		// number of bytes to send
	[bits]    "d" (bitcount),       // number of bits/2
	[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
	);
}


void Clear_LEDs(void) {
	for(int i=0; i<RGB_REGSIZE; i++) {
		rgb[i] = 0;
	}
	send(rgb);
}

void Set_LED_Color(unsigned char LED, unsigned char R, unsigned char G, unsigned char B) {
	rgb[LED*3-3] = G;
	rgb[LED*3-2] = R;
	rgb[LED*3-1] = B;
}

/*
FUNCTION:		LED_RPM

DESCRIPTION:	Sets the LED bar according to the RPM

PARAMETERS:		unsigned short RPM

RETURN:			None
*/
void LED_RPM(unsigned short RPM) {
	const unsigned char LOW_RPM = 0;
	const unsigned short HIGH_RPM = 3000;
	//Color scheme
	const unsigned char color[30] = {0,0,255,0,65,190,0,130,125,0,195,60,0,255,0,65,190,0,130,125,0,195,60,0,255,0,0,255,0,0};
	const unsigned char START_LED = 9;
	unsigned char RPM_LED = START_LED + (RPM-LOW_RPM)*10/(HIGH_RPM-LOW_RPM);
	
	//Correct for too large RPM values
	if(RPM_LED>=START_LED+10) {
		RPM_LED = START_LED+10;
	}
	
	for(unsigned char i=9;i<RPM_LED;i++) {
		Set_LED_Color(i,color[(i-START_LED)*3],color[(i-START_LED)*3+1],color[(i-START_LED)*3+2]);
	}
}

void update_LED(void) {
	send(rgb);
}

void test_kim(void) {
	int index = 0;
	while(1){
		rgb[index] ^= 0xFF;
		index = (index+1) % 78;
		send(rgb);
		delay_ms(500);
	}
}