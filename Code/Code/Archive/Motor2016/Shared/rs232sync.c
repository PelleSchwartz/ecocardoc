/*
* rs232sync.c
*
* Created: 21/06/2016 21:32:15
*  Author: Henning
*/

#include "global.h"
#include "LEDs.h"
#include "inout.h"
#include "rs232.h"
#include "rs232sync.h"
#include "bluetooth.h"

// Function Prototypes
static inline void _rs232_autoReceive(unsigned char data);

// Evil volatile variables
static volatile uint8_t _inTempData[RIO_RX_REGSIZE + 20];
static volatile uint8_t state = RS232_STATE_WAITING;
static volatile uint8_t dataIndex = 0;
static volatile uint8_t packetSize = 0;
static volatile bool isRioAlive;

static uint8_t car;

// Receive Interrupt
ISR(RS232_RX_vect) {
	_rs232_autoReceive(UDR2);
}

// Set the car ID
void rs232_set_car(uint8_t carId) {
	car = carId;
}

/*
This function sends data to the RIO.
*/
void rs232_tx(void) {
	// Steering registers are synchronized to RIO register in rs485sync interrupt
	// 2 byte Sync
	// 1 byte Length
	// x byte Data

	rs232_putc(RS232_START_1);				// Sync
	rs232_putc(RS232_START_2);				// Sync
	rs232_putc(RIO_TX_REGSIZE);				// Length
	for(unsigned char i=0; i<RIO_TX_REGSIZE; i++) {
		rs232_putc(rio_tx[i]);				// Data
	}
}

/*
This function reads the received data and performs actions based on it.

Returns: True if RIO is alive.
*/
static inline void _rs232_react() {
	// Receive starter signal from RIO
	if(rio_rx[RIO_RX_STARTER] == 1 && rio_rx[RIO_RX_ALIVE] == 1) {
		setStarterHigh();			
		if(car == DTU_INNOVATOR) {
			setHornHigh();	// Horn == Starter light on Innovator
		}
		LED4On();
	}
	else {
		setStarterLow();		
		if(car == DTU_INNOVATOR) {
			setHornLow();	// Horn == Starter light on Innovator
		}
		LED4Off();
	}
	// Check if RIO is alive
	isRioAlive = (rio_rx[RIO_RX_ALIVE] == 1);
}

static inline void _update_rs232_data(void) {
	// When receiving data from RS232 RIO
	for(uint8_t i = 0 ; i < packetSize ; i++) {
		rio_rx[i] = _inTempData[i];
	}
	_rs232_react();	
}

// Called by interrupt
static inline void _rs232_autoReceive(unsigned char data) {
	switch(state){
		case RS232_STATE_WAITING:
		{
			if(data == RS232_START_1) {
				state = RS232_STATE_WAITING_FOR_SECOND_START;
			}
		}
		break;		
		case RS232_STATE_WAITING_FOR_SECOND_START:
		{
			if(data == RS232_START_2) {
				state = RS232_STATE_GET_PACKET_SIZE;
			} 
			else {
				state = RS232_STATE_WAITING;
			}
		}
		break;		
		case RS232_STATE_GET_PACKET_SIZE:
		{
			packetSize = data;
			dataIndex = 0;	
			state = RS232_STATE_RECEIVING;		
		}
		break;
		case RS232_STATE_RECEIVING: // Receiving data.
		{
			if(dataIndex < packetSize) {
				_inTempData[dataIndex] = data;
				dataIndex++;
			}			
			if(dataIndex == packetSize){
				_update_rs232_data();
				state = RS232_STATE_WAITING;
				// Toggle LED 6 (optimized)
				PORTD ^= (1 << PD7);
			}
		}
		break;
	}
}