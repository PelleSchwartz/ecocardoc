///*
 //* RS485.c
 //*
 //* Created: 01/05/2016 17:32:15
 //*  Author: Kim
 //*/ 
//
//#include "global.h"
//#include "RS485.h"
//#include <avr/interrupt.h>
//
//void rs485_TransmitMode(void){
	//PORTB |= (1<<PORTB4);
//}
//void rs485_ReceiveMode(void){
	//PORTB &= ~(1<<PORTB4);	
//}
//
//void rs485_EnableInterrupt(void){
	//UCSR1B |= (1<<RXCIE1);
//}
//
//void rs485_DisableInterrupt(void){
	//UCSR1B &= ~(1<<RXCIE1);
//}
//
//void rs485_init(void){
	//PRR0 &= ~(1 << PRUSART1);
	//
	//uint16_t baudrate = UART_BAUD_SELECT(BAUD485, F_CPU);
	//UBRR0H = (unsigned char)(baudrate>>8);
	//UBRR0L = (unsigned char)baudrate;
	//
	///* Enable receiver and transmitter */
	//UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	//
	///* Set frame format: 8data, 1stop bit */
	//UCSR1C = (3 << UCSZ10);
	//
	//SETBIT(DDRB,DDB4); //WriteMode
	//rs485_ReceiveMode();
//}
//
//unsigned char rs485_anyReceived(void){
	//char answer = (UCSR1A & (1<<RXC1));
	//return answer;
//}
//
//unsigned char rs485_receive(void) {
	//while ( !(UCSR1A & (1<<RXC1)) ); // Wait for data to be received
	//unsigned char temp = UDR1; //not really needed.
	//return temp;	// Return received data from buffer
//}
//
//void rs485_Transmit(unsigned char data) {
	///* Wait for empty transmit buffer */
	//while ( !( UCSR1A & (1<<UDRE1)) );
	///* Clear TXC flag */
	//UCSR1A |= (1<<TXC1);
	///* Put data into buffer, sends the data */
	//UDR1 = data;
//}
//
//void rs485_WaitForTransmit(void){
	//while(!(UCSR1A & (1 << TXC1)));
//}
//
//ISR(USART1_RX_vect){
	//rs485_autoReceive(UDR1);
//}

























/*
* rs485.c
*
* Created: 21/06/2016 17:32:15
*  Author: Henning
*/

#include "global.h"
#include "rs485.h"
#include "uart.h"

void rs485_init(void) {
	uart1_init(UART_BAUD_SELECT(BAUD485, F_CPU));
	// Datadir as output
	SETBIT(DDRB, DDB4);
	rs485_set_rx_mode();
}

void rs485_set_tx_mode(void) {
	SETBIT(PORTB, PORTB4);	// Datadir on PB4
}
void rs485_set_rx_mode(void) {
	CLRBIT(PORTB, PORTB4);	// Datadir on PB4
}

void rs485_putc(unsigned char data) {
	uart1_putc(data);
}

void rs485_puts(char * s){
	uart1_puts(s);
}

void rs485_wait_transmit(void) {	
	uint16_t n = 0;
	while(!TESTBIT(UCSR1A, TXC1) && n <= RS485_TX_WAIT_TIMEOUT) {
		n++;
	}
}

uint16_t rs485_available(void) {
	return uart1_available();
}

void rs485_flush_buffer(void) {
	uart1_flush();
}

unsigned char rs485_read_data(void) {
	unsigned int c = uart1_getc();
	if(c & UART_NO_DATA) {
		/*
		* No data available from UART
		*/
		return 0;
	}
	else {
		/*
		* New data available from UART
		* Check for Frame or Overrun error
		*/
		if(c & UART_FRAME_ERROR) {
			/* Framing Error detected, i.e no stop bit detected */
		}
		if(c & UART_OVERRUN_ERROR) {
			/*
			* Overrun, a character already present in the UART UDR register was
			* not read by the interrupt handler before the next character arrived,
			* one or more received characters have been dropped
			*/
		}
		if(c & UART_BUFFER_OVERFLOW) {
			/*
			* We are not reading the receive buffer fast enough,
			* one or more received character have been dropped
			*/
		}
		return (unsigned char)c;
	}
}

void rs485_read_buffer(void * buff, uint16_t len) {
	uint16_t i;
	for(i=0; i<len; i++) {
		((char*)buff)[i] = rs485_read_data();
	}
}