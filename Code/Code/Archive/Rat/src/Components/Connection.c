/*
* Connection.c
*
* Created: 2016-05-14 20:15:18
*  Author: kry
*/

#include "../global.h"
#include "../Hardware/RS485.h"
#include "Connection.h"
#include <avr/interrupt.h>

static void _sendData(void);
static void _updateData(void);
static void _createValidateFor(volatile uint8_t * validateData, volatile uint8_t dataLength);
static uint8_t _checkValidateFor(volatile uint8_t * validateData, volatile uint8_t dataLength);
static uint8_t _validateLength(uint8_t dataLength);

enum {
	STATE_WAITING,
	STATE_CHECKING_ADDRESS,
	STATE_RECEIVING,
	STATE_VALIDATE_RECEIVED,
	STATE_VALIDATE_SEND,
	};

static volatile uint8_t _inDataSize = 0;
static volatile uint8_t _outDataSize = 0;
static volatile uint8_t * _inData = NULL;
static volatile uint8_t * _outData = NULL;

static volatile uint8_t _inTempSize = 0;
static volatile uint8_t _inTempValidateSize = 0;
static volatile uint8_t _outValidateSize = 0;
static volatile uint8_t * _inTempData = NULL;
static volatile uint8_t * _tempValidate = NULL;

static volatile uint8_t _unitID = 0;
static volatile uint8_t _anyChanges = 0;
static volatile uint8_t _outDataOld = 0;

//As the name says, initiate the connection.
//Needs the data size for in and out data, and the address it receives data on.
//The address should be 7 bits long at most (0-127).
void connection_initiate(uint8_t inDataSize, uint8_t outDataSize, uint8_t unitID){
	cli();
	rs485_Init();
	rs485_ReceiveMode();
	rs485_EnableInterrupt();
	
	_unitID = unitID;
	
	_inDataSize = inDataSize;
	_outDataSize = outDataSize;
	
	_inTempSize = RS485_OFFSET + _inDataSize;
	_inTempValidateSize = _validateLength(_inTempSize);
	_outValidateSize = _validateLength(_outDataSize);
	
	if(_inData){ //Shoul never be used, since initiate only should be used once.
		free((void *)_inData);
		free((void *)_inTempData);
		free((void *)_tempValidate);
	}
	if(_outData){
		free((void *)_outData);
	}
	
	if(_inDataSize){
		_inData = malloc(_inDataSize);
		_inTempData = malloc(_inTempSize);
		_tempValidate = malloc(_inTempValidateSize); //<<< _outValidateSize should also matter
	}
	if(_outDataSize){
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

uint8_t connection_GetOutData(int bytenr){
	if(bytenr < _outDataSize){
		return _outData[bytenr];
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

uint8_t connection_OldOutData(void){
	uint8_t temp = _outDataOld;
	_outDataOld = 0;
	return temp;
}

//Basically the protocol, and which steps it contains.
//This is called for every package received, in that way it acts kinda like a loop, one iteration for every package :P
//Invoked by rs485 module when receive interrupt.
void rs485_autoReceive(unsigned char data){
	static volatile uint8_t state = STATE_WAITING;
	static volatile uint8_t dataIndex = 0;
	static volatile uint8_t validateIndex = 0;
	
	switch(state){
		case STATE_WAITING: //Testing something is being sent.
		{
			if(data == RS485_START){
				state = STATE_CHECKING_ADDRESS;
				_inTempData[0] = data;
			}
		}
		break;
		
		case STATE_CHECKING_ADDRESS: //Checking ID, and if it is write or read.
		{
			_inTempData[1] = data;
			if((data >> 1) == _unitID){
				if((data & 0x01) == OP_WRITE){
					state = STATE_RECEIVING;
					dataIndex = 0;
				}
				else if((data & 0x01) == OP_READ){
					#ifdef VALIDATE_TRANSMISSIONS
					state = STATE_VALIDATE_SEND;
					validateIndex = 0;
					#else
					state = STATE_WAITING;
					_sendData();
					#endif
				}
				else{ //Should not be possible
					state = STATE_WAITING;
				}
			}
			else{
				state = STATE_WAITING;
			}
		}
		break;
		
		case STATE_RECEIVING: //Receiving data.
		{
			if(dataIndex < _inDataSize){
				_inTempData[dataIndex + RS485_OFFSET] = data;
				dataIndex++;
			}
			
			if(dataIndex == _inDataSize){
				#ifdef VALIDATE_TRANSMISSIONS
					validateIndex = 0;
					state = STATE_VALIDATE_RECEIVED;
				#else
					_updateData();
					state = STATE_WAITING;
				#endif
			}
		}
		break;
		
		case STATE_VALIDATE_RECEIVED:
		{
			if(validateIndex < _inTempValidateSize){
				_tempValidate[validateIndex] = data;
				validateIndex++;
			}
			
			if(validateIndex == _inTempValidateSize){
				uint8_t valid = _checkValidateFor(_inTempData, _inTempSize);
				
				if(valid){
					_updateData();
				}
				
				state = STATE_WAITING;
			}
		}
		break;
		
		case STATE_VALIDATE_SEND:
		{
			if(validateIndex < _outValidateSize){
				_tempValidate[validateIndex] = data;
				validateIndex++;
			}
			
			if(validateIndex == _outValidateSize){
				uint8_t valid = _checkValidateFor(_inTempData, RS485_OFFSET);
				
				if(valid){
					_sendData();
				}
				
				state = STATE_WAITING;
			}
		}
		break;
	}
}

//Xor all bits in a byte.
//Returns 1 if odd bits.
//Returns 0 if even bits.
static uint8_t _xorBits(uint8_t testSubject){
	testSubject ^= testSubject >> 1;
	testSubject ^= testSubject >> 2;
	testSubject ^= testSubject >> 4;
	testSubject &= 0x01;
	return testSubject;
}

//Formula for calculating bytes needed to validate.
static uint8_t _validateLength(uint8_t dataLength){
	if(dataLength){
		return (1 + ((7 + dataLength) / 8));
	}
	return 0;
}

//How the validation is created, before data is send.
static void _createValidateFor(volatile uint8_t * validateData, volatile uint8_t dataLength){
	static volatile uint8_t validIndex = 0;
	
	//vertical checksum
	for(int i = 0; i < dataLength; i++){
		_tempValidate[validIndex] = validateData[i];
	}
	
	//Horizontal checksum
	validIndex = 1;
	_tempValidate[validIndex] = 0;
	uint8_t counter = 7;
	for(int i = 0; i < dataLength; i++){
		uint8_t temp = validateData[i];
		temp = _xorBits(temp);

		_tempValidate[validIndex] |=  (temp << counter);
		if(counter-- == 0){
			counter = 7;
			validIndex++;
			_tempValidate[validIndex] = 0;
		}
	}
}

//How things get validated.
static uint8_t _checkValidateFor(volatile uint8_t * validateData, volatile uint8_t dataLength){
	uint8_t validLength = _validateLength(dataLength);
	uint8_t temp = 0;
	static volatile uint8_t validIndex = 0;
	
	//vertical checksum
	for(int i = 0; i < dataLength; i++){
		_tempValidate[validIndex] ^= validateData[i];
	}
	
	//Horizontal checksum
	validIndex = 1;
	uint8_t counter = 7;
	for(int i = 0; i < dataLength; i++){
		temp = validateData[i];
		temp = _xorBits(temp);

		_tempValidate[validIndex] ^=  (temp << counter);
		if(counter-- == 0){
			counter = 7;
			validIndex++;
		}
	}
	
	//Checking checksum.
	uint8_t result = 0;
	for(int i = 0; i < validLength; i++){
		result |= _tempValidate[i];
	}
	
	return (!result);
}

//What happens when data is send.
static void _sendData(void){
	rs485_TransmitMode();
	
	#ifdef VALIDATE_TRANSMISSIONS //without data validate
		_createValidateFor(_outData, _outDataSize);
	#endif
	
	for(int i = 0; i < _outDataSize; i++){
		rs485_Transmit(_outData[i]);
	}
	
	#ifdef VALIDATE_TRANSMISSIONS //without data validate
		int tempValSize = _validateLength(_outDataSize);
		for(int i = 0; i < tempValSize; i++){
			rs485_Transmit(_tempValidate[i]);
		}
	#endif
	
	rs485_WaitForTransmit();
	rs485_ReceiveMode();
	_outDataOld = 1;
}

static void _updateData(void){
	for(uint8_t i = 0 ; i < _inDataSize ; i++){
		_inData[i] = _inTempData[i + RS485_OFFSET];
	}
	_anyChanges = 1;
}