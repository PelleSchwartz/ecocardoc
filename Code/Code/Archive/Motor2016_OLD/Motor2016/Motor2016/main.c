/*
* Motor2016.c
*
* Created: 02/05/2016 20:54:54
* Author : Henning
*/

#include "global.h"
#include "LEDs.h"
#include "tunes.h"
#include "bluetooth.h"
#include "rs232.h"
#include "timer.h"
#include "inout.h"
#include "adc.h"
#include "uart.h"
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#define DEBUG 1
#define TIMEOUT 50	// RS232 Received Timeout [ms]

// Functions
void receiveAndParseRs232(void);
void measureBatteryVoltage(void);
void readFromBluetooth(void);

// Local buffers
char blue_buffer[UART_RX0_BUFFER_SIZE];
char rs232_buffer[UART_RX2_BUFFER_SIZE];
uint8_t miniVoltageTooLow = 0;
uint8_t rioHasStopped = 0;

int main(void) {
	// INITIALIZE //
	// Initialize IO Pins
	io_init();
	// Initialize LEDs
	LED_init();
	// Initialize ADC
	adc_init();
	// Initialize RS232
	rs232_init();
	// Initialize Bluetooth
	blue_init();
	// Initialize Timer1 for counter
	//timer_init();	// TODO DOESN'T WORK
	// Initialize Buzzer
	tunes_init();
	// Initialize Globals
	global_init();
	// Enable global interrupts
	sei();
	
	// Local variables
	uint32_t counter = 0;
	
	delay_ms(1000);
	sing(STARTUP_MELODY_ID);
	if(DEBUG) blue_puts("Innovator has started\n\r");
	
	LED3On();
	
	while(1) {
		counter++;
		// Annoy everyone until everything is restarted
		if(rioHasStopped) {
			sing(WARNING_MELODY_ID);	// TODO (currently disabled)
		}
		// Read from RS232
		receiveAndParseRs232();
		// Receive starter signal from RIO
		if(rio_rx[RIO_REC_STARTER]) {
			setStarterHigh();
			setHornHigh();	// Horn == Starter light on Innovator
			LED4On();
		}
		else {
			setStarterLow();
			setHornLow();	// Horn == Starter light on Innovator
			LED4Off();
		}
		// Check if RIO has shut down
		if(rio_rx[RIO_REC_STOP]) {
			rioHasStopped = 1;		
		}
		// Read from the Burn button connected to Gear-Sensor
		uint16_t gSensorVoltage = digitalReadGSensor();		// ADC on ADC0
		if(gSensorVoltage) {
			if(DEBUG) blue_puts("BURN\n\r");
			LED2On();
			LED5On();
			rio_tx[RIO_BURN] = 1;
		}
		else {
			LED2Off();
			LED5Off();
			rio_tx[RIO_BURN] = 0;
		}
		// Read from Battery and external emergency stop sensor (VIN to Mini)
		measureBatteryVoltage();
		// Transmit to RS232 every second loop
		LED2Toggle();
		rs232_sync();
	}
}

void receiveAndParseRs232(void) {
	// Receive the data
	uint16_t i = 0;
	uint16_t n = 0;
	rs232_buffer[0] = '\0';
	// START READ WHILE
	while(n <= TIMEOUT) {
		while(rs232_available() == 0 && n <= TIMEOUT) {		// Timeout waiting for more data
			n++;
			delay_ms(1);
		}
		if(n >= TIMEOUT) {		// Total timeout
			break;
		}
		else {
			rs232_buffer[i] = rs232_read_data();
		}
		// End of data capturing
		i++;
		if(i >= UART_RX0_BUFFER_SIZE - 3) break;
	}
	if(i > 0)  {
		rs232_buffer[i] = '\n';
		rs232_buffer[i+1] = '\r';
		rs232_buffer[i+2] = '\0';
	}
	// END READ WHILE
	rs232_flush_buffer();
	// Parse RS232
	if(rs232_buffer[0] != '\0') {
		for(int i=0; i<UART_RX0_BUFFER_SIZE-3; i++)		 {
			if(rs232_buffer[i] == 0x55 && rs232_buffer[i+1] == 0xAA) {
				uint8_t sizeOfPackage = rs232_buffer[i+2];
				// Store the received package in rio_rx buffer
				uint8_t k = 0;
				for(int j=i+3; i<UART_RX0_BUFFER_SIZE; j++) {
					if(rs232_buffer[j] == '\n') break;
					if(k >= sizeOfPackage - 1) break;
					rio_rx[k] = rs232_buffer[j];
					k++;
				}
				break;
			}
		}
	}
}

void measureBatteryVoltage(void) {
	uint16_t mini_voltage = adc_read(8);	// Max value is 1024 == 15V
	if(mini_voltage < 512 && mini_voltage > 35) { // Voltage is below 7.5V but above 0.5V
		// Emergency button must have been pressed
		miniVoltageTooLow++;
		if(miniVoltageTooLow > 5) {
			rio_tx[RIO_STOP] = 1;
			LEDBlink(3, 3);
			sing(WARNING_MELODY_ID);
		}
	}
	else {
		miniVoltageTooLow = 0;
		rio_tx[RIO_STOP] = 0;
		LED3On();
	}
}

void readFromBluetooth(void) {
	// Read from Bluetooth	// TODO Cannot currently read due to CTS
	//uint16_t blueAvailableBytes = blue_available();
	//if(blueAvailableBytes > 0) {
	//blue_read_buffer(blue_buffer, blueAvailableBytes);
	//blue_puts(blue_buffer);
	//}
}



