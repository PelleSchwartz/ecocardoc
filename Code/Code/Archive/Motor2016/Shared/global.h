/*
 * global.h
 *
 * Global definitions for Motor 2016
 *
 * Created: 11/06/2016 17:37:30
 *  Author: Henning
 */ 

#ifndef GLOBAL_H_
#define GLOBAL_H_

/* Crystal clock speed */
#define F_CPU 		16000000UL

/* Baudrate for Bluetooth */
// Note: RS232 and RS485 baudrates are defined in SuperGlobalConstants.h
#define BAUDBLUE	9600

/* Helping functions */
#define TESTBIT(var, bit)	(var  & (1<<bit))
#define SETBIT(var, bit)	(var |= (1<<bit))
#define CLRBIT(var, bit)	(var &= ~(1<<bit))
#define FLIPBIT(var, bit)	(var ^= (1<<bit))

/* Includes */
#include "SuperGlobalConstants.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

/* Global variables */
extern volatile uint32_t time;
extern volatile uint8_t rio_tx[];
extern volatile uint8_t rio_temp_rx[];
extern volatile uint8_t rio_rx[];
extern volatile uint8_t steering_rx[];
extern volatile uint8_t steering_tx[];
extern volatile uint8_t lights_reg[];
extern volatile uint8_t front_light_tx[];
extern volatile uint8_t front_light_rx[];
extern volatile uint8_t back_light_tx[];

/* Initialization */
void global_init(void);

#endif /* GLOBAL_H_ */