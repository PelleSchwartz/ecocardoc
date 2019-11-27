/*
 * SuperGlobalConstants.h
 *
 * Used for definitions used everywhere.
 * Author: Kim
 */ 
 
#ifndef SuperGlobalConstants_H_
#define SuperGlobalConstants_H_

// Baudrates for the USART
#define BAUD485 	19200
#define BAUD232		19200

// ID for Innovator, Dynamo and the Testbench
#define DTU_DYNAMO		1
#define DTU_INNOVATOR	2
#define DTU_TEST		3

// Start bytes for RS232
#define RS232_START_1	0x55
#define RS232_START_2	0xAA

// UID for RS485 bus devices
#define RS485_ID_FRONT_LIGHT	1
#define RS485_ID_STEERING_WHEEL 2
#define RS485_ID_BACK_LIGHT		3

// Start byte for RS485 from MOTOR
#define RS485_START_1		0xA5
#define RS485_START_2		0x08

// Start byte for RS485 response to MOTOR
#define RS485_RESPONSE_START_1		0x42
#define RS485_RESPONSE_START_2		0x08

// Constants for the LIGHT toggle value
#define LIGHT_OFF	0
#define LIGHT_ON	1

// Constants for the read and write operations
#define OP_READ		0
#define OP_WRITE	1

#endif /* SuperGlobalConstants_H_ */