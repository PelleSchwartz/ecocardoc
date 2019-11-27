/*
 * control_LB.h
 *
 * Created: 21/4/2017
 * Author: Søren Møller Christensen
 *
 */
#include "light_io.h"
#include "light_global.h"

#ifndef LIGHT_B_CONTROL_
#define LIGHT_B_CONTROL_

//OUTPUT and INPUT pins defined in light_io.h
#define R_STARTER OB2
#define L_STARTER OA1
#define R_SIGNAL OA3
#define L_SIGNAL OB3
#define BACK_LIGHT OB4
#define BRAKE_LIGHT OA4



void starterLight(uint8_t starter);
void brakeLight(float brake);
void backNormalLight(uint8_t normal);
void backLeftSignal(uint8_t lS);
void backRightSignal(uint8_t rS);
void turnSignal(uint8_t right, uint8_t left);

#endif /* LIGHT_B_CONTROL_ */
