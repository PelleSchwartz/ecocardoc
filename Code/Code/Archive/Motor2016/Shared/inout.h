/*
 * inout.h
 *
 * Created: 11/06/2016 18:20:07
 *  Author: Henning
 */ 

#ifndef INOUT_H_
#define INOUT_H_

void io_init(void);
void wheel_sensor_init(uint8_t numberOfMagnetsPerRevolution, uint32_t mMagicWheelConstant);

void setHornHigh(void);
void setHornLow(void);

void setStarterHigh(void);
void setStarterLow(void);

uint8_t digitalReadGSensor(void);
uint16_t getWheelSensorPeriod(void);
uint32_t getDistanceCompleted(void);

void SetPWMDutyGear(uint16_t duty);
void SetPWMDutySpeed(uint16_t duty);
void pwm_init(void);

#endif /* INOUT_H_ */