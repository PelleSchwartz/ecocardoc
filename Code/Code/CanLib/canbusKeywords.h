#pragma once

#define MT_STATUS 0
#define MT_MEASURE 1

// CAN NODES
enum NODES {
	ECU_ID,
	STEERING_ID,
	LIGHT_FRONT_ID,
	LIGHT_BACK_ID,
	AUTO_ID,
	NUM_OF_TX_NODES
};

// CAN priority list (top = highest priority)
enum CAN_PRIORITY {
	CAN_RESERVED,			// Do not use
	CAN_STEERING_PRIORITY,	// Steering board
	CAN_ECU_HIGH_PRIORITY,	// ECU board HIGH (for important values, same as Motor board)
	CAN_AUTO_PRIORITY,		// Auto board
	CAN_LIGHT_F_PRIORITY,	// Only sends front brake pressure
	CAN_ECU_LOW_PRIORITY,	// ECU board LOW (for less important values)
	CAN_LIGHT_B_PRIORITY	// Doesn't send anything
};

////////////////////////////// DATA FIELDS //////////////////////////////

// STEERING STATES
enum STATE_STEERING {
	BUTTON_CL, // Closed loop
	BUTTON_IDLE,
	BUTTON_AUTOGEAR,
	BUTTON_NEUTRAL,
	BUTTON_POTUP,
	BUTTON_POTDOWN,
	BUTTON_AUTOPILOT,
	BUTTON_HYPER,
	BUTTON_PARTY,
	BUTTON_LIGHT,
	BUTTON_FAN,
	BUTTON_WIPER,
	BUTTON_BLINK_RIGHT,
	BUTTON_BURN,
	BUTTON_HORN,
	BUTTON_BLINK_LEFT,
	BUTTON_GEARUP,
	BUTTON_GEARDOWN,
	NUM_OF_STATE_STEERING
};

// ECU STATES
enum STATE_ECU {
	STATE_ECU_ALIVE,
	STATE_ECU_STARTER,
	STATE_ECU_BURN,
	STATE_ECU_PARTY,
	STATE_ECU_AUTOGEAR,
	NUM_OF_STATE_ECU
};

// AUTO STATES
enum STATE_AUTO {
	AUTO_BURN,
	AUTO_HORN,
	AUTO_LIGHT,
	AUTO_BLINK_LEFT,
	AUTO_BLINK_RIGHT,
	AUTO_HAS_CONTROL,	// Autopilot Active
	AUTO_PARKING,
	NUM_OF_STATE_AUTO
};

// SENSORS: When ECU runs with RIO (not standalone), the ECU values come from the RIO
enum SENSORS {
	ECU_WATER_TEMP,
	ECU_OIL_TEMP,
	ECU_EXHAUST_TEMP,
	ECU_RPM,
	ECU_LAMBDA,		// Excess air ratio
	ECU_GEAR,
	ECU_KMPERL,		// Fuel consumption [km/l
	ECU_REMAINING_FUEL,
	ECU_POTENTIOMETER,
	ECU_SPEED,
	ECU_DISTANCE,
	ECU_CURRENT,	// Currently unused
	ECU_KWH,		// Currently unused
	ECU_BRAKE,
	ECU_BAT_VOLTAGE,
	// Time
	SET_RTC_TIME,
	RTC_TIME,
	// Front
	FRONT_BRAKE,
	// Auto
	AUTO_DISTANCE_FRONT,
	AUTO_STEERING_POT,
	AUTO_BATTERY_VOLTAGE,
	// Do not edit
	NUM_OF_SENSORS
};

////////////////////////////// STRINGS FOR UI //////////////////////////////

// STEERING STATES
static const char* const STATE_STEERING_STRINGS[NUM_OF_STATE_STEERING]{
	"BUTTON_CL",
	"BUTTON_IDLE",
	"BUTTON_AUTOGEAR",
	"BUTTON_NEUTRAL",
	"BUTTON_POTUP",
	"BUTTON_POTDOWN",
	"BUTTON_AUTOPILOT",
	"BUTTON_HYPER",
	"BUTTON_PARTY",
	"BUTTON_LIGHT",
	"BUTTON_FAN",
	"BUTTON_WIPER",
	"BUTTON_BLINK_RIGHT",
	"BUTTON_BURN",
	"BUTTON_HORN",
	"BUTTON_BLINK_LEFT",
	"BUTTON_GEARUP",
	"BUTTON_GEARDOWN"
};

// "ECU STATES
static const char* const STATE_ECU_STRINGS[NUM_OF_STATE_ECU] {
	"STATE_ECU_ALIVE",
	"STATE_ECU_STARTER",
	"STATE_ECU_BURN",
	"STATE_ECU_PARTY",
	"STATE_ECU_AUTOGEAR"
};

// AUTO STATES
static const char* const STATE_AUTO_STRINGS[NUM_OF_STATE_AUTO] {
	"AUTO_BURN",
	"AUTO_HORN",
	"AUTO_LIGHT",
	"AUTO_BLINK_LEFT",
	"AUTO_BLINK_RIGHT",
	"AUTO_HAS_CONTROL",
	"AUTO_PARKING"
};

// SENSORS
static const char* const SENSORS_STRINGS[NUM_OF_SENSORS] {
	"ECU_WATER_TEMP",
	"ECU_OIL_TEMP",
	"ECU_EXHAUST_TEMP",
	"ECU_RPM",
	"ECU_LAMBDA",
	"ECU_GEAR",
	"ECU_KMPERL",
	"ECU_REMAINING_FUEL",
	"ECU_POTENTIOMETER",
	"ECU_SPEED",
	"ECU_DISTANCE",
	"ECU_CURRENT",
	"ECU_KWH",
	"ECU_BRAKE",
	"ECU_BAT_VOLTAGE",
	"SET_RTC_TIME",
	"RTC_TIME",
	"FRONT_BRAKE",
	"AUTO_DISTANCE_FRONT",
	"AUTO_STEERING_POT",
	"AUTO_BATTERY_VOLTAGE"
};


////////////////////////////// MISC FIELDS //////////////////////////////

enum Error_MO {
	ECU_RX_ERROR,
	ECU_TX_ERROR,
	STEERING_RX_ERROR,
	STEERING_TX_ERROR,
	FRONT_LIGHT_RX_ERROR,
	FRONT_LIGHT_TX_ERROR,
	BACK_LIGHT_RX_ERROR,
	BACK_LIGHT_TX_ERROR
};

// Send in emergency message.
enum errorNo {
	ERROR_OK, // No error
	ERROR_DEFAULT,
	ERROR_BATTERY, // Low battery
	ERROR_EXTERNAL_BUTTON // External kill switch.
};

enum measurement {
	TEMPERATURE,
	VOLTAGE,
	VELOCITY,
	CURRENT
};
