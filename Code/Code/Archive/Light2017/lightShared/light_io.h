/*
 * light_io.h
 *
 * Created: 20/4/2017
 * Author: Håkon Westh-Hansen
 *
 */

#include "light_global.h"

#ifndef LIGHT_IO_H_
#define LIGHT_IO_H_

#define OA1 6
#define OA2 31
#define OA3 2
#define OA4 28
#define OB1 5
#define OB2 30
#define OB3 1
#define OB4 29
#define IN1 23
#define IN2 32
#define testLED 33

void LED_on(uint8_t LED);
void LED_off(uint8_t LED);
void LED_toggle(uint8_t LED);
void LED_blink(uint8_t LED, uint16_t duty);
void light_io_init();
void io_test();


#endif /* LIGHT_IO_H_ */
