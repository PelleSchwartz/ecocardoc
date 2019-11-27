/*
* RS485.c
*
* Created: 01/05/2016 17:32:15
*  Author: Kim
*/

#include "global.h"
#include "LEDs.h"
#include "rs232.h"
#include "rs485.h"
#include "rs485sync.h"
#include "adc.h"
#include "delay.h"

// Function prototype
static inline void _rs485_react(void);
static void _rs485_execute(uint8_t id, uint8_t operation);
static inline void _rs485_tx(uint8_t id);
static inline void _rs485_rx(uint8_t id);
static inline void _rs232_to_rs485_sync(void);
static void _setBrakeLight(uint8_t signal);
static void _setHorn(uint8_t signal);
static void _setStarterLight(uint8_t signal);
static inline void _rs485_autoReceive(unsigned char data);

// Evil volatile variables
static volatile uint8_t _inTempData[STEERING_RX_REGSIZE + 10];
static volatile uint8_t state = RS485_STATE_WAITING;
static volatile uint8_t dataIndex = 0;
static volatile uint8_t receivingDataFrom = 0;
static volatile uint8_t lastRatRespons;

// Receive Interrupt
ISR(RS485_RX_vect) {
	_rs485_autoReceive(UDR1);
}

void rs485_sync(void) {
	// Read data from STEERING and FRONT LIGHT
	lastRatRespons++;
	if(lastRatRespons>1){
		rio_tx[RIO_TX_BURN] = 0;
		steering_rx[STEERING_BURN] = 0;
	}
	
	_rs485_execute(RS485_ID_STEERING_WHEEL, OP_READ);
	_rs485_execute(RS485_ID_FRONT_LIGHT, OP_READ);
	// Interrupt sync rs485_to_rs232
	// Sync applicable STEERING data to LIGHTS
	_rs485_react();
	// Sync RIO data to RS485	
	_rs232_to_rs485_sync();
	// Send data to STEERING; FRONT LIGHT and BACK LIGHT
	_rs485_execute(RS485_ID_STEERING_WHEEL, OP_WRITE);
	_rs485_execute(RS485_ID_FRONT_LIGHT, OP_WRITE);
	_rs485_execute(RS485_ID_BACK_LIGHT, OP_WRITE);
}

// Sync received data from RIO
static inline void _rs232_to_rs485_sync(void) {	
	_setStarterLight(rio_rx[RIO_RX_STARTER]);
	// Sync applicable RIO data to STEERING
	steering_tx[STEERING_KMH] = rio_rx[RIO_RX_KMH];
	steering_tx[STEERING_RPM_H] = rio_rx[RIO_RX_RPM_H];
	steering_tx[STEERING_RPM_L] = rio_rx[RIO_RX_RPM_L];
	steering_tx[STEERING_WATER_TEMP] = rio_rx[RIO_RX_WATER_TEMP];
	steering_tx[STEERING_OIL_TEMP] = rio_rx[RIO_RX_OIL_TEMP];
	steering_tx[STEERING_GEAR] = rio_rx[RIO_RX_GEAR];
	steering_tx[STEERING_LAMBDA] = rio_rx[RIO_RX_LAMBDA];
	steering_tx[STEERING_ENCODER_FAN] = 0;
	steering_tx[STEERING_BAT_VOLTAGE_H] = (getPreviousAdc4Value() >> 8) & 0x00FF;
	steering_tx[STEERING_BAT_VOLTAGE_L] = getPreviousAdc4Value() & 0x00FF;	
}

static void _rs485_execute(uint8_t id, uint8_t operation) {
	uint8_t n = 0;
	while(state == RS485_STATE_RECEIVING && n < RS485_RX_WAIT_TIMEOUT) {
		delay_ms(1);
		n++;
	}
	if(operation == OP_WRITE) {			// Write task
		_rs485_tx(id);
	}
	else if(operation == OP_READ ) {	// Read task
		_rs485_rx(id);
	}
}

static void _send_header(uint8_t id, uint8_t operation) {
	rs485_putc(RS485_START_1);
	rs485_putc(RS485_START_2);
	rs485_putc(((id & 0x7F) << 1) | (operation & 0x01));
}

/*
This function sends data over RS485 to the device with the specified ID
*/
static inline void _rs485_tx(uint8_t id) {
	// Set Datadir to TX
	rs485_set_tx_mode();
	// Get data length
	uint8_t len = 0;
	switch(id) {
		case RS485_ID_FRONT_LIGHT:
		len = FRONT_LIGHT_TX_REGSIZE;
		break;
		case RS485_ID_BACK_LIGHT:
		len = BACK_LIGHT_TX_REGSIZE;
		break;
		case RS485_ID_STEERING_WHEEL:
		len = STEERING_TX_REGSIZE;
		break;
	}
	// Set Header
	_send_header(id, OP_WRITE);
	// Send data
	for(uint8_t i=0; i<len; i++) {
		switch (id) {
			case RS485_ID_FRONT_LIGHT:
			rs485_putc(front_light_tx[i]);
			break;
			case RS485_ID_BACK_LIGHT:
			rs485_putc(back_light_tx[i]);
			break;
			case RS485_ID_STEERING_WHEEL:
			rs485_putc(steering_tx[i]);
			break;
		}
	}
	// Wait for Transmit to complete
	rs485_wait_transmit();
	// Set Datadir back to RX
	rs485_set_rx_mode();
}

static inline void _update_rs485_data(void){
	// When receiving data from STEERING WHEEL
	switch(receivingDataFrom) {
		case RS485_ID_STEERING_WHEEL:
		{
			for(uint8_t i = 0 ; i < STEERING_RX_REGSIZE; i++) {
				steering_rx[i] = _inTempData[i];
			}
			rio_tx[RIO_TX_BURN] = steering_rx[STEERING_BURN];	// "RIO_START"
			rio_tx[RIO_TX_IDLE] = steering_rx[STEERING_IDLE];
			rio_tx[RIO_TX_CLOSED_LOOP] = steering_rx[STEERING_CLOSED_LOOP];
			rio_tx[RIO_TX_AUTOGEAR] = steering_rx[STEERING_AUTOGEAR];
			rio_tx[RIO_TX_POT_UP] = steering_rx[STEERING_POT_UP];
			rio_tx[RIO_TX_POT_DOWN] = steering_rx[STEERING_POT_DOWN];
			rio_tx[RIO_TX_NEUTRAL] = steering_rx[STEERING_NEUTRAL];
			rio_tx[RIO_TX_GEAR_UP] = steering_rx[STEERING_GEAR_UP];
			rio_tx[RIO_TX_GEAR_DOWN] = steering_rx[STEERING_GEAR_DOWN];
			lastRatRespons = 0;
		}
		break;
		case RS485_ID_BACK_LIGHT:
		// We currently are not receiving anything from BACKLIGHT
		break;
		case RS485_ID_FRONT_LIGHT:
		{
			for(uint8_t i = 0 ; i < FRONT_LIGHT_RX_REGSIZE; i++) {
				front_light_rx[i] = _inTempData[i];
			}
			rio_tx[RIO_TX_BRAKE] = front_light_rx[FL_RX_BRAKE];
		}
		break;
	}
}

// Called by interrupt
static inline void _rs485_autoReceive(unsigned char data) {
	// Get data length
	uint8_t len = 0;
	switch(receivingDataFrom) {
		case RS485_ID_FRONT_LIGHT:
		len = FRONT_LIGHT_RX_REGSIZE;
		break;
		case RS485_ID_BACK_LIGHT:
		len = BACK_LIGHT_RX_REGSIZE;
		break;
		case RS485_ID_STEERING_WHEEL:
		len = STEERING_RX_REGSIZE;
		break;
	}
	switch(state) {		
		case RS485_STATE_WAITING:
		{
			if(data == RS485_RESPONSE_START_1) {
				state = RS485_STATE_WAITING_FOR_SECOND_START;
			}
		}
		break;		
		case RS485_STATE_WAITING_FOR_SECOND_START:
		{
			if(data == RS485_RESPONSE_START_2) {
				state = RS485_STATE_RECEIVING;
				dataIndex = 0;
			}
			else {
				state = RS485_STATE_WAITING;
			}
		}
		break;		
		case RS485_STATE_RECEIVING: // Receiving data.
		{
			if(dataIndex < len) {
				_inTempData[dataIndex] = data;
				dataIndex++;
			}			
			if(dataIndex == len) {
				_update_rs485_data();
				state = RS485_STATE_RECEIVE_COMPLETE;
				// Toggle LED 4 (optimized)
				PORTC ^= (1 << PC1);
			}
		}
		break;
	}
}

static void _rs485_rx(uint8_t id) {
	// Set Datadir to TX
	rs485_set_tx_mode();
	// Set the ID
	receivingDataFrom = id;
	// Request data
	_send_header(id, OP_READ);
	// Wait for Transmit to complete
	rs485_wait_transmit();
	// Set Datadir back to RX
	rs485_set_rx_mode();
	uint8_t n = 0;
	// Leave it for the interrupt to actually receive and process data
	// These waits may not be necessary as long as the one in _rs485_execute is kept
	while(state != RS485_STATE_RECEIVE_COMPLETE && n < RS485_RX_WAIT_TIMEOUT) {
		delay_ms(1);
		n++;
	}
	state = RS485_STATE_WAITING;
}

// Sync received data from steering wheel
static void _rs485_react(void) {
	// Fan toggle
	if(steering_rx[STEERING_FAN]) {
		SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_FAN);
		SETBIT(lights_reg[LIGHTS_DIVERSE_FLAGS], LDF_FAN);
	}
	else {
		CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_FAN);
		CLRBIT(lights_reg[LIGHTS_DIVERSE_FLAGS], LDF_FAN);
	}
	// Wiper toggle
	if(steering_rx[STEERING_WIPER]) {
		SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_WIPER);
		SETBIT(lights_reg[LIGHTS_DIVERSE_FLAGS], LDF_WIPER);
	}
	else {
		CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_WIPER);
		CLRBIT(lights_reg[LIGHTS_DIVERSE_FLAGS], LDF_WIPER);
	}
	
	// Normal Light toggle
	if(steering_rx[STEERING_LIGHT_NORMAL]) {		
		SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_NORMAL_LIGHT);
		SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BACKLIGHT);
		SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_NORMAL_LIGHT);
		SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_FRONTLIGHT);
	}
	else {
		CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_NORMAL_LIGHT);
		CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BACKLIGHT);
		CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_NORMAL_LIGHT);
		CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_FRONTLIGHT);
	}
	// Turn Right toggle
	if(steering_rx[STEERING_BLINK_R]) {		
		SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNRIGHT_LIGHT);
		SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_RIGHT);
		SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_RIGHT);
		steering_tx[STEERING_F_BLINK_R] = 1;
	}
	else {
		CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNRIGHT_LIGHT);
		CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_RIGHT);
		CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_RIGHT);
		steering_tx[STEERING_F_BLINK_R] = 0;
	}
	// Turn Left toggle
	if(steering_rx[STEERING_BLINK_L]) {
		SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNLEFT_LIGHT);
		SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_LEFT);		
		SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_LEFT);
		steering_tx[STEERING_F_BLINK_L] = 1;
	}
	else {
		CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNLEFT_LIGHT);
		CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_LEFT);
		CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_LEFT);
		steering_tx[STEERING_F_BLINK_L] = 0;
	}
	// Horn momentary
	_setHorn(steering_rx[STEERING_HORN]);
	// Brake momentary
	_setBrakeLight(front_light_rx[FL_RX_BRAKE]);
}

static void _setHorn(uint8_t signal) {
	if(signal) {
		SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_HORN);
	}
	else {
		CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_HORN);
	}
}

static void _setBrakeLight(uint8_t signal) {
	if(signal) {
		SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BRAKELIGHT);
		SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_BRAKE_LIGHT);
	}
	else {
		CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BRAKELIGHT);
		CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_BRAKE_LIGHT);
	}
}

static void _setStarterLight(uint8_t signal) {
	if(signal) {
		SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_STARTER);
		SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_STARTER_LIGHT);
	}
	else {
		CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_STARTER);
		CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_STARTER_LIGHT);
	}
}

//static void _setRightSignal(uint8_t signal) {
	//if(signal) {
		//SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNRIGHT_LIGHT);
		//SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_RIGHT);
		//SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_RIGHT);
	//}
	//else {
		//CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNRIGHT_LIGHT);
		//CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_RIGHT);
		//CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_RIGHT);
	//}
//}
//
//static void _setLeftSignal(uint8_t signal) {
	//if(signal) {
		//SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNLEFT_LIGHT);
		//SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_LEFT);
		//SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_LEFT);
	//}
	//else {
		//CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_TURNLEFT_LIGHT);
		//CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_TURN_LEFT);
		//CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_LEFT);
	//}
//}
//
//static void _setNormalLight(uint8_t signal) {
	//// Back light
	//if(signal) {
		//SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BACKLIGHT);
		//SETBIT(back_light_tx[BACK_BYTEZERO], BL_B0_NORMAL_LIGHT);
	//}
	//else {
		//CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BACKLIGHT);
		//CLRBIT(back_light_tx[BACK_BYTEZERO], BL_B0_NORMAL_LIGHT);
	//}
	//// Front light
	//if(signal) {
		//SETBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_NORMAL_LIGHT);
		//SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_FRONTLIGHT);
	//}
	//else {
		//CLRBIT(front_light_tx[FRONT_BYTEZERO], FL_B0_NORMAL_LIGHT);
		//CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_FRONTLIGHT);
	//}
//}