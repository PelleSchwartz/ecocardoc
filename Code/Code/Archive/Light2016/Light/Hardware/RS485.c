/*
 * RS485.c
 *
 * Created: 01/05/2016 17:32:15
 *  Author: Kim
 */ 

#include "global.h"
#include "leds.h"
#include "RS485.h"

void rs485_TransmitMode(void){
	LEDnOn(2);
}
void rs485_ReceiveMode(void){
	LEDnOff(2);
}

void rs485_EnableInterrupt(void){
	UCSR0B |= (1<<RXCIE0);
}

void rs485_DisableInterrupt(void){
	UCSR0B &= ~(1<<RXCIE0);
}

void rs485_Init(void) {		
	LEDnWriteMode(2);
	rs485_ReceiveMode();
	
	// Make sure I/O clock to USART0 is enabled
	PRR &= ~(1 << PRUSART0);
	
	uint16_t baudrate = UART_BAUD_SELECT(BAUD485, F_CPU);
	UBRR0H = (unsigned char)(baudrate>>8);
	UBRR0L = (unsigned char)baudrate;
	
	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR0A = (1 << TXC0) | (0 << U2X0);

	// Enable receiver, transmitter and receive interrupt
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Asynchronous mode, no parity, 1 stop bit, character size = 8-bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << UCPOL0);
}

unsigned char rs485_anyReceived(void){
	char answer = (UCSR0A & (1<<RXC0));
	return answer;
}

void rs485_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Clear TXC flag */
	UCSR0A |= (1<<TXC0);
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void rs485_WaitForTransmit(void){
	uint8_t n = 0;
	while(n <= RS485_TX_WAIT_TIMEOUT_CK) {
		if(TESTBIT(UCSR0A, TXC0)) {
			SETBIT(UCSR0A, TXC0);
			break;
		}
		n++;
	}
}