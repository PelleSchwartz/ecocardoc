/*
 * BlueTooth.c
 *
 * Created: 29/05/2016 13:28:25
 *  Author: Henning
 */ 

#include "..\asf.h"

#include "BlueTooth.h"

void blue_EnableInterrupt(void){
	UCSR0B |= (1<<RXCIE0);
}

void blue_DisableInterrupt(void){
	UCSR0B &= ~(1<<RXCIE0);
}

void blue_Init(void){
	PRR0 &= ~(1 << PRUSART0);
	
	/*Set baud rate */
	UBRR0H = (unsigned char)(BLUETOOTH_USED_BAUD>>8);
	UBRR0L = (unsigned char)BLUETOOTH_USED_BAUD;
	
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (3<<UCSZ00);
}

void blue_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Clear TXC flag */
	UCSR0A |= (1<<TXC0);
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void blue_Transmit_S(const char *s ){
	while (*s) {
		blue_Transmit(*s++);
	}
}

void blue_WaitTransmit(void){ //Never ever use if not send anything.
	while(!TESTBIT(UCSR0A, TXC0));
}

unsigned char blue_receive(void) {
	while ( !(UCSR0A & (1<<RXC0)) ); // Wait for data to be received
	unsigned char temp = UDR0; //not realy needed.
	return temp;	// Return received data from buffer
}