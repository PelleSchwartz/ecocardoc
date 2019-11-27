/*
 * Test.h
 *
 * Created: 13-05-2015 16:04:47
 *  Author: Nicklas
 */ 


#ifndef TEST_H_
#define TEST_H_

#include "asf.h"
#include "LEDs.h"
#include "BilADC.h"
#include "rs485.h"
#include "rs232.h"
#include "Timer.h"
#include "DutyCycle.h"
#include "EEPROM.h"

void test_rs485(void);
void test2_rs485(void);
void test3_rs485(void);
void test4_rs485(void);
void freqTest(void);
void dutyCycleTest(void);
void batteryTest(void);
void uartreceiveTest(void);
void uartreceiveTest2(void);
void uartreceiveTestInterrupt(void);
void uartSendTest(void);
void testLED(void);

void testDataValidation1(void);
void testDataValidation2(void);

void freqTest(void){
	while(1){
		LED1Toggle();
		_delay_ms(1000);
	}
}

void test_rs485(void){
	light_reg[FRONT_LIGHT_STATE] = 0x00;
	light_reg[TURNSIG_RIGHT_STATE] = 0x00;
	light_reg[TURNSIG_LEFT_STATE] = 0x00;
	light_reg[LED1_STATE] = 0x00;
	light_reg[LED2_STATE] = 0x00;
	light_reg[WSVIPER_STATE] = 0x01;
	light_reg[AIRFAN_STATE] = 0x01;
	LED1On();
	LED2On();
	
	//And finally the program.
	while(1){
		//skal bare blinke med alle lys
		rs485_send_packet(ID_FRONT_LIGHT, OP_WRITE, 0, 0, FRONT_LIGHT_REGSIZE);
		
		//light_reg[FRONT_LIGHT_STATE] ^= 0x01;
		//light_reg[TURNSIG_RIGHT_STATE] ^= 0x01;
		//light_reg[TURNSIG_LEFT_STATE] ^= 0x01;
		//light_reg[LED1_STATE] ^= 0x01;
		//light_reg[LED2_STATE] ^= 0x01;
		light_reg[WSVIPER_STATE] = 0x01;
		light_reg[AIRFAN_STATE] = 0x01;
		
		_delay_ms(5000);
		LED1Toggle();
		LED2Toggle();
	}
}

void test2_rs485(void){
	light_B_reg[BACK_LIGHT_STATE] = 0x00;
	light_B_reg[STOP_LIGHT_STATE] = 0x00;
	light_B_reg[STARTER_LIGHT_STATE] = 0x00;
	light_B_reg[TURNSIG_B_RIGHT_STATE] = 0x00;
	light_B_reg[TURNSIG_B_LEFT_STATE] = 0x01;
	LED1On();
	
	//And finally the program.
	while(1){
		//skal bare blinke med alle lys
		rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
		
//		light_B_reg[BACK_LIGHT_STATE] ^= 0x01;
//		light_B_reg[STOP_LIGHT_STATE] ^= 0x01;
//		light_B_reg[STARTER_LIGHT_STATE] ^= 0x01;
		light_B_reg[TURNSIG_B_RIGHT_STATE] ^= 0x01;
		light_B_reg[TURNSIG_B_LEFT_STATE] ^= 0x01;
		_delay_ms(1000);
		LED1Toggle();
	}
}

void test3_rs485(void){
	
	//And finally the program.
	while(1){
		
		for(int i = 0; i < BACK_LIGHT_REGSIZE; i++){
			light_B_reg[i] = 0x01;
		}
		/*
		light_B_reg[BACK_LIGHT_STATE] = 0x01;
		light_B_reg[STOP_LIGHT_STATE] = 0x01;
		light_B_reg[STARTER_LIGHT_STATE] = 0x01;
		light_B_reg[TURNSIG_B_RIGHT_STATE] = 0x01;
		light_B_reg[TURNSIG_B_LEFT_STATE] = 0x01;
		*/
	
		rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
		
		
		for(int i = 0; i < BACK_LIGHT_REGSIZE; i++){
			light_B_reg[i] = 0x00;
		}
		
		
		_delay_ms(100);
		LED1On();
		//skal bare blinke med alle lys
//	while(1){
		rs485_send_packet(ID_REAR_LIGHT, OP_READ, 0, 0, BACK_LIGHT_REGSIZE);
	
		rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
		
		if(light_B_reg[BACK_LIGHT_STATE]){
			LED2On();
		}
		else{
			LED2Off();
		}
		if(light_B_reg[STOP_LIGHT_STATE]){
			LED3On();
		}
		else{
			LED3Off();
		}
		if(light_B_reg[STARTER_LIGHT_STATE]){
			LED4On();
		}
		else{
			LED4Off();
		}
		if(light_B_reg[TURNSIG_B_RIGHT_STATE]){
			LED5On();
		}
		else{
			LED5Off();
		}
		if(light_B_reg[TURNSIG_B_LEFT_STATE]){
			LED6On();
		}
		else{
			LED6Off();
		}
		
		_delay_ms(100);
		LED1Off();
		LED2Off();
		LED3Off();
		LED4Off();
		LED5Off();
		LED6Off();
		
	}
}

void test4_rs485(void){
	
	while(1){
		//ask steering wheel.
		rs485_send_packet(ID_STEERING_WHEEL, OP_READ, 0, 0, STEERING_REGSIZE);
		
		if(steering_reg[STEERING_BLINK_L]){
			LED1On();
		}
		else{
			LED1Off();
		}
		if(steering_reg[STEERING_BLINK_H]){
			LED2On();
		}
		else{
			LED2Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED3On();
		}
		else{
			LED3Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED4On();
		}
		else{
			LED4Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED5On();
		}
		else{
			LED5Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED6On();
		}
		else{
			LED6Off();
		}
	}
	
}

void dutyCycleTest(void){
//	uint16_t i1 = (7*2499)/100; //2499 = 100%
//	uint32_t i2 = (50*2499)/100;
//	uint16_t i3 = i2;
//	int i = 2400;
	while(1){		
		SetPWMDutyGear(1249);
		_delay_ms(3000);
		
		SetPWMDutyGear(833);
		_delay_ms(3000);
	}
	
}

void LED1Blink(int times);
void LED1Blink(int times) {
	LED1Off();
	_delay_ms(500);
	for(int i=0; i<times; i++) {
		LED1On();
		_delay_ms(200);
		LED1Off();
		_delay_ms(200);
	}
}

void batteryTest(){
	readBatterySensor();
	_delay_ms(2000);
	uint32_t volt = 0;
	volt = readBatterySensor();
	int blink = (volt*15)/0xFF;
	LED1Blink(blink);
	while(1){
	}
}

void uartreceiveTest(void){ //same as above, just with interrupts
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	cli();
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1);//|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,

	
	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	
	unsigned char data;
	while(1){
		while( !(UCSR1A & (1<<RXC1)) );
		LED1On();
		data = UDR1;
		if(data){
			LED2On();
		}
	}
}

void uartreceiveTest2(void){ //same as above, just with interrupts
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	/*
	cli();
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1);//|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,

	
	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	*/
	
	CLRBIT(PORTA, DDA3);
	unsigned char data;
	while(1){
		data = rs485_receive();
		switch (data){
			case 1:
				LED1On();
				break;
			case 2:
				LED2On();
				break;
			case 3:
				LED3On();
				break;
			case 4:
				LED4On();
				break;
			case 5:
				LED5On();
				break;
			case 6:
				LED6On();
				break;
			case 7:
				LED1Off();
				break;
			case 8:
				LED2Off();
				break;
			case 9:
				LED3Off();
				break;
			case 10:
				LED4Off();
				break;
			case 11:
				LED5Off();
				break;
			case 12:
				LED6Off();
				break;
		}
	}
}

void uartreceiveTestInterrupt(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	cli();
	
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1)|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,

	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	while(1);
}

void uartSendTest(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	/*
	cli();
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<TXEN1);// Enable Transmit

	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	*/
	
	SETBIT(PORTA, DDA3); //set to send data.
	unsigned char data = 0x00;
	while(1){
		data = (data % 4) + 1;
		
		rs485_transmit(data);
		/*
		while ( !( UCSR1A & (1<<UDRE1)) );
		
		UDR1 = data;
		*/
		_delay_ms(1);
	}
}

void testLED(void){
	LED1On();
	LED2On();
	LED3On();
	LED4On();
	LED5On();
	LED6On();
	
	while(1){
		_delay_ms(500);
		LED1Toggle();
		LED2Toggle();
		LED3Toggle();
		LED4Toggle();
		LED5Toggle();
		LED6Toggle();
	}
}

void testDataValidation1(void){
	uint8_t dataLength = 7;
	uint8_t eLength = calcExstraSpace(dataLength);
	uint8_t fullLength = dataLength + eLength;
	uint8_t testArr[9] = {0x00};
	
	testArr[0] = 0x03;
	testArr[1] = 0x31;
	testArr[2] = 0x42;
	testArr[3] = 0xF2;
	testArr[4] = 0x9A;
	testArr[5] = 0x17;
	testArr[6] = 0xCC;
//	testArr[7] = 0x10;
	
	addCheckValue(testArr, fullLength, eLength);
	
	if(testArr[7] == 0b11000011){
		LED6On();
	}
	if(testArr[8] == 0b01010000){
		LED5On();
	}
	if(eLength == 2){
		LED4On();
	}
	
	uint8_t check1 = validateCheckValue(testArr, fullLength, eLength);
	
	testArr[8] = 0b01010001;
	
	uint8_t check2 = validateCheckValue(testArr, fullLength, eLength);
	
	if(check1){
		LED3On();
	}
	
	if(!check2){
		LED2On();
	}
	
	while(1){
		
	}
}

void testDataValidation2(void){
//	uint8_t dataLength = 7;
//	uint8_t eLength = calcExstraSpace(dataLength);
//	uint8_t fullLength = dataLength + eLength;
	uint8_t testArr1[2] = {0x00};
	uint8_t testArr2[5] = {0x00};
	uint8_t testArr3[2] = {0x00};
	
	testArr1[0] = 0x03;
	testArr1[1] = 0x31;
	testArr2[0] = 0x42;
	testArr2[1] = 0xF2;
	testArr2[2] = 0x9A;
	testArr2[3] = 0x17;
	testArr2[4] = 0xCC;
	
	addCheckValue2(testArr1, 2, testArr2, 5, testArr3, 0);
	
	if(testArr3[0] == 0b11000011){
		LED6On();
	}
	if(testArr3[1] == 0b01010000){
		LED5On();
	}
	
	/*
	if(eLength == 2){
		LED4On();
	}
	
	uint8_t check1 = validateCheckValue(testArr, fullLength, eLength);
	
	testArr[8] = 0b01010001;
	
	uint8_t check2 = validateCheckValue(testArr, fullLength, eLength);
	
	if(check1){
		LED3On();
	}
	
	if(!check2){
		LED2On();
	}
	*/
	while(1){
		
	}
}

ISR(USART1_RX_vect)
{
	
	unsigned char data = UDR1;
	if(data){
	}
}



#endif /* TEST_H_ */