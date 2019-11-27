/*
* global.c
*
* Created: 11/06/2016 17:52:00
*  Author: Henning
*/
#include "global.h"
#include "rs232.h"
#include "rs485.h"

// Global uptime in ms
volatile uint32_t time = 0;

// RS232 Buffers
volatile uint8_t rio_tx[RIO_TX_REGSIZE] = {0};					// MOTOR		->	RIO
volatile uint8_t rio_rx[RIO_RX_REGSIZE] = {0};					// RIO			->	MOTOR

// RS485 Buffers
volatile uint8_t steering_rx[STEERING_RX_REGSIZE]  = {0};		// STEERING		->	MOTOR
volatile uint8_t steering_tx[STEERING_TX_REGSIZE]  = {0};		// MOTOR		->	STEERING
volatile uint8_t lights_reg[LIGHTS_REGSIZE] = {0};				// MOTOR		->	LIGHTS		(currently not used)
volatile uint8_t front_light_tx[FRONT_LIGHT_TX_REGSIZE] = {0};	// MOTOR		->	FRONT LIGHT
volatile uint8_t front_light_rx[FRONT_LIGHT_RX_REGSIZE] = {0};	// FRONT LIGHT	->	MOTOR
volatile uint8_t back_light_tx[BACK_LIGHT_TX_REGSIZE] = {0};	// MOTOR		->	BACK LIGHT

void global_init(void) {
	// Perform any needed initialization of the global variables here
}