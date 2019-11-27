/*
* rs485.c
*
* Created: 21/06/2016 17:32:15
*  Author: Henning
*/

#include "global.h"
#include "rs485.h"
#include "rs485sync.h"
#include <avr/interrupt.h>

ISR(USART1_RX_vect) {
	rs485_autoReceive(UDR1);
}

void uart1_init(uint16_t baudrate) {	
	// Make sure I/O clock to USART1 is enabled
	PRR0 &= ~(1 << PRUSART1);
	
	UBRR1H = (uint8_t)(baudrate>>8);
	UBRR1L = (uint8_t) baudrate;
	
	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR1A = (1 << TXC1) | (0 << U2X1);

	// Enable receiver, transmitter and receive interrupt
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);

	// Asynchronous mode, no parity, 1 stop bit, character size = 8-bit
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10) | (0 << UCPOL1);
}

uint16_t uart1_available() {
	char answer = (UCSR1A & (1<<RXC1));
	return answer;
}

void uart1_putc(uint8_t data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Clear TXC flag */
	UCSR1A |= (1<<TXC1);
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void uart1_flush() {
	// DOES NOT DO ANYTHING
}

void rs485_init(void) {
	uart1_init(UART_BAUD_SELECT(BAUD485, F_CPU));
	// Datadir as output
	SETBIT(DDRB, DDB4);
	rs485_set_rx_mode();
}

void rs485_set_tx_mode(void) {
	delay_ms(5);
	SETBIT(PORTB, PORTB4);	// Datadir on PB4
	delay_ms(5);
}
void rs485_set_rx_mode(void) {
	CLRBIT(PORTB, PORTB4);	// Datadir on PB4
}

void rs485_putc(uint8_t data) {
	uart1_putc(data);
}

void rs485_wait_transmit(void) {
	uint8_t n = 0;
	while(n <= RS485_TX_WAIT_TIMEOUT_CK) {
		if(TESTBIT(UCSR1A, TXC1)) {
			SETBIT(UCSR1A, TXC1);
			break;
		}
		n++;
	}
}

uint16_t rs485_available(void) {
	return uart1_available();
}

void rs485_flush_buffer(void) {
	uart1_flush();
}
