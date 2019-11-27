/*
 * global.h
 * 
 * Contains all global values, includes and references to global variables.
 *
 * Created: 13/4/2015 22:04:12
 *  Author: Henning
 */ 
#ifndef GLOBAL_H_
#define GLOBAL_H_

/* Includes */
#include "SuperGlobalConstants.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "delay.h"

/* Crystal clock speed */
#define F_CPU 		16000000UL

/* Global variables */
extern volatile uint32_t time;

/* Helping functions */
#define TESTBIT(var, bit)	(var  & (1<<bit))
#define SETBIT(var, bit)	(var |= (1<<bit))
#define CLRBIT(var, bit)	(var &= ~(1<<bit))
#define FLIPBIT(var, bit)	(var ^= (1<<bit))
// Necessary for return functions
#define GETBIT(var, bit)	((var  & (1<<bit))>>bit)

#endif /* GLOBAL_H_ */