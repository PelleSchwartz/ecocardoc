/*
 * rs485protocol.c
 *
 * Created: 22/05/2016 14:39:16
 *  Author: Kim
 */ 

#include "../asf.h"
#include "DataValidation.h"
#include "../Hardware/RS485.h"
#include "Hardware/LEDs.h"

#include "rs485protocol.h"

void _transmitCommand(uint8_t id);
void _receiveCommand(uint8_t id);
void _sendHeader(uint8_t id, uint8_t operation);

void initiateRS485(void) {
	rs485_Init();
}

//The new (maybe temporary) send packet function.
void rs485_send_packet(uint8_t id, uint8_t operation, uint8_t local, uint8_t dest, uint8_t len) {
	if(operation == OP_WRITE) { //Write task
		_transmitCommand(id);
	}
	else if (operation == OP_READ){ //Read task
		_receiveCommand(id);
	}
}

void rs485_send_packet_old(uint8_t id, uint8_t operation, uint8_t local, uint8_t dest, uint8_t len) {
	unsigned char r = 0x00;
	unsigned char i, p[6];
	p[0] = 0xA5;
	p[1] = ((id & 0x7F) << 1) | (operation & 0x01);
	p[2] = dest;
	p[3] = len;
	
	/*************************
	** Send header
	*************************/
	SETBIT(UCSR1A, TXC1);	// Clear TXC flag if it is already set
	//while(TESTBIT(UCSR3A, RXC3)) rs485_receive(); //while(UCSR3A & ) rs485_receive();
	rs485_TransmitMode();
	for(i=0;i<4;i++) { 
		rs485_Transmit(p[i]);
	}	
	
	while(!(UCSR1A & (1 << TXC1)));					// Wait for TX Complete Flag
	UCSR1A |= 1<<TXC1;								// Clear TX Complete Flag
	rs485_ReceiveMode();
	r = UDR1;
	
	/*************************
	** Handle specific task
	*************************/
	if(operation) { //Write task
		
		rs485_TransmitMode();
		
		for(i=0;i<len;i++){
				
			switch (id){
				case ID_FRONT_LIGHT:
				rs485_Transmit(light_F_reg[local+i]);	
				break;
				
				case ID_REAR_LIGHT:
				rs485_Transmit(light_B_reg[local+i]);
				break;
				
				case ID_STEERING_WHEEL:
				rs485_Transmit(steering_reg_w[local+i]);	
				break;
			}				
		} 
		while(!TESTBIT(UCSR1A, TXC1));
		UCSR1A |= 1<<TXC1;
		rs485_ReceiveMode();
		r = UDR1;
	}
	else { //Read task
		for(i=0;i<len;i++) {
			timeout = RX_TIMEOUT;
			while(!TESTBIT(UCSR1A, RXC1)){
				if(--timeout<2){
					//LED3On();
					//_delay_ms(500);
					//LED3Off();
					return;
					}
				}	// If timeout occur, ignore this client until next round
			switch (id){
				case ID_FRONT_LIGHT:
					light_F_reg[local+i] = rs485_receive();
				break;
				
				case ID_REAR_LIGHT:
					light_B_reg[local+i] = rs485_receive();
				break;
				
				case ID_STEERING_WHEEL:
					steering_reg[local+i] = rs485_receive();
				break;			
			}
			
		}
		_delay_ms(1);
	}

}

//will get that local and dest removed later....
void rs485_send_packet2(uint8_t id, uint8_t operation, uint8_t dest, uint8_t len) {
	unsigned char r = 0x00;
	unsigned char i, p[4];
	p[0] = 0xA5;
	p[1] = ((id & 0x7F) << 1) | (operation & 0x01);
	p[2] = dest;
	p[3] = len;  
	
	unsigned char inTemp[22] = {0x00};
	unsigned char checkSum[3] = {0x00};
	
	/*************************
	** Send header
	*************************/
	SETBIT(UCSR1A, TXC1);	// Clear TXC flag if it is already set
	//while(TESTBIT(UCSR3A, RXC3)) rs485_receive(); //while(UCSR3A & ) rs485_receive();
	rs485_TransmitMode();
	for(i=0;i<4;i++) { 
		rs485_Transmit(p[i]);
	}	
	if(!operation){//if read
		addCheckValue2(p, 4, NULL, 0, checkSum, 0);
		for(i=0;i<2;i++) {
			rs485_Transmit(checkSum[i]);
		}
	}
	
	while(!(UCSR1A & (1 << TXC1)));					// Wait for TX Complete Flag
	UCSR1A |= 1<<TXC1;								// Clear TX Complete Flag
	rs485_ReceiveMode();
	r = UDR1;
	
	/*************************
	** Handle specific task
	*************************/
	if(operation) { //Write task
		
		rs485_TransmitMode();
		
		for(i=0;i<len;i++){	
			switch (id){
				case ID_FRONT_LIGHT:
				rs485_Transmit(light_F_reg[i]);
				break;
				
				case ID_REAR_LIGHT:
				rs485_Transmit(light_B_reg[i]);
				break;
				
				case ID_STEERING_WHEEL:
				rs485_Transmit(steering_reg_w[i]);	
				break;
				
			}				
		} 
		switch (id){
			case ID_FRONT_LIGHT:
			addCheckValue2(p, 4, light_F_reg, len, checkSum, 0);
			break;
			case ID_REAR_LIGHT:
			addCheckValue2(p, 4, light_B_reg, len, checkSum, 0);
			break;
			case ID_STEERING_WHEEL:
			addCheckValue2(p, 4, steering_reg_w, len, checkSum, 0);
			break;
		}
		int temp = calcExstraSpace(len);
		for(int i = 0; i < temp; i++){
			rs485_Transmit(checkSum[i]);
		}
		
		
		while(!TESTBIT(UCSR1A, TXC1));
		UCSR1A |= 1<<TXC1;
		rs485_ReceiveMode();
		r = UDR1;
	}
	else { //Read task
		int tot = len + calcExstraSpace(len);
		for(i=0;i<tot;i++) {
			timeout = RX_TIMEOUT;
			while(!TESTBIT(UCSR1A, RXC1)){
				if(--timeout<2){
					return;
					}
				}	// If timeout occur, ignore this client until next round
			
			inTemp[i] = rs485_receive();
			
		}
		_delay_ms(1);
		
		if(validateCheckValue(inTemp, len, (tot-len))){ //checks if checksum is correct.
			for(int i = 0; i < len; i++){
				switch (id){
					case ID_FRONT_LIGHT:
					light_F_reg[i] = inTemp[i];
					break;
					
					case ID_REAR_LIGHT:
					light_B_reg[i] = inTemp[i];
					break;
					
					case ID_STEERING_WHEEL:
					steering_reg[i] = inTemp[i];
					break;

				}
			}
		}
	}
}

void _sendHeader(uint8_t id, uint8_t operation){
	SETBIT(UCSR1A, TXC1);	//Clear TXC flag if it is already set
	
	rs485_Transmit(0xA5);
	rs485_Transmit(((id & 0x7F) << 1) | (operation & 0x01));
}

void _transmitCommand(uint8_t id){
	uint8_t len = 0;
	
	switch (id){
		case ID_FRONT_LIGHT:
		len = FRONT_LIGHT_REGSIZE;
		break;
		
		case ID_REAR_LIGHT:
		len = BACK_LIGHT_REGSIZE;
		break;
		
		case ID_STEERING_WHEEL:
		len = STEERING_REGSIZE_W;
		break;
	}
	
	rs485_TransmitMode();
	_sendHeader(id, OP_WRITE);
	
	for(uint8_t i=0; i<len; i++){
		
		switch (id){
			case ID_FRONT_LIGHT:
			rs485_Transmit(light_F_reg[i]);
			break;
			
			case ID_REAR_LIGHT:
			rs485_Transmit(light_B_reg[i]);
			break;
			
			case ID_STEERING_WHEEL:
			rs485_Transmit(steering_reg_w[i]);
			break;
		}
	}
	
	rs485_WaitTransmit();
	rs485_ReceiveMode();
}

void _receiveCommand(uint8_t id){
	uint8_t len = 0;
	uint8_t shouldNotExist;
	
	rs485_TransmitMode();
	_sendHeader(id, OP_READ);
	rs485_WaitTransmit();
	rs485_ReceiveMode();
	
	switch (id){
		case ID_FRONT_LIGHT:
		len = FL_OUT_BYTESIZE;
		break;
		
		case ID_REAR_LIGHT:
		len = 0;
		break;
		
		case ID_STEERING_WHEEL:
		len = STEERING_REGSIZE;
		break;
	}
	
	for(uint8_t i=0; i<len; i++) {
		timeout = RX_TIMEOUT;
		while(!TESTBIT(UCSR1A, RXC1)){
			if(--timeout<2){
				return;
			}
		}		
		switch (id){
			case ID_FRONT_LIGHT:
			light_F_out_reg[i] = rs485_receive();
			break;
			
			case ID_REAR_LIGHT:
			shouldNotExist = rs485_receive();
			break;
			
			case ID_STEERING_WHEEL:
			steering_reg[i] = rs485_receive();
			break;
		}
		
	}
}

void rs485_interruptReceive(unsigned char data){
	//Unused because interrupts are disabled.
}