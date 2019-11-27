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

/* Crystal clock speed */
#define F_CPU 		8000000UL

/* Constants */
#define POT1_MAX		1000
#define POT1_START		500
#define POT1_MIN		0
#define DEBOUNCE_DELAY	40

/* Helping functions */
#define TESTBIT(var, bit)	(var  & (1<<bit))
#define SETBIT(var, bit)	(var |= (1<<bit))
#define CLRBIT(var, bit)	(var &= ~(1<<bit))
#define FLIPBIT(var, bit)	(var ^= (1<<bit))

#endif /* GLOBAL_H_ */