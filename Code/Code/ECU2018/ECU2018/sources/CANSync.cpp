/*
* CANSync.cpp
*
* Description: Responsible for all CAN communication
*
*/

/*==========*/
/* Includes */
/*==========*/

#include <TimeLib.h>
#include "adc.h"
#include "canbus.h"
#include "global.h"
#include "rs232.h"
#include "inout.h"
#include "CANSync.h"
#include "position.h"
#include "ecu.h"
#include "PCSync.h"
#include "ObjectHome.h"
#include "config.h"
#include "dataLogger.h"

// The previously received gear value from Steering
static int previousSteeringGear;
//static int oldpot;

// Flag if RTC has been synced from Steering
static bool hasRtcSynced = false;

/*===========*/
/* Functions */
/*===========*/

/******************************************************************
* Function: _CAN_react
* Description: Reacts on CAN commands
******************************************************************/
static inline void _CAN_react() {
	// Security precaution
	bool burn = false, idle = false;
	// For additional safety, if required, remove the AUTO_HAS_CONTROL check to stop if steering gone to shit
	if((CAN.goneToShit(STEERING_ID) && !CAN.getSystemState(AUTO_ID, AUTO_HAS_CONTROL)) || emergency) {
		burn = false;	// Send burn stop if steering has gone to shit, or the emergency button has been presed
		idle = false;
	} else {
		// For additional safety, if required, use this to stop autonomous burn when steering wheel dead: CAN.getSystemState(STEERING_ID, BUTTON_AUTOPILOT) &&
		if(CAN.getSystemState(AUTO_ID, AUTO_HAS_CONTROL) &&
			CAN.getSystemState(AUTO_ID, AUTO_BURN) &&
			!CAN.goneToShit(AUTO_ID)) {
			if(CAN.getSystemState(AUTO_ID, AUTO_BURN)) burn = true;
		} else {
			if(CAN.getSystemState(STEERING_ID, BUTTON_BURN)) burn = true;
			if(CAN.getSystemState(STEERING_ID, BUTTON_IDLE)) idle = true;
		}
	}
	if(burn && idle) {
		// That's not allowed!
		burn = false;
		idle = false;
	}
	requestBurn(burn);
	requestIdle(idle);
	// Auto state logic
	if(CAN.getSystemState(AUTO_ID, AUTO_HAS_CONTROL)) setExtraOutHigh();
	else setExtraOutLow();
	// Gear logic
	int steeringGear = 0;
	if(CAN.getSystemState(STEERING_ID, BUTTON_GEARUP)) steeringGear = 1;
	else if(CAN.getSystemState(STEERING_ID, BUTTON_GEARDOWN)) steeringGear = 2;
	else if(CAN.getSystemState(STEERING_ID, BUTTON_NEUTRAL)) steeringGear = 3;
	else if(CAN.getSystemState(STEERING_ID, BUTTON_AUTOGEAR)) steeringGear = 4;
	if(steeringGear != previousSteeringGear) {
		previousSteeringGear = steeringGear;
		if(CAN.getSystemState(STEERING_ID, BUTTON_AUTOGEAR)) {
			setAutoGear(true);
		} else {
			setAutoGear(false);
		}
		// Logic for setting gear state
		if(CAN.getSystemState(STEERING_ID, BUTTON_GEARUP) && gear.getCurrentGear() == 0) {
			gear.requestGear(1);
		} else if(CAN.getSystemState(STEERING_ID, BUTTON_GEARUP) && gear.getCurrentGear() == 1) {
			gear.requestGear(2);
		} else if(CAN.getSystemState(STEERING_ID, BUTTON_GEARDOWN)) {
			gear.requestGear(1);
		} else if(CAN.getSystemState(STEERING_ID, BUTTON_NEUTRAL)) {
			gear.requestGear(0);
		}
	}
	// Sync RTC
	if(!hasRtcSynced && CAN.getMeasurement(RTC_TIME) > 0) {
		Teensy3Clock.set(CAN.getMeasurement(RTC_TIME));
		setTime(CAN.getMeasurement(RTC_TIME));
		setTimeOffsetToNow(CAN.getMeasurement(RTC_TIME) - millis() / 1000);
		hasRtcSynced = true;
	}
	/*//implement pot setup here
	int newpot = -1;
	if(CAN.getSystemState(STEERING_ID, BUTTON_POTDOWN)) {
		newpot = 0;
	} else if(CAN.getSystemState(STEERING_ID, BUTTON_POTUP)) {
		newpot = 1;
	}
	if(newpot != oldpot) {
		oldpot = newpot;
		if(CAN.getSystemState(STEERING_ID, BUTTON_POTDOWN)) {
			setFuelCorrection(getFuelCorrection() - 0.1);
		} else if(CAN.getSystemState(STEERING_ID, BUTTON_POTUP)) {
			setFuelCorrection(getFuelCorrection() + 0.1);
		}
	}*/
}

/******************************************************************
* Function: _CAN_transmit
* Description: Sends data from the ECU board out on the CAN bus
******************************************************************/
static inline void _CAN_transmit() {
	// Motor measurements
	CAN.sendMeasurement(CAN_ECU_HIGH_PRIORITY, 0, ECU_BRAKE, getBrakePressure());
	CAN.sendMeasurement(CAN_ECU_HIGH_PRIORITY, 0, ECU_SPEED, 3.6f * getSpeed());
	CAN.sendMeasurement(CAN_ECU_HIGH_PRIORITY, 0, ECU_DISTANCE, getDistance());
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_BAT_VOLTAGE, getBatteryVoltage());
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_RPM, getEncoderRPM());
	if(STANDALONE_MODE) {
		uint32_t systemstate = 0;
		if(!emergency) systemstate |= (1 << STATE_ECU_ALIVE);
		if(getStarter()) systemstate |= (1 << STATE_ECU_STARTER);
		if(isEngineRunning()) systemstate |= (1 << STATE_ECU_BURN);
		if(getPcParty()) systemstate |= (1 << STATE_ECU_PARTY);
		if(getAutoGear()) systemstate |= (1 << STATE_ECU_AUTOGEAR);
		CAN.sendStatus(CAN_ECU_HIGH_PRIORITY, systemstate);
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_REMAINING_FUEL, getRemainingFuel());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_WATER_TEMP, getWaterTemperature());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_OIL_TEMP, getOilTemperature());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_EXHAUST_TEMP, getExhaustTemperature());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_RPM, getEncoderRPM());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_LAMBDA, getLambda());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_GEAR, gear.getCurrentGear());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_KMPERL, getDistancePerVolume());
		CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_POTENTIOMETER, getFuelCorrection());
	}
}

/******************************************************************
* Function: _rs232_to_CAN_sync
* Description: Sends/syncs received data from RIO with CAN
******************************************************************/
static inline void _rs232_to_CAN_sync() {
	// Update CAN measurements array

	// Motor / rio state variable. enum defined in canbus.
	uint32_t systemstate = 0;

	if(rio_rx[RIO_RX_ALIVE]) {
		systemstate |= 1 << STATE_ECU_ALIVE;
	} else {
		systemstate &= ~(1 << STATE_ECU_ALIVE);
	}

	if(rio_rx[RIO_RX_STARTER]) {
		systemstate |= 1 << STATE_ECU_STARTER;
	} else {
		systemstate &= ~(1 << STATE_ECU_STARTER);
	}

	CAN.sendStatus(CAN_ECU_HIGH_PRIORITY, systemstate);

	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_REMAINING_FUEL, (rio_rx[RIO_RX_REMAINING_FUEL_H] << 8) | rio_rx[RIO_RX_REMAINING_FUEL_L]);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_SPEED, rio_rx[RIO_RX_KMH]);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_WATER_TEMP, rio_rx[RIO_RX_WATER_TEMP]);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_OIL_TEMP, rio_rx[RIO_RX_OIL_TEMP]);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_RPM, (rio_rx[RIO_RX_RPM_H] << 8) | rio_rx[RIO_RX_RPM_L]);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_LAMBDA, rio_rx[RIO_RX_LAMBDA] / 100.0);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_GEAR, rio_rx[RIO_RX_GEAR]);
	CAN.sendMeasurement(CAN_ECU_LOW_PRIORITY, 0, ECU_KMPERL, (rio_rx[RIO_RX_KMPERL_H] << 8) | rio_rx[RIO_RX_KMPERL_L]);
}

/******************************************************************
* Function: _rs232_to_CAN_sync
* Description: Sends/syncs received data from ECU & CAN to RIO
******************************************************************/
static inline void _CAN_to_rs232_sync() {
	// Security precaution
	rio_tx[RIO_TX_BURN] = 0;
	if(CAN.goneToShit(STEERING_ID) || emergency) {
		rio_tx[RIO_TX_BURN] = 0;	// Send burn stop if steering has gone to shit, or the emergency button has been presed
	} else {
		if(CAN.getSystemState(STEERING_ID, BUTTON_AUTOPILOT) &&
			CAN.getSystemState(AUTO_ID, AUTO_HAS_CONTROL) &&
			CAN.getSystemState(AUTO_ID, AUTO_BURN) &&
			!CAN.goneToShit(AUTO_ID)) {
			rio_tx[RIO_TX_BURN] = CAN.getSystemState(AUTO_ID, AUTO_BURN);	// "RIO_START"
		} else {
			rio_tx[RIO_TX_BURN] = CAN.getSystemState(STEERING_ID, BUTTON_BURN);	// "RIO_START"
		}
	}
	rio_tx[RIO_TX_IDLE] = CAN.getSystemState(STEERING_ID, BUTTON_IDLE);
	rio_tx[RIO_TX_CLOSED_LOOP] = CAN.getSystemState(STEERING_ID, BUTTON_CL);
	rio_tx[RIO_TX_AUTOGEAR] = CAN.getSystemState(STEERING_ID, BUTTON_AUTOGEAR);
	rio_tx[RIO_TX_GEAR_UP] = CAN.getSystemState(STEERING_ID, BUTTON_GEARUP);
	rio_tx[RIO_TX_GEAR_DOWN] = CAN.getSystemState(STEERING_ID, BUTTON_GEARDOWN);
	rio_tx[RIO_TX_POT_UP] = CAN.getSystemState(STEERING_ID, BUTTON_POTUP);
	rio_tx[RIO_TX_POT_DOWN] = CAN.getSystemState(STEERING_ID, BUTTON_POTDOWN);
	rio_tx[RIO_TX_NEUTRAL] = CAN.getSystemState(STEERING_ID, BUTTON_NEUTRAL);
	rio_tx[RIO_TX_BRAKE] = CAN.getSystemState(LIGHT_FRONT_ID, 0);
}

static inline void _motor_to_rs232_sync() {
	// Get distance completed
	uint16_t distance = getDistance();
	rio_tx[RIO_TX_DISTANCE_H] = (distance >> 8) & 0x00FF;
	rio_tx[RIO_TX_DISTANCE_L] = distance & 0x00FF;
}

static void _rs232_react() {
	// Set Gear
	uint16_t gearPwm = (((uint16_t)rio_rx[RIO_RX_SERVO_GEAR_H]) << 8) | rio_rx[RIO_RX_SERVO_GEAR_L];
	SetPWMDutyGear(gearPwm);
}

// Sends the all CAN values over Serial in JSON-compatible format
void CANtoSerial() {
	// JSON-like header
	Serial.print("[{\"");
	Serial.print(ECU_UI_CAN);
	Serial.print("\": \"");
	Serial.println("##### CAN START #####");
	// Send all values
	Serial.println("SENSORS:");
	for(int i = 0; i < NUM_OF_SENSORS; i++) {
		Serial.print("\t");
		Serial.print(SENSORS_STRINGS[i]);
		Serial.print(": ");
		Serial.println(CAN.getMeasurement(SENSORS(i)));
	}
	Serial.println("STEERING:");
	for(int i = 0; i < NUM_OF_STATE_STEERING; i++) {
		Serial.print("\t");
		Serial.print(STATE_STEERING_STRINGS[i]);
		Serial.print(": ");
		Serial.println(CAN.getSystemState(STEERING_ID, STATE_STEERING(i)));
	}
	Serial.println("AUTO:");
	for(int i = 0; i < NUM_OF_STATE_AUTO; i++) {
		Serial.print("\t");
		Serial.print(STATE_AUTO_STRINGS[i]);
		Serial.print(": ");
		Serial.println(CAN.getSystemState(AUTO_ID, STATE_AUTO(i)));
	}
	Serial.println("ECU:");
	for(int i = 0; i < NUM_OF_STATE_ECU; i++) {
		Serial.print("\t");
		Serial.print(STATE_ECU_STRINGS[i]);
		Serial.print(": ");
		Serial.println(CAN.getSystemState(ECU_ID, STATE_ECU(i)));
	}
	// End of sending
	Serial.println("##### CAN END #####");
	Serial.println("\"}]");
}

// Sends the all RS232 values over Serial in JSON-compatible format
void RIOtoSerial() {
	// JSON-like header
	Serial.print("[{\"");
	Serial.print(ECU_UI_RS232);
	Serial.print("\": \"");
	Serial.println("##### RS232 START #####");
	// Header text so we understand what we are looking at
	Serial.print("RIO debug (RX):");
	Serial.print(" AL: ");
	Serial.print("\t WT: ");
	Serial.print("\t OT: ");
	Serial.print("\t RPM: ");
	Serial.print("\t LA: ");
	Serial.print("\t GR: ");
	Serial.print("\t KMH: ");
	Serial.print("\t KMPERL: ");
	Serial.print("\t REMF: ");
	Serial.print("\t START: ");
	Serial.println("\t SERVO: ");

	// Data from RIO -> MOTOR
	Serial.print("\t\t");
	Serial.print(rio_rx[RIO_RX_ALIVE]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_RX_WATER_TEMP]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_RX_OIL_TEMP]);
	Serial.print("\t ");
	Serial.print((uint16_t)rio_rx[RIO_RX_RPM_H] << 8 | rio_rx[RIO_RX_RPM_L]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_RX_LAMBDA]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_RX_GEAR]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_RX_KMH]);
	Serial.print("\t ");
	Serial.print((uint16_t)rio_rx[RIO_RX_KMPERL_H] << 8 | rio_rx[RIO_RX_KMPERL_L]);
	Serial.print("\t\t ");
	Serial.print((uint16_t)rio_rx[RIO_RX_REMAINING_FUEL_H] << 8 | rio_rx[RIO_RX_REMAINING_FUEL_L]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_RX_STARTER]);
	Serial.print("\t\t ");
	Serial.println((uint16_t)rio_rx[RIO_RX_SERVO_GEAR_H] << 8 | rio_rx[RIO_RX_SERVO_GEAR_L]);

	// What we are sending to RIO header
	Serial.print("RIO debug (TX):");
	Serial.print(" STOP: ");
	Serial.print("\t BURN: ");
	Serial.print("\t IDLE: ");
	Serial.print("\t CL: ");
	Serial.print("\t PTU: ");
	Serial.print("\t PTD: ");
	Serial.print("\t NEU: ");
	Serial.print("\t GRU: ");
	Serial.print("\t GRD: ");
	Serial.print("\t WH: ");
	Serial.print("\t WL: ");
	Serial.print("\t DH: ");
	Serial.print("\t DL: ");
	Serial.println("\t BRK: ");

	// Data from MOTOR -> RIO
	Serial.print("\t\t");
	Serial.print(rio_tx[RIO_TX_STOP]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_BURN]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_IDLE]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_CLOSED_LOOP]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_POT_UP]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_POT_DOWN]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_NEUTRAL]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_GEAR_UP]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_GEAR_DOWN]);
	Serial.print("\t ");
	Serial.print(rio_tx[RIO_TX_WHEEL_PERIOD_H]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_TX_WHEEL_PERIOD_L]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_TX_DISTANCE_H]);
	Serial.print("\t ");
	Serial.print(rio_rx[RIO_TX_DISTANCE_L]);
	Serial.print("\t ");
	Serial.println(rio_tx[RIO_TX_BRAKE]);

	// End of sending
	Serial.println("##### RS232 END #####");
	Serial.println("\"}]");
}

void CAN_sync() {
	if(STANDALONE_MODE) {	// When RIO not connected
		// Send data motor -> CAN bus
		_CAN_react();
		_CAN_transmit();
	} else {				// When RIO connected
		// Send data RIO -> CAN bus
		_rs232_to_CAN_sync();
		// Send data CAN -> RIO
		_CAN_to_rs232_sync();
		// Send data motor -> CAN bus
		_CAN_transmit();
		// React on data
		_rs232_react();
		// Debugging
#ifdef RIO_DEBUG
		RIOtoSerial();
#endif
	}
}
