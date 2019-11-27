/*
 * control_LB.h
 *
 * Created: 21/4/2017
 * Author: Søren Møller Christensen
 *
 */

#include "light_global.h"
#include "light_io.h"

#ifndef LIGHT_B_CONTROL_
#define LIGHT_B_CONTROL_

//OUTPUT and INPUT pins defined in light_io.h
#define R_STARTER OA1
#define BACK_LIGHT OA2
#define L_SIGNAL OA3
#define L_STARTER OA4
#define BRAKE_LIGHT OB2
#define R_SIGNAL OB3

void starterLight(uint8_t starter);
void brakeLight(uint8_t brake);
void backNormalLight(uint8_t normal);
void backLeftSignal(uint8_t lS);
void backRightSignal(uint8_t rS);

#endif /* LIGHT_B_CONTROL_ */
