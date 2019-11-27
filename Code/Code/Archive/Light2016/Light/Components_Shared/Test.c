/*
 * Test.c
 *
 * Created: 2016-06-12 20:38:33
 *  Author: kry
 */ 

#include "global.h"
#include <avr/io.h>
#include <util/delay.h>
#include "RS485.h"

#include "Test.h"

void uartReciveSpamTest(void){
	rs485_Init();
	rs485_DisableInterrupt();
	rs485_ReceiveMode();
	while(1){
		
	}
}

void uartEchoTest(void){
	rs485_Init();
	rs485_DisableInterrupt();
	rs485_ReceiveMode();
	uint8_t received;
	uint8_t index = 0;
	uint8_t allReceived[30];
	while(1){
		index = 0;
		received = rs485_receive();
		if(received != 0x00){
			allReceived[index] = received;
			index = 1;
			for(uint16_t i = 0; i < 20000; i++){
				if(UCSR0A & (1<<RXC0)){
					i = 0;
					received = rs485_receive();
					allReceived[index] = received;
					index++;
				}
			}
			
			rs485_TransmitMode();
			rs485_Transmit(RS485_RESPONSE_START_1);
			rs485_Transmit(RS485_RESPONSE_START_2);
			for(int i = 0; i < 30; i++){
				if(allReceived[i] == 0){
					rs485_Transmit(i);
				}
				else{
					rs485_Transmit(allReceived[i]);
					allReceived[i] = (char)0;
				}
				
			}
			rs485_WaitForTransmit();
			rs485_ReceiveMode();
		}
	}
}

void uartSpamTest(void){
	rs485_Init();
	rs485_DisableInterrupt();
	rs485_TransmitMode();
	while(1){
		rs485_Transmit(0x42);
		_delay_us(100);
	}
}