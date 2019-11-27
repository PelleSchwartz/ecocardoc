/*
 Name:		ECU2018.ino
 Description: Main ino file for funning the ECU 2018
 Created:	4/16/2018 9:52:09 AM
 Author:	Asger, Frederik, Irene og Berk
 Based on
 * MotorDynamo2017.ino
 */

 /*==========*/
 /* Includes */
 /*==========*/

#include <canbus.h>
#include <tunes.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include "sources/config.h"
#include "sources/dataLogger.h"
#include "sources/ecu.h"
#include "sources/adc.h"
#include "sources/bluetooth.h"
#include "sources/global.h"
#include "sources/inout.h"
#include "sources/LEDs.h"
#include "sources/CANSync.h"
#include "sources/rs232.h"
#include "sources/rs232sync.h"
#include "sources/position.h"
#include "sources/PCSync.h"
#include "sources/ObjectHome.h"

/*=============*/
/* Definitions */
/*=============*/

//#define DISABLE_EMERGENCY_STOP
//#define RESET_ECU_VALUES
#define USE_DISPLAY

// Car ID
#define DTU_DYNAMO 1
#define CAR DTU_DYNAMO
// The number of 0 packets received from RIO before we determine the RIO is RIP
#define RIO_STOPPED_GRACE_VALUE 10
// The period of Bluetooth synchronization [ms]
#define BLUE_SYNC_PERIOD 1000
// The period of RIO synchronization [ms]
#define RS232_SYNC_PERIOD 100
// The period of CAN synchronization [ms]
#define CAN_SYNC_PERIOD 10
// The period of PC synchronization (ECU UI) [ms]
#define PC_SYNC_PERIOD 25
// The period for checking Mini voltage (important for emergency stop) [ms]
#define MINI_CHECK_PERIOD 5
// The period of ADC reads (important for emergency stop) [ms]
#define ADC_READ_PERIOD 5
// The period for display updates [ms]
#define DISPLAY_OUTPUT_PERIOD 500
// The period for muting emergency music [ms]
#define MUTE_PERIOD 1000 * 60 * 10
// Buzzer pin
#define BUZZER_PIN 36
// OLED display reset pin
#define OLED_RESET_PIN 5
// On board button
#define BUTTON_PIN 2
// Threshold below which to consider emergency button pressed [ADC]
#define MINI_SENSOR_ADC_THRESHOLD 15000 
// Threshold below which to consider battery critically low [V]
#define BATTERY_VOLTAGE_LOW_THRESHOLD 11.5f


// For party
#define PARTY_ANIMATION_SPEED 400

/*========================*/
/* Variables & Prototypes */
/*========================*/

// Prototype Functions
static inline void sendTelemetryToBluetooth();
time_t getTeensy3Time();

// Counter for counts of too low measured Mini and battery voltage
static uint16_t miniVoltageTooLow = 0, batteryVoltageTooLow = 0;

/* Local variables for main loop */
static uint32_t blueSyncTiming = 0;
static uint32_t CANSyncTiming = 0;
static uint32_t pcSyncTiming = 0;
static uint32_t adcReadTiming = 0;
static uint32_t miniCheckTiming = 0;
#if !(STANDALONE_MODE)
static uint32_t rs232SyncTiming = 0;
// Counter for received RIO Safe Shutdown signal or lost RIO connection
static uint16_t rioHasStopped = 0;
#endif
#ifdef USE_DISPLAY
static uint32_t displayTiming = 0;
static uint32_t loopBeganAtMicros = 0;
static uint32_t loopsSinceOutput = 0;
static uint32_t forMeasuringLoopTime = 0;
#endif

// Emergency
uint8_t emergency = 0;

// Button
static uint32_t buttonPressedTime = 0;
static bool muteEmergencySong = false;

// Logging data
#define COMMIT_LOG_TO_FILE_PERIOD 10000 // Every 10s
static uint32_t timeSinceLastSD = 0;
static uint32_t timeSinceLastBuffer = 0;

// Ms Timer
IntervalTimer myTimer;
static volatile uint32_t counter = 0;

#ifdef USE_DISPLAY
// Display 
Adafruit_SSD1306 display(OLED_RESET_PIN);
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] = { B00000000, B11000000, B00000001, B11000000, B00000001, B11000000, B00000011, B11100000, B11110011, B11100000, B11111110, B11111000, B01111110, B11111111, B00110011, B10011111, B00011111, B11111100, B00001101, B01110000, B00011011, B10100000, B00111111, B11100000, B00111111, B11110000, B01111100, B11110000, B01110000, B01110000, B00000000, B00110000 };
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif

// SETUP //
void setup() {
	/* Initializations */
#ifdef RESET_ECU_VALUES
	eeWriteDefaultValues();
#endif // RESET_ECU_VALUES
	// Initialize ECU
	initializeEcu();
	// Initialize IO Pins
	io_init();
	// Initialize Time
	// Initialize time
	setSyncProvider(getTeensy3Time);
	// Startup delay
	delay(400);
	// Load variables
	eeLoadToRAM();
	// Initialize USB communication
	Serial.begin(115200);
	//while (!Serial) {}
	//Serial.printf("Serial now started up at %d",micros());
	// Initialize ADC
	adc_init();
	//Serial.printf("ADC initialized at %d",micros());
	// Initialize LEDs
	LED_init();
	//Serial.printf("LED initialized at %d", micros());
	// Initialize PWM for Gear servo
	pwm_init();
	//Serial.printf("PWM initialized at %d", micros());
	// Initialize RS232
#if !(STANDALONE_MODE)
	rs232_init();
	rs232_set_car(CAR);
#endif
	// Initialize CAN
	can_init(ECU_ID);
	//Serial.printf("CAN initialized at %d \n", micros());
	// Initialize Bluetooth
	blue_init();
	//Serial.printf("Bluetooth initialized at %d \n", micros());
	// Initialize Buzzer
	tunes_init(BUZZER_PIN);
	//Serial.printf("Tunes initialized at %d \n", micros());
	// Initialize wheel sensor
	initializeWheelSensor();
	//Serial.printf("Wheel initialized at %d \n", micros());
	// Initialize Encoders
	initializeEncoder();
	// Initialize Objects (currently unused)
	initializeObjects();
	//Serial.printf("Encoder initialized at %d \n", micros());
	// Attach interrupt to onboard button
	attachInterrupt(BUTTON_PIN, buttonInterruptHandler, RISING);
	//Serial.printf("button interupt set up at %d \n", micros());
#ifdef USE_DISPLAY
	// Initialize display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32 screen)
	display.display();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.clearDisplay();
	display.setCursor(0, 0);
	//Serial.printf("Display initialized at %d \n", micros());
#endif // USE_DISPLAY
	// Set up data logging
	initializeLogger();
	// Enable global interrupts
	//Serial.printf("Logger initialized at %d \n", micros());
	// Set up main timer
	myTimer.begin(timerCallback, 1000);
	//Serial.printf("Timer set up at %d \n", micros());
	sei();
	// Play a song so we know we have started
	sing(STARTUP_MELODY_ID);
	//Serial.printf("Done with upstart at %d \n", millis());

}

/*===========*/
/* Main loop */
/*===========*/
// The loop function runs over and over again until power down or reset
void loop() {
	//Serial.printf("main loop start at %d \n", micros());
	/* EMERGENCY STOP */
	// Read from Battery and external emergency stop sensor (VIN to Mini)
	if(millis() - adcReadTiming >= ADC_READ_PERIOD) {
		runAnalogMeasurements();
		adcReadTiming = millis();
	}
	//Serial.printf("main loop, analog read done at %d \n", micros());
	if(millis() - miniCheckTiming >= MINI_CHECK_PERIOD) {
		uint16_t mini_voltage = getMiniAdcValue();
		if(mini_voltage < MINI_SENSOR_ADC_THRESHOLD) {
			// Emergency button must have been pressed
			miniVoltageTooLow++;
			if(miniVoltageTooLow > 5) {
				rio_tx[RIO_TX_STOP] = 1;
				LED_on(LED2);
#ifndef DISABLE_EMERGENCY_STOP
				emergency = 1;
#endif // DISABLE_EMERGENCY_STOP
				stopEngine();
			}
		} else {
			miniVoltageTooLow = 0;
			rio_tx[RIO_TX_STOP] = 0;
			LED_off(LED2);
		}
		miniCheckTiming = millis();
		// Battery low song
		if(getBatteryVoltage() < BATTERY_VOLTAGE_LOW_THRESHOLD) {
			emptyBufferIntoFile();
			batteryVoltageTooLow++;
			if(batteryVoltageTooLow > 3000 && tunes_is_ready()) {
				sing(WARNING_MELODY_ID);
				batteryVoltageTooLow = 0;
			}
		} else {
			batteryVoltageTooLow = 0;
		}
		// Reset mute after MUTE_PERIOD
		muteEmergencySong = muteEmergencySong && (millis() - buttonPressedTime < MUTE_PERIOD);
		// Emergency song
		if(emergency && !muteEmergencySong && tunes_is_ready()) {
			sing(GAME_MELODY_ID);
		}
	}
	//Serial.printf("main loop, minicheck done at %d \n", micros());
	/* COMMUNICATION */
	// Transmit to CAN/update values
	if(millis() - CANSyncTiming >= CAN_SYNC_PERIOD) {
		CAN_sync();
		/*if(emergency) {
			CAN.emergencyStop(ERROR_EXTERNAL_BUTTON);
		}
		// Party Mode detection
		if(CAN.getSystemState(STEERING_ID, BUTTON_PARTY) || getPcParty()) {
			if(tunes_is_ready()) {
				sing(SAX_MELODY_ID);
			}
		}*/
		CANSyncTiming = millis();
	}
	//Serial.printf("main loop,  done at %d \n", micros());
	// Transmit to Bluetooth
	if(millis() - blueSyncTiming >= BLUE_SYNC_PERIOD) {
		sendTelemetryToBluetooth();
		blueSyncTiming = millis();
	}
	
	// Communicate with ECU UI
	if(millis() - pcSyncTiming >= PC_SYNC_PERIOD) {
		pcSync();
		pcSyncTiming = millis();
	}
	//LED_toggle(LED3);
	// Update display
#ifdef USE_DISPLAY
	loopsSinceOutput++;
	forMeasuringLoopTime += (micros() - loopBeganAtMicros);
	loopBeganAtMicros = micros();
	if(millis() - displayTiming >= DISPLAY_OUTPUT_PERIOD) {
		displayTiming = millis();
		forMeasuringLoopTime /= loopsSinceOutput;
		display.printf("Loop time: %d us\n", forMeasuringLoopTime);
		//display.printf("Engine Running: %d\n", isEngineRunning());
		//display.printf("Gear: %d\n", gear.getCurrentGear());
		//display.printf("Burn: %d\n", CAN.getSystemState(STEERING_ID, BUTTON_BURN));
		//display.printf("RPM: %f \n", getEncoderRPM());
		//display.printf("Gear wait: %f", getGearChangeWait());
		//display.printf("For Code: %f", getEngineCalibrationForCode());
		//display.printf("For PC: %f", getEngineCalibrationForPC());
		forMeasuringLoopTime = 0;
		loopsSinceOutput = 0;
		display.display();
		display.clearDisplay();
		display.setCursor(0, 0);
	}
#endif // USE_DISPLAY
	// Data logging
	// Write to buffer every log period, or every 1/10 of log period when engine is running
	if((isEngineRunning() && millis() - timeSinceLastBuffer >= getEngineRunningLogPeriod()) || millis() - timeSinceLastBuffer >= getLogPeriod()) {
		timeSinceLastBuffer = millis();
		logData();
	}
	
	// Commit to file
	if((millis() - timeSinceLastSD >= COMMIT_LOG_TO_FILE_PERIOD || getBufferSize() >= MAX_LOG_QUEUE_SIZE) && !isEngineRunning()) {
		timeSinceLastSD = millis();
		emptyBufferIntoFile();
	}

	
#if !(STANDALONE_MODE)
	// Annoy everyone until everything is restarted
	if(rioHasStopped > RIO_STOPPED_GRACE_VALUE) {
		if(tunes_is_ready()) {
			sing(SAX_MELODY_ID);
		}
		rioHasStopped = 0;
	}

	// RS232 data is received in interrupt
	if(rio_rx[RIO_RX_ALIVE] != 1) {
		rioHasStopped++;
	} else {
		rioHasStopped = 0;
	}

	// Transmit to RIO over RS232
	if(millis() - rs232SyncTiming >= RS232_SYNC_PERIOD) {
		rs232_tx();
		rs232SyncTiming = millis();
	}
#endif
	//Serial.printf("Main loop end at %d \n", micros());
}	// END OF LOOP

/*=========*/
/* Methods */
/*=========*/

time_t getTeensy3Time() {
	return Teensy3Clock.get();
}

static inline void logData() {
	writeToBuffer((float)millis(), 0);
	writeToBuffer(getSpeed() * 3.6f, 1);
	writeToBuffer(getDistance(), 1);
	writeToBuffer(getEncoderRPM(), 1);
	writeToBuffer(getLambda(), 1);
	writeToBuffer(getStarter(), 1);
	writeToBuffer(gear.getCurrentGear(), 1);
	writeToBuffer(getFuelCorrection(), 1);
	writeToBuffer(getDistancePerVolume() > 999 ? 999 : getDistancePerVolume(), 1);
	writeToBuffer(getConsumedFuelMass(), 1);
	writeToBuffer(getBatteryVoltage(), 1);
	writeToBuffer(getOilTemperature(), 1);
	writeToBuffer(getWaterTemperature(), 1);
	writeToBuffer(getExhaustTemperature(), 1);
	writeToBuffer(CAN.getMeasurement(FRONT_BRAKE), 1);
	writeToBuffer(getBrakePressure(), 1);
	writeToBuffer(CAN.getStatus(ECU_ID), 1);
	writeToBuffer(getInjectionDurationTime(), 1);
	writeToBuffer(getIgnitionStartAngle(), 1);
	writeToBuffer(isEngineRunning(), 1);
}

static inline void sendTelemetryToBluetooth() {
	StaticJsonBuffer<BLUE_SEND_BUFFER_SIZE> sendBuffer;
	JsonObject& sendValue = sendBuffer.createObject();
	
	sendValue["tW"] = getWaterTemperature();
	sendValue["tO"] = getOilTemperature();
	sendValue["bP1"] = CAN.getMeasurement(FRONT_BRAKE);
	sendValue["bP2"] = getBrakePressure();
	sendValue["rR"] = getEncoderRPM();
	sendValue["lb"] = getLambda();
	sendValue["sp"] = getSpeed() * 3.6f;
	sendValue["dW"] = getDistance();
	sendValue["gr"] = gear.getCurrentGear();
	sendValue["bv"] = getBatteryVoltage();
	sendValue["rKL"] = getDistancePerVolume();
	sendValue["bu"] = isEngineRunning();
	sendValue["rA"] = !emergency;
	sendValue["rS"] = getStarter();
	sendValue["fC"] = getRemainingFuel();

	sendValue.printTo(BLUESERIAL);
}

// The "MAIN" timer runs every 1ms
inline void timerCallback() {
	counter++;
	ecuTimerCallback();
	ioTimerCallback();
	if(counter % GEAR_TIMER_PERIOD == 0) gearTimerCallback();
}

// Slightly debounced for conveniency
void buttonInterruptHandler() {
	// The button now mutes the song and deletes the log
	if(millis() - buttonPressedTime >= 1000) {
		sing(NO_MELODY_ID);
		muteEmergencySong = true;
		deleteLogFileFromSd();
		buttonPressedTime = millis();
	}
}
