/*
 * leds.h
 *
 * Created: 01/5/2016
 *  Author: Kim
 */ 


#ifndef LEDS_H_
#define LEDS_H_

/*
n is the LED nr. on the board.
Changing to WriteMode means we can write data out from the chip.
Changing to ReadMode means we can read data from the pin.
*/
void LEDnWriteMode(uint8_t n);
void LEDnReadMode(uint8_t n);

/*
LednOn changes the port to a logical 1.
LednOff changes the port to a logical 0.
LEDnToggle changes the port from 1 to 0 or 0 to 1.
The value is only transmitted to the pin if that LED is in write mode.
*/
void LEDnOn(uint8_t n);
void LEDnOff(uint8_t n);
void LEDnToggle(uint8_t n);

/*
If in read mode, it reads the logical value on the pin.
If in write mode, it reads the logical value which is stored in the port(and transmitted to the pin).
*/
uint8_t LEDnRead(uint8_t n);

#endif /* LEDS_H_ */