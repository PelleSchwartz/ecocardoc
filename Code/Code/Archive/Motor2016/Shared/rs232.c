/*
 * rs232.c
 *
 * Created: 11/06/2016 17:48:11
 *  Author: Henning
 */ 
#include "global.h"
#include "rs232.h"
#include "rs232sync.h"
#include "uart.h"

// Note: ISR(RS232_RX_vect) needs to be define somewhere else

static inline void uart2_init(uint16_t baudrate) {
	// Make sure I/O clock to USART1 is enabled
	PRR1 &= ~(1 << PRUSART2);
	
	UBRR2H = (uint8_t)(baudrate>>8);
	UBRR2L = (uint8_t) baudrate;
	
	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR2A = (1 << TXC2) | (0 << U2X2);

	// Enable receiver, transmitter and receive interrupt
	UCSR2B = (1 << RXEN2) | (1 << TXEN2) | (1 << RXCIE2);

	// Asynchronous mode, no parity, 1 stop bit, character size = 8-bit
	UCSR2C = (1 << UCSZ21) | (1 << UCSZ20) | (0 << UCPOL2);
}

static inline uint16_t uart2_available() {
	char answer = (UCSR2A & (1<<RXC2));
	return answer;
}

static inline void uart2_putc(uint8_t data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR2A & (1<<UDRE2)) );
	/* Clear TXC flag */
	UCSR2A |= (1<<TXC2);
	/* Put data into buffer, sends the data */
	UDR2 = data;
}

void rs232_init(void) {
	uart2_init(UART_BAUD_SELECT(BAUD232, F_CPU));
}

void rs232_putc(unsigned char data) {
	uart2_putc(data);
}

void rs232_wait_transmit(void) {
	uint8_t n = 0;
	while(n <= RS232_TX_WAIT_TIMEOUT_CK) {
		if(TESTBIT(UCSR2A, TXC2)) {
			SETBIT(UCSR2A, TXC2);
			break;
		}
		n++;
	}
}

uint16_t rs232_available(void) {
	return uart2_available();
}