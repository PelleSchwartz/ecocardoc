/*
* Motor2016.c
*
* Created: 02/05/2016 20:54:54
* Author : Henning
*/

// Shared Includes
#include "global.h"
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <delay.h>
#include "LEDs.h"
#include "tunes.h"
#include "bluetooth.h"
#include "rs232.h"
#include "rs232sync.h"
#include "timer.h"
#include "inout.h"
#include "adc.h"
#include "uart.h"
#include "delay.h"
// Dynamo Includes
#include "rs485.h"
#include "rs485sync.h"

// TODO DEBUG
#include <avr/interrupt.h>
ISR(BADISR_vect) {
	// Something has gone horribly wrong
	sing(UNDERWORLD_MELODY_ID);		// TODO
}

// Car ID
#define CAR DTU_DYNAMO
// Constants for the car-specific wheel sensor
// Number of Magnets per Revolution
#define NUMBER_OF_MAGNETS_PER_REVOLUTION 1
// Diameter of the wheel [cm]
#define WHEEL_DIAMETER	80		// TODO Get actual value
// Time delay for RS485 sync [ms]
#define RS485_SYNC_TIME 200
// The number of 0 packets received from RIO before we determine the RIO is RIP
#define RIO_STOPPED_GRACE_VALUE 10
// The period of Bluetooth synchronization
#define BLUE_SYNC_PERIOD	200

// Prototype Functions
static inline void blue_tx(void);
static inline void measureBatteryVoltage(void);
static inline void readFromBluetooth(void);

// Local buffers
// Buffer for received Bluetooth messages (currently not used)
//static char blue_buffer[UART_RX0_BUFFER_SIZE];
// Counter for counts of too low measured Mini Voltage
static uint16_t miniVoltageTooLow = 0;
// Counter for received RIO Safe Shutdown signal or lost RIO connection
static uint16_t rioHasStopped = 0;
// Buffer for Bluetooth/USB messages
static char blue_tx_buffer[256];
// Message to send to Android
static char message[20];

int main(void) {
	// INITIALIZE //
	// Initialize IO Pins
	io_init();
	// Initialize LEDs
	LED_init();	
	// Initialize PWM for Gear servo
	pwm_init();	
	// Initialize Wheel Sensor
	wheel_sensor_init(NUMBER_OF_MAGNETS_PER_REVOLUTION, WHEEL_DIAMETER);
	// Initialize ADC
	adc_init();
	// Initialize RS232
	rs232_init();
	rs232_set_car(CAR);
	// Initialize RS485
	rs485_init();
	// Initialize Bluetooth
	blue_init();
	// Initialize Timer1 for counter
	timer_init();
	// Initialize Buzzer
	tunes_init();
	// Initialize Globals
	global_init();
	// Enable global interrupts
	sei();
	
	// Local variables
	uint32_t counter = 0;
	uint32_t lastBlueSync = 0;
	
	sing(STARTUP_MELODY2_ID);
	
	// START OF MAIN WHILE
	while(1) {
		counter++;
		// Annoy everyone until everything is restarted
		if(rioHasStopped > RIO_STOPPED_GRACE_VALUE) {
			//if(DEBUG) blue_puts("\nRIO IS DEAD\n");
			sing(SAX_MELODY_ID);
			rioHasStopped = 0;
		}	
		// Set Gear
		uint16_t gearPwm = (((uint16_t)rio_rx[RIO_RX_SERVO_GEAR_H]) << 8) | rio_rx[RIO_RX_SERVO_GEAR_L];
		SetPWMDutyGear(gearPwm);
		// Read from Battery and external emergency stop sensor (VIN to Mini)
		measureBatteryVoltage();		
		// Get wheel period from wheel sensor (interrupt based)
		uint16_t wheelPeriod = getWheelSensorPeriod();
		rio_tx[RIO_TX_WHEEL_PERIOD_H] = (wheelPeriod >> 8) & 0x00FF;
		rio_tx[RIO_TX_WHEEL_PERIOD_L] = wheelPeriod & 0x00FF;
		uint16_t distance = getDistanceCompleted();
		rio_tx[RIO_TX_DISTANCE_H] = (distance >> 8) & 0x00FF;
		rio_tx[RIO_TX_DISTANCE_L] = distance & 0x00FF;		
		// Receive and parse RS485, Sync RS232 to RS485, then react on the received data and transmit
		rs485_sync();		
		if(steering_rx[STEERING_PARTY_TIME]) {
			setHornHigh();
		} 
		else {
			setHornLow();
		}		
		// RS232 data is received in interrupt
		if(rio_rx[RIO_RX_ALIVE] != 1) rioHasStopped++;
		else rioHasStopped = 0;
		// Transmit to RIO over RS232
		rs232_tx();
		// Transmit to Bluetooth
		if(time - lastBlueSync >= BLUE_SYNC_PERIOD) {
			blue_tx();
			lastBlueSync = time;
		}
		// LED toggle
		LED3Toggle();
	}
	// END OF MAIN WHILE
}

static inline void blue_tx(void) {
	blue_puts(ANDROID_START_STRING);
	sprintf(blue_tx_buffer,
	",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,",
	rio_rx[RIO_RX_WATER_TEMP],
	rio_rx[RIO_RX_OIL_TEMP],
	front_light_rx[FL_RX_BRAKE],
	(rio_rx[RIO_RX_RPM_H] << 8 | rio_rx[RIO_RX_RPM_L]),
	rio_rx[RIO_RX_LAMBDA],
	rio_rx[RIO_RX_KMH],
	rio_rx[RIO_RX_GEAR],
	getPreviousAdc4Value(),	// BATTERY_VOLTAGE
	(rio_rx[RIO_RX_KMPERL_H] << 8 | rio_rx[RIO_RX_KMPERL_L]),
	steering_rx[STEERING_BURN],
	rio_rx[RIO_RX_ALIVE],
	rio_rx[RIO_RX_STARTER],
	message
	);
	blue_puts(blue_tx_buffer);
	blue_puts(ANDROID_END_STRING);
}

static inline void measureBatteryVoltage(void) {
	uint16_t mini_voltage = adc_read(4);	// Max value is 1024 == 15V
	if(mini_voltage < 512 && mini_voltage > 35) { // Voltage is below 7.5V but above 0.5V
		// Emergency button must have been pressed
		miniVoltageTooLow++;
		if(miniVoltageTooLow > 5) {
			rio_tx[RIO_TX_STOP] = 1;
			LED5On();
		} 
	}
	else {
		miniVoltageTooLow = 0;
		rio_tx[RIO_TX_STOP] = 0;
		LED5Off();
	}
}

static inline void readFromBluetooth(void) {
	// Read from Bluetooth	// TODO Cannot currently read due to CTS
	//uint16_t blueAvailableBytes = blue_available();
	//if(blueAvailableBytes > 0) {
	//blue_read_buffer(blue_buffer, blueAvailableBytes);
	//blue_puts(blue_buffer);
	//}
}