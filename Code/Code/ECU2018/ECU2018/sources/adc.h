/*
 * adc.h
 *
 * Created: 17/5/2015 11:52:28
 * Author: Henning
 *
 */

#include "global.h"

#ifndef ADC_H_
#define ADC_H_

void adc_init();
void runAnalogMeasurements();
float getBrakePressure();
float getBatteryVoltage();
int getMiniAdcValue();
float getCoreTemperature();
float getLambda();

#endif /* ADC_H_ */
