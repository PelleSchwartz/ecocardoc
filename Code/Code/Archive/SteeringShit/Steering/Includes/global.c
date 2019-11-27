/*
* global.c
*
* Created: 11/06/2016 17:52:00
*  Author: Henning
*/
#include "global.h"
#include "rs485protocol.h"

// Global uptime in ms
volatile uint32_t time = 0;

// RS485 Buffers
uint8_t steering_out[STEERING_RX_REGSIZE]  = {0};		// STEERING		->	MOTOR
uint8_t steering_in[STEERING_TX_REGSIZE]  = {0};		// MOTOR		->	STEERING

void global_init(void) {
	// Perform any needed initialization of the global variables here
}