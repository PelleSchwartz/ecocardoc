/*
 * RS485.c
 *
 * Created: 01/05/2016 17:32:15
 *  Author: Kim
 */ 

#include "..\asf.h"

#include "RS485.h"
#include "LEDs.h"

void rs485_TransmitMode(void){
	SETBIT(PORTA, PA3);
}
void rs485_ReceiveMode(void){
	char useless;
	CLRBIT(PORTA, PA3);
	useless = UDR1;
	if(useless){
		LED5On();
	}
	
}

void rs485_EnableInterrupt(void){
	UCSR1B |= (1<<RXCIE1);
}

void rs485_DisableInterrupt(void){
	UCSR1B &= ~(1<<RXCIE1);
}

void rs485_Init(void){
	PRR1 &= ~(1 << PRUSART1);
	
	/*Set baud rate */
	UBRR1H = (unsigned char)(RS485BAUD>>8);
	UBRR1L = (unsigned char)RS485BAUD;
	
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (3<<UCSZ10);
	
	/* Datadir port set as output, and set to receive mode */
	DDRA |= (1 << DDA3);
	rs485_ReceiveMode();
}

void rs485_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Clear TXC flag */
	UCSR1A |= (1<<TXC1);
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void rs485_Transmit_S(const char *s ){
	while (*s) {
		rs485_Transmit(*s++);
	}
}

void rs485_WaitTransmit(void){ //Never ever use if not send anything.
	while(!TESTBIT(UCSR1A, TXC1));
}

unsigned char rs485_receive(void) {
	while ( !(UCSR1A & (1<<RXC1)) ); // Wait for data to be received
	unsigned char temp = UDR1; //not realy needed.
	return temp;	// Return received data from buffer
}

ISR(USART1_RX_vect){
	rs485_interruptReceive(UDR1);
}