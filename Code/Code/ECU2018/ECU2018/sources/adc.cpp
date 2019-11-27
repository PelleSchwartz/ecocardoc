/*==========*/
/* Includes */
/*==========*/
#include <ADC.h>
#include "Arduino.h"
#include "global.h"
#include "adc.h"
#include "config.h"

#define USE_ASYNC_ADC

#define BATTERY_SENSOR_PIN 35
#define BRAKE_SENSOR_PIN 15
#define MINI_SENSOR_PIN 16
#define LAMBDA_SENSOR_1_PIN 23
#define LAMBDA_SENSOR_2_PIN 22	// Currently unused / ADC0
#define HALL_SENSOR_PIN 37		// Currently unused / ADC1
#define CORE_TEMPERATURE_SENSOR_PIN 24

#define NUM_ADC0_SENSORS 3
const int ADC0_SENSORS_PINS[NUM_ADC0_SENSORS]{
	BRAKE_SENSOR_PIN,
	LAMBDA_SENSOR_1_PIN,
	CORE_TEMPERATURE_SENSOR_PIN
};
#define NUM_ADC1_SENSORS 2
const int ADC1_SENSORS_PINS[NUM_ADC1_SENSORS]{
	BATTERY_SENSOR_PIN,
	MINI_SENSOR_PIN,
};

#define LAMBDA_VOLTAGE_DIVIDER_CONSTANT 1.0f / (2000.0f / (2000.0f + 1000.0f))		// A voltage division between a 1k and a 2k resistor
#define BATTERY_ADC_TO_VOLTAGE_FACTOR 15.0f / 65535.0f

/*===========*/
/* Functions */
/*===========*/

static ADC *adc = new ADC();
static volatile int batteryAdc = 0, brakeAdc = 0, miniAdc = 0, lambda1Adc = 0, lambda2Adc = 0, coreTempAdc = 0;
static volatile int adc0ReadIndex = 0, adc1ReadIndex = 0;

void adc_init() {
	// Set pin modes
	pinMode(BATTERY_SENSOR_PIN, INPUT);
	pinMode(BRAKE_SENSOR_PIN, INPUT);
	pinMode(MINI_SENSOR_PIN, INPUT);
	pinMode(LAMBDA_SENSOR_1_PIN, INPUT);
	// Set the resolution for analogRead
	adc->setAveraging(32, ADC_0);
	adc->setResolution(16, ADC_0);
	adc->setReference(ADC_REFERENCE::REF_3V3, ADC_0);
	adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED, ADC_0);
	adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED, ADC_0);
	adc->setAveraging(32, ADC_1);
	adc->setResolution(16, ADC_1);
	adc->setReference(ADC_REFERENCE::REF_3V3, ADC_1);
	adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED, ADC_1);
	adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED, ADC_1);
	adc->startSingleRead(ADC0_SENSORS_PINS[0], ADC_0);
	adc->startSingleRead(ADC1_SENSORS_PINS[0], ADC_1);
}

void runAnalogMeasurements() {
#ifdef USE_ASYNC_ADC
	if(adc->isComplete(ADC_0)) {
		int v = (uint16_t)adc->readSingle(ADC_0);
		switch(ADC0_SENSORS_PINS[adc0ReadIndex]) {
		case BRAKE_SENSOR_PIN:
			brakeAdc = v;
			break;
		case LAMBDA_SENSOR_1_PIN:
			lambda1Adc = v;
			break;
		case CORE_TEMPERATURE_SENSOR_PIN:
			coreTempAdc = v;
			break;
		}
		adc0ReadIndex++;
		if(adc0ReadIndex == NUM_ADC0_SENSORS) adc0ReadIndex = 0;
		adc->startSingleRead(ADC0_SENSORS_PINS[adc0ReadIndex], ADC_0);
	}
	if(adc->isComplete(ADC_1)) {
		int v = (uint16_t)adc->readSingle(ADC_1);
		switch(ADC1_SENSORS_PINS[adc1ReadIndex]) {
		case BATTERY_SENSOR_PIN:
			batteryAdc = v;
			break;
		case MINI_SENSOR_PIN:
			miniAdc = v;
			break;
		}
		adc1ReadIndex++;
		if(adc1ReadIndex == NUM_ADC1_SENSORS) adc1ReadIndex = 0;
		adc->startSingleRead(ADC1_SENSORS_PINS[adc1ReadIndex], ADC_1);
	}
#else
	batteryAdc = analogRead(BATTERY_SENSOR_PIN);
	brakeAdc = analogRead(BRAKE_SENSOR_PIN);
	miniAdc = analogRead(MINI_SENSOR_PIN);
	lambda1Adc = analogRead(LAMBDA_SENSOR_1_PIN);
	coreTempAdc = analogRead(70);
#endif
}

// Get the core temperature of the Teensy microprocessor
float getCoreTemperature() {
	if(coreTempAdc == 0) return 0;
	float volts = coreTempAdc / 65535.0f * 3.3f;
	float temperature = 25 - ((volts - 0.716f) / 0.00162f);
	return temperature;
}

float getBrakePressure() {
	return (brakeAdc / 65535.0f * 3.3f * (1000 + 2000) / 2000 - 1) * 35 / 4 - 1;
}

float getBatteryVoltage() {
	return batteryAdc * BATTERY_ADC_TO_VOLTAGE_FACTOR;
}

int getMiniAdcValue() {
	return miniAdc;
}

// Returns the calculated value from the lambda 1 sensor
// The analog value is read as a 16bit value, converted to voltage, multiplied by the voltage division, 
// multiplied division, multiplied with the lambda conversion slope and then added the lambda intercept from the UI.
static float calculateLambda1value() {
	return (lambda1Adc / 65535.0f) * 3.3f * LAMBDA_VOLTAGE_DIVIDER_CONSTANT * getLambda1ConversionSlope() + getLambda1ConversionIntercept();
}

float getLambda() {
	return calculateLambda1value();
}