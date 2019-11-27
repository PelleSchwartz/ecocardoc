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
#include <stdint.h>
#include <util/delay.h>
#include <header/rgb.h>

#define WS2811_PORT PORTA //RGB is on PORTA
#define bit 0 //on bit 0
#define array_size 78 //We have 26 LED's times 3 values = 78

//Function prototypes
void Set_LED_Color(unsigned char LED, unsigned char R, unsigned char G, unsigned char B);
void update_LED(void);
void test_kim(void);

//Global vaariables
unsigned char rgb[78] = {0};
/*
* Type that holds RGB values.
* The in-memory order of this type is actually GRB, but the constructor takes
* its values in RGB order.
*/


/*
* This function sends the RGB-data in an array of rgb structs through
* the given io-pin.
* The port is determined by the macro WS2811_PORT, but the actual pin to
* be used is an argument to this function. This allows a single instance of this function
* to control up to 8 separate channels.
*/

void send(unsigned char* values)
{
	const uint8_t mask =_BV(bit);
	uint8_t low_val = WS2811_PORT & (~mask);
	uint8_t high_val = WS2811_PORT | mask;
	uint8_t size = array_size * sizeof values[0]; // size in bytes


	// reset the controllers by pulling the data line low
	uint8_t bitcount = 4;
	WS2811_PORT = low_val;
	_delay_us(80);

	// note: the labels in this piece of assembly code aren't very explanatory. The real documentation
	// of this code can be found in the spreadsheet ws2811@8Mhz.ods
	// The order of the blocks of code have been determined by arrange_timed_code.cpp
	// The code could be made a little shorter by combining adjacent nops into RJMPS, but since I'm expecting
	// to re-assemble the code fragments from the spreadsheet at some point, I'm not bothering with that now.

	asm volatile(
	"            LD __tmp_reg__, %a[dataptr]+"	"\n" // fill the first byte and determine first bit value
	"            LSL __tmp_reg__"	"\n"
	"            LDI %[bits], 4"    "\n"
	"            BRCC L13"	"\n"
	"            RJMP L5"	"\n"
	"    H1:     NOP"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            NOP"	"\n"
	"            RJMP END"	"\n"
	"    L1:     SUBI %[bits], 1"	"\n"
	"            BREQ M2"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            BRCC L8"	"\n"
	"            NOP"	"\n"
	"    L15:    NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            RJMP L9"	"\n"
	"    M4:     LDI %[bits], 4"	"\n"
	"            LD __tmp_reg__, %a[dataptr]+"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            SUBI %[bytes], 1"	"\n"
	"            BREQ H1"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            BRCS L9"	"\n"
	"            NOP"	"\n"
	"    L16:    OUT %[portout], %[downreg]"	"\n"
	"            NOP"	"\n"
	"    L13:    OUT %[portout], %[upreg]"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            BRCC L10"	"\n"
	"            SUBI %[bits], 1"	"\n"
	"            BREQ M4"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            BRCS L15"	"\n"
	"            NOP"	"\n"
	"    L8:     NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            RJMP L16"	"\n"
	"    M2:     LDI %[bits], 4"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            LD __tmp_reg__, %a[dataptr]+"	"\n"
	"            SUBI %[bytes], 1"	"\n"
	"            BREQ H1"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            BRCC L16"	"\n"
	"            NOP"	"\n"
	"    L9:     OUT %[portout], %[downreg]"	"\n"
	"            NOP"	"\n"
	"    L5:     OUT %[portout], %[upreg]"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            BRCS L1"	"\n"
	"            SUBI %[bits], 1"	"\n"
	"            BREQ M1"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            BRCC L4"	"\n"
	"            NOP"	"\n"
	"    L12:    NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            RJMP L5"	"\n"
	"    M3:     LD __tmp_reg__, %a[dataptr]+"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            LDI %[bits], 4"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            SUBI %[bytes], 1"	"\n"
	"            BREQ H2"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            BRCC L16"	"\n"
	"            NOP"	"\n"
	"            RJMP L5"	"\n"
	"    L10:    SUBI %[bits], 1"	"\n"
	"            BREQ M3"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            BRCS L12"	"\n"
	"            NOP"	"\n"
	"    L4:     NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            RJMP L13"	"\n"
	"    M1:     LD __tmp_reg__, %a[dataptr]+"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            LDI %[bits], 4"	"\n"
	"            OUT %[portout], %[upreg]"	"\n"
	"            NOP"	"\n"
	"            OUT %[portout], %[downreg]"	"\n"
	"            SUBI %[bytes], 1"	"\n"
	"            BREQ H2"	"\n"
	"            LSL __tmp_reg__"	"\n"
	"            BRCS L9"	"\n"
	"            NOP"	"\n"
	"            RJMP L13"	"\n"
	"    H2:     NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"            NOP"	"\n"
	"    END:    NOP"	"\n"
	: /* no output */
	: /* inputs */
	[dataptr] "e" (values), 	// pointer to grb values
	[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
	[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
	[bytes]   "d" (size),		// number of bytes to send
	[bits]    "d" (bitcount),   // number of bits/2
	[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
	);
}


void Clear_LEDs(void)
{
	unsigned char rgb[78] = {0};
	send(rgb);
}


void Set_LED_Color(unsigned char LED, unsigned char R, unsigned char G, unsigned char B)
{
	rgb[LED*3-3] = G;
	rgb[LED*3-2] = R;
	rgb[LED*3-1] = B;
	//send(rgb);
}



/*
FUNCTION:		LED_RPM

DESCRIPTION:	Sets the LED bar according to the RPM

PARAMETERS:		unsigned short RPM

RETURN:			None
*/
void LED_RPM(unsigned short RPM)
{
	const unsigned char LOW_RPM = 0;
	const unsigned short HIGH_RPM = 3000;
	//Color scheme
	const unsigned char color[30] = {0,0,255,0,65,190,0,130,125,0,195,60,0,255,0,65,190,0,130,125,0,195,60,0,255,0,0,255,0,0};
	const unsigned char START_LED = 9;
	unsigned char RPM_LED = START_LED + (RPM-LOW_RPM)*10/(HIGH_RPM-LOW_RPM);
	
	//Correct for too large RPM values
	if(RPM_LED>=START_LED+10)
	{
		RPM_LED = START_LED+10;
	}
	
	for(unsigned char i=9;i<RPM_LED;i++)
	{
		Set_LED_Color(i,color[(i-START_LED)*3],color[(i-START_LED)*3+1],color[(i-START_LED)*3+2]);
	}
}

void update_LED(void){
	send(rgb);
}

void test_kim(void){
	int index = 0;
	while(1){
		rgb[index] ^= 0xFF;
		index = (index+1) % 78;
		send(rgb);
		_delay_ms(500);
	}
}