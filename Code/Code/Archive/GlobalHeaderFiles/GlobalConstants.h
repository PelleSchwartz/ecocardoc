/*
 * GlobalConstants.h
 *
 * Used for definitions used everywhere.
 * Author: Kim
 */ 
 
#ifndef GlobalConstants_H_
#define GlobalConstants_H_

/* General Definitions */
#define F_CPU 		16000000UL
//#define F_CPU 		1000000UL
//#define F_CPU 		8000000UL
//#define BAUD485 	250000

#define BAUD485 	19200
#define BAUD232		19200
#define BAUDEPOS232 115200
#define RS485BAUD 	(F_CPU/16/BAUD485-1)
#define RS232BAUD	(F_CPU/16/BAUD232-1)
#define RS232EPOS2BAUD (F_CPU/16/BAUDEPOS232)-1

#define RX_TIMEOUT	100000
#define OP_READ		0
#define OP_WRITE	1

#define ID_FRONT_LIGHT	  1
#define ID_REAR_LIGHT	  3
#define ID_STEERING_WHEEL 2
#define ID_KERS			  4

#define TESTBIT(var, bit)	(var  & (1<<bit))
#define SETBIT(var, bit)	(var |= (1<<bit))
#define CLRBIT(var, bit)	(var &= ~(1<<bit))
#define FLIPBIT(var, bit)	(var ^= (1<<bit))

#endif /* GlobalConstants_H_ */