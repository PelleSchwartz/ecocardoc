/*
 * RS485.c
 *
 * Created: 01/05/2016 17:32:15
 *  Author: Kim
 */ 

#include "..\global.h"
#include "RS485.h"
#include <avr/interrupt.h>

void rs485_TransmitMode(void){
	PORTB |= (1<<PORTB4);
}
void rs485_ReceiveMode(void){
	PORTB &= ~(1<<PORTB4);	
}

void rs485_EnableInterrupt(void){
	UCSR1B |= (1<<RXCIE1);
}

void rs485_DisableInterrupt(void){
	UCSR1B &= ~(1<<RXCIE1);
}

void rs485_Init(void){
	PRR0 &= ~(1 << PRUSART1);
	
	uint16_t baudrate = UART_BAUD_SELECT(BAUD485, F_CPU);
	UBRR0H = (unsigned char)(baudrate>>8);
	UBRR0L = (unsigned char)baudrate;
	
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (3 << UCSZ10);
	
	SETBIT(DDRB,DDB4); //WriteMode
	rs485_ReceiveMode();
}

unsigned char rs485_anyReceived(void){
	char answer = (UCSR1A & (1<<RXC1));
	return answer;
}

unsigned char rs485_receive(void) {
	while ( !(UCSR1A & (1<<RXC1)) ); // Wait for data to be received
	unsigned char temp = UDR1; //not really needed.
	return temp;	// Return received data from buffer
}

void rs485_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Clear TXC flag */
	UCSR1A |= (1<<TXC1);
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void rs485_WaitForTransmit(void){
	while(!(UCSR1A & (1 << TXC1)));
}

ISR(USART1_RX_vect){
	rs485_autoReceive(UDR1);
}