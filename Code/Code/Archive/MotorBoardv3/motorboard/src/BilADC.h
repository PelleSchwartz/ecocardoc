/*
 * BilADC.h
 *
 * Created: 17/3/2015 17:11:57
 *  Author: Henning
 */ 


#ifndef BILADC_H_
#define BILADC_H_

uint8_t readGearSensor(void);
uint8_t readWheelSensor(void);
uint16_t readLambdaSensor(void);
uint8_t readBatterySensor(void);
uint8_t readKersSensor(void);
uint8_t readOilTempSensor(void);
uint8_t readH2OTempSensor(void);
uint8_t readSPJLDSensor(void);
uint8_t readFlowSensor(void);

uint8_t readExtra0Sensor(void);
uint8_t readExtra1Sensor(void);
uint8_t readExtra3Sensor(void);
uint8_t readExtra4Sensor(void);
uint8_t readExtra5Sensor(void);
uint8_t readExtra6Sensor(void);

#include "adc.h"

#define Bil_ADC_VREF ADC_VREF_AVCC //can be set to ADC_VREF_AREF

uint8_t readGearSensor(void){
	return adc_read_8bit(ADC_MUX_ADC0,Bil_ADC_VREF);
}

uint8_t readWheelSensor(void){
	return adc_read_8bit(ADC_MUX_ADC2,Bil_ADC_VREF);
}

uint16_t readLambdaSensor(void){
	return adc_read_10bit(ADC_MUX_ADC3,Bil_ADC_VREF);
}

uint8_t readBatterySensor(void){
	return adc_read_8bit(ADC_MUX_ADC4,Bil_ADC_VREF);
}

uint8_t readKersSensor(void){
	return adc_read_8bit(ADC_MUX_ADC5,Bil_ADC_VREF);
}

uint8_t readOilTempSensor(void){
	return adc_read_8bit(ADC_MUX_ADC6,Bil_ADC_VREF);
}

uint8_t readH2OTempSensor(void){
	return adc_read_8bit(ADC_MUX_ADC7,Bil_ADC_VREF);
}

uint8_t readExtra0Sensor(void){
	return adc_read_8bit(ADC_MUX_ADC8,Bil_ADC_VREF);
}

uint8_t readExtra1Sensor(void){
	return adc_read_8bit(ADC_MUX_ADC9,Bil_ADC_VREF);
}

uint8_t readExtra3Sensor(void){
	return adc_read_8bit(ADC_MUX_ADC10,Bil_ADC_VREF);
}

uint8_t readExtra4Sensor(void){
	return adc_read_8bit(ADC_MUX_ADC11,Bil_ADC_VREF);
}

uint8_t readExtra5Sensor(void){
	return adc_read_8bit(ADC_MUX_ADC12,Bil_ADC_VREF);
}

uint8_t readExtra6Sensor(void){
	return adc_read_8bit(ADC_MUX_ADC13,Bil_ADC_VREF);
}

uint8_t readSPJLDSensor(void){
	return adc_read_8bit(ADC_MUX_ADC14,Bil_ADC_VREF);
}

uint8_t readFlowSensor(void){
	return adc_read_8bit(ADC_MUX_ADC15,Bil_ADC_VREF);
}

#endif /* BILADC_H_ */