#include <eeprom.h>
#include "config.h"

// Hardcoded lambda constants
#define LAMBDA_1_SLOPE 0.144f				//= 3.008 is what we calculated for AFR. But alas, the MEKs are masters of black magic.
#define LAMBDA_1_INTERCEPT 0.8f			//= 7.35 is what we calculated for AFR. But alas, the MEKs are masters of black magic.
// Minimum log period [ms]	(if too low, buffer might overflow)
#define MIN_LOG_PERIOD 100

namespace RamEnum {
	enum RAMVariables {
		fuelCorrection,
		engineCalibration,
		maxRPM,
		starterLimit,
		idleLimit,
		fuelTankVolume,
		ignitionDwellTime,
		injectionTiming,
		idleInjection,
		gearChangeWait,
		gearChangeLimit,
		gearUpSpeed,
		gearDownSpeed,
		gearNeutralPwm,
		gear1Pwm,
		gear2Pwm,
		fuelConversionSlope,
		fuelConversionIntercept,
		lambda1ConversionSlope,
		lambda1ConversionIntercept,
		extraValue0,
		extraValue1,
		fuelDensity,
		fuelLowerHeatingValue,
		equivalentLowerHeatingValue,
		wheelDiameter,		// [m]
		wheelPulsesPerRevolution,
		logPeriod, // [ms]
		extraValue2,
		extraValue3,
		extraValue4,
		extraValue5,
		extraValue6,
		extraValue7,
		extraValue8,
		extraValue9,

		arrayStartAddress // It is critical that this is the last one (as it fills 100 bytes)
	};
}

// Each array of 25 values fills up 100 bytes (float = 32 bits = 4 bytes, 25 * 4 bytes)
#define EE_IGNITION_ARRAY_START (sizeof(float) * RamEnum::arrayStartAddress)
#define EE_INJECTION1_ARRAY_START ((sizeof(float) * RamEnum::arrayStartAddress) + LUT_ARRAY_LENGTH * 4)
#define EE_INJECTION2_ARRAY_START ((sizeof(float) * RamEnum::arrayStartAddress) + 2 * LUT_ARRAY_LENGTH * 4)

/*
// RAM stuff
// The following is values stored in the RAM and on the EEPROM.
// They can be accessed using their respective get functions
*/
static float fuelCorrection = 1,
engineCalibration = 187,
maxRPM = 4000,
starterLimit = 1200,
idleLimit = 2000,
fuelTankVolume = 100,
ignitionDwellTime = 3000,
injectionTiming = 180 - 360,
idleInjection = 8000,
gearChangeWait = 150,
gearChangeLimit = 2800,
gearUpSpeed = 18,
gearDownSpeed = 12,
gearNeutralPwm = 178,
gear1Pwm = 200,
gear2Pwm = 157,
fuelConversionSlope = 0.78f,
fuelConversionIntercept = 810,
lambda1ConversionSlope = LAMBDA_1_SLOPE,
lambda1ConversionIntercept = LAMBDA_1_INTERCEPT,
extraValue0 = 0,
extraValue1 = 0,
fuelDensity = 745, //g/L very approximated
fuelLowerHeatingValue = 26900,
equivalentLowerHeatingValue = 42900,
wheelDiameter = 0.5588f,		// [m]
wheelPulsesPerRevolution = 15,
logPeriod = 1000,
extraValue2 = 0,
extraValue3 = 0,
extraValue4 = 0,
extraValue5 = 0,
extraValue6 = 0,
extraValue7 = 0,
extraValue8 = 0,
extraValue9 = 0;

// These values will be overwritten from EEPROM
static float ignitionArray[LUT_ARRAY_LENGTH] = {
	20, 20, 20, 20, 20, 20, 25, 25, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35
};
// These values will be overwritten from EEPROM
static float injection1Array[LUT_ARRAY_LENGTH] = {
	7000, 7000, 7000, 7000, 7000, 7000, 7000, 6000, 5500, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 4500, 4500, 4500, 4501, 4500
};
// These values will be overwritten from EEPROM
static float injection2Array[LUT_ARRAY_LENGTH] = {
	7000, 7000, 7000, 7000, 7000, 7000, 7000, 6000, 5500, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 5300, 4500, 4500, 4500, 4501, 4500
};

// Loads all the variables from the EEPROM to the ram.
// They can be accessed using the respective get-function, eg. getMaxRPM()
// They can be set (in RAM and EEPROM) using their set-functions.
void eeLoadToRAM() {
	// As a float is 4 bytes (32 bit), they have to be seperated by that much.
	EEPROM.get(sizeof(float) * RamEnum::fuelCorrection, fuelCorrection);
	EEPROM.get(sizeof(float) * RamEnum::engineCalibration, engineCalibration);
	EEPROM.get(sizeof(float) * RamEnum::maxRPM, maxRPM);
	EEPROM.get(sizeof(float) * RamEnum::starterLimit, starterLimit);
	EEPROM.get(sizeof(float) * RamEnum::idleLimit, idleLimit);
	EEPROM.get(sizeof(float) * RamEnum::fuelTankVolume, fuelTankVolume);
	EEPROM.get(sizeof(float) * RamEnum::ignitionDwellTime, ignitionDwellTime);
	EEPROM.get(sizeof(float) * RamEnum::injectionTiming, injectionTiming);
	EEPROM.get(sizeof(float) * RamEnum::idleInjection, idleInjection);
	EEPROM.get(sizeof(float) * RamEnum::gearChangeWait, gearChangeWait);
	EEPROM.get(sizeof(float) * RamEnum::gearChangeLimit, gearChangeLimit);
	EEPROM.get(sizeof(float) * RamEnum::gearUpSpeed, gearUpSpeed);
	EEPROM.get(sizeof(float) * RamEnum::gearDownSpeed, gearDownSpeed);
	EEPROM.get(sizeof(float) * RamEnum::gearNeutralPwm, gearNeutralPwm);
	EEPROM.get(sizeof(float) * RamEnum::gear1Pwm, gear1Pwm);
	EEPROM.get(sizeof(float) * RamEnum::gear2Pwm, gear2Pwm);
	EEPROM.get(sizeof(float) * RamEnum::fuelConversionSlope, fuelConversionSlope);
	EEPROM.get(sizeof(float) * RamEnum::fuelConversionIntercept, fuelConversionIntercept);
	EEPROM.get(sizeof(float) * RamEnum::lambda1ConversionSlope, lambda1ConversionSlope);
	EEPROM.get(sizeof(float) * RamEnum::lambda1ConversionIntercept, lambda1ConversionIntercept);
	EEPROM.get(sizeof(float) * RamEnum::extraValue0, extraValue0);
	EEPROM.get(sizeof(float) * RamEnum::extraValue1, extraValue1);
	EEPROM.get(sizeof(float) * RamEnum::fuelDensity, fuelDensity);
	EEPROM.get(sizeof(float) * RamEnum::fuelLowerHeatingValue, fuelLowerHeatingValue);
	EEPROM.get(sizeof(float) * RamEnum::equivalentLowerHeatingValue, equivalentLowerHeatingValue);
	EEPROM.get(sizeof(float) * RamEnum::wheelDiameter, wheelDiameter);
	EEPROM.get(sizeof(float) * RamEnum::wheelPulsesPerRevolution, wheelPulsesPerRevolution);
	EEPROM.get(sizeof(float) * RamEnum::logPeriod, logPeriod);
	EEPROM.get(sizeof(float) * RamEnum::extraValue2, extraValue2);
	EEPROM.get(sizeof(float) * RamEnum::extraValue3, extraValue3);
	EEPROM.get(sizeof(float) * RamEnum::extraValue4, extraValue4);
	EEPROM.get(sizeof(float) * RamEnum::extraValue5, extraValue5);
	EEPROM.get(sizeof(float) * RamEnum::extraValue6, extraValue6);
	EEPROM.get(sizeof(float) * RamEnum::extraValue7, extraValue7);
	EEPROM.get(sizeof(float) * RamEnum::extraValue8, extraValue8);
	EEPROM.get(sizeof(float) * RamEnum::extraValue9, extraValue9);
	EEPROM.get(EE_IGNITION_ARRAY_START, ignitionArray);
	EEPROM.get(EE_INJECTION1_ARRAY_START, injection1Array);
	EEPROM.get(EE_INJECTION2_ARRAY_START, injection2Array);
}

float getFuelCorrection() { return fuelCorrection; }
float getEngineCalibrationForPC() { return engineCalibration; } // Encoder not offset to be between [-360;360], "raw" offset
float getEngineCalibrationForCode() { return engineCalibration - 360; } // Encoder offset have to be between [-360;360] last: 187-360
float getMaxRPM() { return maxRPM; }
float getStarterLimit() { return starterLimit; }
float getIdleLimit() { return idleLimit; }
float getFuelTankVolume() { return fuelTankVolume; }
float getIgnitionDwellTime() { return ignitionDwellTime; }
float getInjectionTiming() { return injectionTiming; }
float getIdleInjection() { return idleInjection; }
float getGearChangeWait() { return gearChangeWait; }
float getGearChangeLimit() { return gearChangeLimit; }
float getGearUpSpeed() { return gearUpSpeed; }
float getGearDownSpeed() { return gearDownSpeed; } // [km/t]
float getGearNeutralPwm() { return gearNeutralPwm; }
float getGear1Pwm() { return gear1Pwm; }
float getGear2Pwm() { return gear2Pwm; }
float getFuelConversionSlope() { return fuelConversionSlope; }
float getFuelConversionIntercept() { return fuelConversionIntercept; }
float getLambda1ConversionSlope() { return LAMBDA_1_SLOPE; }
float getLambda1ConversionIntercept() { return LAMBDA_1_INTERCEPT; }
float getExtraValue0() { return extraValue0; }
float getExtraValue1() { return extraValue1; }
float getFuelDensity() { return fuelDensity; } // Density of ethanol [g/L]
float getFuelLowerHeatingValue() { return fuelLowerHeatingValue; } // [kJ/kg] of ethanol
float getEquivalentFuelLowerHeatingValue() { return equivalentLowerHeatingValue; } // [kJ/kg] of gasoline
float getWheelDiameter() { return wheelDiameter; }
float getWheelPulsesPerRevolution() { return wheelPulsesPerRevolution; }
float getLogPeriod() { return logPeriod; }
float getExtraValue2() { return extraValue2; }
float getExtraValue3() { return extraValue3; }
float getExtraValue4() { return extraValue4; }
float getExtraValue5() { return extraValue5; }
float getExtraValue6() { return extraValue6; }
float getExtraValue7() { return extraValue7; }
float getExtraValue8() { return extraValue8; }
float getExtraValue9() { return extraValue9; }
float* getIgnitionArray() { return ignitionArray; }
float* getInjection1Array() { return injection1Array; }
float* getInjection2Array() { return injection2Array; }

// "Auto"generated, too lazy to format
void setFuelCorrection(float newfuelCorrection) { EEPROM.put(sizeof(float) * RamEnum::fuelCorrection, newfuelCorrection); fuelCorrection = newfuelCorrection; }
void setEngineCalibration(float newengineCalibration) { EEPROM.put(sizeof(float) * RamEnum::engineCalibration, newengineCalibration); engineCalibration = newengineCalibration; }
void setMaxRPM(float newmaxRPM) { EEPROM.put(sizeof(float) * RamEnum::maxRPM, newmaxRPM); maxRPM = newmaxRPM; }
void setStarterLimit(float newstarterLimit) { EEPROM.put(sizeof(float) * RamEnum::starterLimit, newstarterLimit); starterLimit = newstarterLimit; }
void setIdleLimit(float newidleLimit) { EEPROM.put(sizeof(float) * RamEnum::idleLimit, newidleLimit); idleLimit = newidleLimit; }
void setFuelTankVolume(float newfuelTankVolume) { EEPROM.put(sizeof(float) * RamEnum::fuelTankVolume, newfuelTankVolume); fuelTankVolume = newfuelTankVolume; }
void setIgnitionDwellTime(float newignitionDwellTime) { EEPROM.put(sizeof(float) * RamEnum::ignitionDwellTime, newignitionDwellTime); ignitionDwellTime = newignitionDwellTime; }
void setInjectionTiming(float newinjectionTiming) { EEPROM.put(sizeof(float) * RamEnum::injectionTiming, newinjectionTiming); injectionTiming = newinjectionTiming; }
void setIdleInjection(float newidleInjection) { EEPROM.put(sizeof(float) * RamEnum::idleInjection, newidleInjection); idleInjection = newidleInjection; }
void setGearChangeWait(float newgearChangeWait) { EEPROM.put(sizeof(float) * RamEnum::gearChangeWait, newgearChangeWait); gearChangeWait = newgearChangeWait; }
void setGearChangeLimit(float newgearChangeLimit) { EEPROM.put(sizeof(float) * RamEnum::gearChangeLimit, newgearChangeLimit); gearChangeLimit = newgearChangeLimit; }
void setGearUpSpeed(float newgearUpSpeed) { EEPROM.put(sizeof(float) * RamEnum::gearUpSpeed, newgearUpSpeed); gearUpSpeed = newgearUpSpeed; }
void setGearDownSpeed(float newgearDownSpeed) { EEPROM.put(sizeof(float) * RamEnum::gearDownSpeed, newgearDownSpeed); gearDownSpeed = newgearDownSpeed; }
void setGearNeutralPwm(float newgearNeutralPwm) { EEPROM.put(sizeof(float) * RamEnum::gearNeutralPwm, newgearNeutralPwm); gearNeutralPwm = newgearNeutralPwm; }
void setGear1Pwm(float newgear1Pwm) { EEPROM.put(sizeof(float) * RamEnum::gear1Pwm, newgear1Pwm); gear1Pwm = newgear1Pwm; }
void setGear2Pwm(float newgear2Pwm) { EEPROM.put(sizeof(float) * RamEnum::gear2Pwm, newgear2Pwm); gear2Pwm = newgear2Pwm; }
void setFuelConversionSlope(float newfuelConversionSlope) { EEPROM.put(sizeof(float) * RamEnum::fuelConversionSlope, newfuelConversionSlope); fuelConversionSlope = newfuelConversionSlope; }
void setFuelConversionIntercept(float newfuelConversionIntercept) { EEPROM.put(sizeof(float) * RamEnum::fuelConversionIntercept, newfuelConversionIntercept); fuelConversionIntercept = newfuelConversionIntercept; }
void setLambda1ConversionSlope(float newlambdaConversionSlope) { EEPROM.put(sizeof(float) * RamEnum::lambda1ConversionSlope, LAMBDA_1_SLOPE); lambda1ConversionSlope = LAMBDA_1_SLOPE; }
void setLambda1ConversionIntercept(float newlambdaConversionIntercept) { EEPROM.put(sizeof(float) * RamEnum::lambda1ConversionIntercept, LAMBDA_1_INTERCEPT); lambda1ConversionIntercept = LAMBDA_1_INTERCEPT; }
void setExtraValue0(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue0, extraValue0); extraValue0 = newValue; }
void setExtraValue1(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue1, extraValue1); extraValue1 = newValue; }
void setFuelDensity(float newfuelDensity) { EEPROM.put(sizeof(float) * RamEnum::fuelDensity, newfuelDensity); fuelDensity = newfuelDensity; }
void setFuelLowerHeatingValue(float newfuelLowerHeatingValue) { EEPROM.put(sizeof(float) * RamEnum::fuelLowerHeatingValue, newfuelLowerHeatingValue); fuelLowerHeatingValue = newfuelLowerHeatingValue; }
void setEquivalentFuelLowerHeatingValue(float newequivalentLowerHeatingValue) { EEPROM.put(sizeof(float) * RamEnum::equivalentLowerHeatingValue, newequivalentLowerHeatingValue); equivalentLowerHeatingValue = newequivalentLowerHeatingValue; }
void setWheelDiameter(float newwheelDiameter) { EEPROM.put(sizeof(float) * RamEnum::wheelDiameter, newwheelDiameter); wheelDiameter = newwheelDiameter; }
void setWheelPulsesPerRevolution(float newwheelPulsesPerRevolution) { EEPROM.put(sizeof(float) * RamEnum::wheelPulsesPerRevolution, newwheelPulsesPerRevolution); wheelPulsesPerRevolution = newwheelPulsesPerRevolution; }
void setLogPeriod(float newlogPeriod) { EEPROM.put(sizeof(float) * RamEnum::logPeriod, newlogPeriod); if(newlogPeriod < MIN_LOG_PERIOD) newlogPeriod = MIN_LOG_PERIOD;  logPeriod = newlogPeriod; }
void setExtraValue2(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue2, extraValue2); extraValue2 = newValue; }
void setExtraValue3(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue3, extraValue3); extraValue3 = newValue; }
void setExtraValue4(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue4, extraValue4); extraValue4 = newValue; }
void setExtraValue5(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue5, extraValue5); extraValue5 = newValue; }
void setExtraValue6(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue6, extraValue6); extraValue6 = newValue; }
void setExtraValue7(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue7, extraValue7); extraValue7 = newValue; }
void setExtraValue8(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue8, extraValue8); extraValue8 = newValue; }
void setExtraValue9(float newValue) { EEPROM.put(sizeof(float) * RamEnum::extraValue9, extraValue9); extraValue9 = newValue; }

// Extra gets
float getEngineRunningLogPeriod() {
	float period = getLogPeriod() / 10.0f;
	if(period < MIN_LOG_PERIOD) return MIN_LOG_PERIOD;
	else return period;
}

void setIgnitionArray(float newIgnitionArrayValue, int RPM) {
	int i = RPM / LUT_TABLE_INCREMENTS;
	EEPROM.put(EE_IGNITION_ARRAY_START + sizeof(float) * i, newIgnitionArrayValue);
	ignitionArray[i] = newIgnitionArrayValue;
}

void setInjection1Array(float newInjection1ArrayValue, int RPM) {
	int i = RPM / LUT_TABLE_INCREMENTS;
	EEPROM.put(EE_INJECTION1_ARRAY_START + sizeof(float) * i, newInjection1ArrayValue);
	injection1Array[i] = newInjection1ArrayValue;
}

void setInjection2Array(float newInjection2ArrayValue, int RPM) {
	int i = RPM / LUT_TABLE_INCREMENTS;
	EEPROM.put(EE_INJECTION2_ARRAY_START + sizeof(float) * i, newInjection2ArrayValue);
	injection2Array[i] = newInjection2ArrayValue;
}

// Can only be run before loadToRam, or it will just write what is already there...
void eeWriteDefaultValues() {
	EEPROM.put(sizeof(float) * RamEnum::fuelCorrection, fuelCorrection);
	EEPROM.put(sizeof(float) * RamEnum::engineCalibration, engineCalibration);
	EEPROM.put(sizeof(float) * RamEnum::maxRPM, maxRPM);
	EEPROM.put(sizeof(float) * RamEnum::starterLimit, starterLimit);
	EEPROM.put(sizeof(float) * RamEnum::idleLimit, idleLimit);
	EEPROM.put(sizeof(float) * RamEnum::fuelTankVolume, fuelTankVolume);
	EEPROM.put(sizeof(float) * RamEnum::ignitionDwellTime, ignitionDwellTime);
	EEPROM.put(sizeof(float) * RamEnum::injectionTiming, injectionTiming);
	EEPROM.put(sizeof(float) * RamEnum::idleInjection, idleInjection);
	EEPROM.put(sizeof(float) * RamEnum::gearChangeWait, gearChangeWait);
	EEPROM.put(sizeof(float) * RamEnum::gearChangeLimit, gearChangeLimit);
	EEPROM.put(sizeof(float) * RamEnum::gearUpSpeed, gearUpSpeed);
	EEPROM.put(sizeof(float) * RamEnum::gearDownSpeed, gearDownSpeed);
	EEPROM.put(sizeof(float) * RamEnum::gearNeutralPwm, gearNeutralPwm);
	EEPROM.put(sizeof(float) * RamEnum::gear1Pwm, gear1Pwm);
	EEPROM.put(sizeof(float) * RamEnum::gear2Pwm, gear2Pwm);
	EEPROM.put(sizeof(float) * RamEnum::fuelConversionSlope, fuelConversionSlope);
	EEPROM.put(sizeof(float) * RamEnum::fuelConversionIntercept, fuelConversionIntercept);
	EEPROM.put(sizeof(float) * RamEnum::lambda1ConversionSlope, lambda1ConversionSlope);
	EEPROM.put(sizeof(float) * RamEnum::lambda1ConversionIntercept, lambda1ConversionIntercept);
	EEPROM.put(sizeof(float) * RamEnum::extraValue0, extraValue0);
	EEPROM.put(sizeof(float) * RamEnum::extraValue1, extraValue1);
	EEPROM.put(sizeof(float) * RamEnum::fuelDensity, fuelDensity);
	EEPROM.put(sizeof(float) * RamEnum::fuelLowerHeatingValue, fuelLowerHeatingValue);
	EEPROM.put(sizeof(float) * RamEnum::equivalentLowerHeatingValue, equivalentLowerHeatingValue);
	EEPROM.put(sizeof(float) * RamEnum::wheelDiameter, wheelDiameter);
	EEPROM.put(sizeof(float) * RamEnum::wheelPulsesPerRevolution, wheelPulsesPerRevolution);
	EEPROM.put(sizeof(float) * RamEnum::logPeriod, logPeriod);
	EEPROM.put(sizeof(float) * RamEnum::extraValue2, extraValue2);
	EEPROM.put(sizeof(float) * RamEnum::extraValue3, extraValue3);
	EEPROM.put(sizeof(float) * RamEnum::extraValue4, extraValue4);
	EEPROM.put(sizeof(float) * RamEnum::extraValue5, extraValue5);
	EEPROM.put(sizeof(float) * RamEnum::extraValue6, extraValue6);
	EEPROM.put(sizeof(float) * RamEnum::extraValue7, extraValue7);
	EEPROM.put(sizeof(float) * RamEnum::extraValue8, extraValue8);
	EEPROM.put(sizeof(float) * RamEnum::extraValue9, extraValue9);
	EEPROM.put(EE_IGNITION_ARRAY_START, ignitionArray);
	EEPROM.put(EE_INJECTION1_ARRAY_START, injection1Array);
	EEPROM.put(EE_INJECTION2_ARRAY_START, injection2Array);
}

INTERPOL_t calculateInterpolation(float rpm) {
	float rpmDivided = rpm / (float)LUT_TABLE_INCREMENTS;
	INTERPOL_t interpolation;
	interpolation.lower = (int)rpmDivided;
	interpolation.upper = interpolation.lower + 1;
	interpolation.increment = rpmDivided - interpolation.lower;
	return interpolation;
}
