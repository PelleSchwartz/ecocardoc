/*
* Connection.c
*
* Created: 2016-05-14 20:15:18
*  Author: kry
*/

#include "global.h"
#include "RS485.h"
#include "Connection.h"
#include "leds.h"

#define RS485_OFFSET 0

static void _sendData(void);
static void _updateData(void);

enum {
	STATE_WAITING,
	STATE_WAITING_FOR_SECOND_START,
	STATE_CHECKING_ADDRESS,
	STATE_RECEIVING
};

static volatile uint8_t _inDataSize = 0;
static volatile uint8_t _outDataSize = 0;
static volatile uint8_t * _inData = NULL;
static volatile uint8_t * _outData = NULL;

static volatile uint8_t _inTempSize = 0;
static volatile uint8_t * _inTempData = NULL;

static volatile uint8_t _unitID = 0;
static volatile uint8_t _anyChanges = 0;

//As the name says, initiate the connection.
//Needs the data size for in and out data, and the address it receives data on.
//The address should be 7 bits long at most (0-127).
void connection_initiate(uint8_t inDataSize, uint8_t outDataSize, uint8_t unitID) {
	cli();
	rs485_Init();
	rs485_ReceiveMode();
	rs485_EnableInterrupt();
	
	_unitID = unitID;
	
	_inDataSize = inDataSize;
	_outDataSize = outDataSize;
	
	_inTempSize = RS485_OFFSET + _inDataSize;
	
	if(_inData) { //Should never be used, since initiate only should be used once.
		free((void *)_inData);
		free((void *)_inTempData);
	}
	if(_outData) {
		free((void *)_outData);
	}
	
	if(_inDataSize) {
		_inData = malloc(_inDataSize);
		_inTempData = malloc(_inTempSize);
	}
	if(_outDataSize) {
		_outData = malloc(_outDataSize);
	}
	sei();
}

uint8_t connection_GetInData(int byteNr){
	if(byteNr < _inDataSize){
		return _inData[byteNr];
	}
	return 0;
}

void connection_SetOutData(int byteNr, uint8_t newValue){
	if(byteNr < _outDataSize){
		_outData[byteNr] = newValue;
	}
}

void connection_SetOutDataBit(int byteNr, int bitNr, uint8_t highLow){
	if(byteNr < _outDataSize){
		if(highLow){
			SETBIT(_outData[byteNr], bitNr);
		}
		else{
			CLRBIT(_outData[byteNr], bitNr);
		}
	}
}

uint8_t connection_AnyChanges(void){
	uint8_t temp = _anyChanges;
	_anyChanges = 0;
	return temp;
}

// Evil volatile variables
static volatile uint8_t state = STATE_WAITING;
static volatile uint8_t dataIndex = 0;

//Basically the protocol, and which steps it contains.
//This is called for every package received, in that way it acts kinda like a loop, one iteration for every package :P
//Invoked by rs485 module when receive interrupt.
static void rs485_AutoReceive(unsigned char data) {	
	switch(state) {
		case STATE_WAITING:
		{
			if(data == RS485_START_1) {
				state = STATE_WAITING_FOR_SECOND_START;
			}
		}
		break;
		case STATE_WAITING_FOR_SECOND_START:
		{
			if(data == RS485_START_2) {
				state = STATE_CHECKING_ADDRESS;
			}
			else {
				state = STATE_WAITING;
			}
		}
		break;		
		case STATE_CHECKING_ADDRESS: // Checking ID, and if it is write or read.
		{
			if((data >> 1) == _unitID) {
				if((data & 0x01) == OP_WRITE) {
					state = STATE_RECEIVING;
					dataIndex = 0;
				}
				else if((data & 0x01) == OP_READ) {
					_sendData();
					state = STATE_WAITING;	
				}
				else { //Should not be possible
					state = STATE_WAITING;
				}
			}
			else {
				state = STATE_WAITING;
			}
		}
		break;		
		case STATE_RECEIVING: //Receiving data.
		{
			if(dataIndex < _inDataSize) {
				_inTempData[dataIndex] = data;
				dataIndex++;
			}			
			if(dataIndex == _inDataSize) {
				_updateData();
				state = STATE_WAITING;
			}
		}
		break;
	}
}

void _send_header(void) {
	rs485_Transmit(RS485_RESPONSE_START_1);
	rs485_Transmit(RS485_RESPONSE_START_2);
}

//What happens when data is send.
static void _sendData(void) {
	// This is needed to make it work. Further work is required to determine why.
	delay_us(30);
	// Set Datadir to TX
	rs485_TransmitMode();	
	// Transmit Data
	_send_header();
	for(int i = 0; i < _outDataSize; i++){
		rs485_Transmit(_outData[i]);
	}
	// Wait for transmit to complete
	rs485_WaitForTransmit();
	// Set Datadir to RX
	rs485_ReceiveMode();
}

void _updateData(void) {
	for(uint8_t i = 0 ; i < _inDataSize ; i++){
		_inData[i] = _inTempData[i];
		
	}
	
	_anyChanges = 1;
}

ISR(USART_RX_vect){
	rs485_AutoReceive(UDR0);
}