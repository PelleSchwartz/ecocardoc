/**
 * \file
 *
 * \brief megaAVR STK600 UART interrupt example
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/**
 * \mainpage
 * \section board STK600 development board
 * \section intro Introduction
 * This example demonstrates how to use the megaAVR UART with interrupts.
 *
 * \section files Files:
 * - mega_uart_interrupt_example.c: megaAVR STK600 UART interrupt example
 *
 * \section exampledescription Brief description of the example application
 * This application will initialize the UART, send a string and then receive
 * it and check if it is the same string as we sent.
 *
 * \note The RX and TX pins should be externally connected in order to pass the
 * test.
 *
 * \section compinfo Compilation Info
 * This software was written for the <A href="http://gcc.gnu.org/">GNU GCC</A>
 * for AVR. \n
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

 // _ASSERT_ENABLE_ is used for enabling assert, typical for debug purposes
#define F_CPU 		16000000UL
#define BAUD485 	250000
#define RS485BAUD 	(F_CPU/16/BAUD485-1)
#define TESTBIT(var, bit)	(var  & (1<<bit))
#define SETBIT(var, bit)	(var |= (1<<bit))
#define CLRBIT(var, bit)	(var &= ~(1<<bit))
#define FLIPBIT(var, bit)	(var ^= (1<<bit))

#include <util/delay.h>

#define _ASSERT_ENABLE_
#include <string.h>
#include "compiler.h"

/**
 * \def BUFFER_SIZE
 * \brief The size of the UART buffer
 */
#define BUFFER_SIZE 20

// set the correct BAUD and F_CPU defines before including setbaud.h
#include "conf_clock.h"
#include "conf_uart.h"

/**
 * \name avr_libc_inc avr libc include files
 * @{
 */
#include <util/setbaud.h>
#include <avr/interrupt.h>
//! @}

#include "ring_buffer.h"

// buffers for use with the ring buffer (belong to the UART)
uint8_t out_buffer[BUFFER_SIZE];
uint8_t in_buffer[BUFFER_SIZE];

// the string we send and receive on UART
const char test_string[] = "Hello, world!";

//! ring buffer to use for the UART transmission
struct ring_buffer ring_buffer_out;
//! ring buffer to use for the UART reception
struct ring_buffer ring_buffer_in;

enum {
	BACK_LIGHT_STATE,
	BACK_LIGHT_PWM,
	STOP_LIGHT_STATE,
	STOP_LIGHT_PWM,
	STARTER_LIGHT_STATE,
	STARTER_LIGHT_PWM,
	TURNSIG_RIGHT_STATE,
	TURNSIG_RIGHT_PWM,
	TURNSIG_LEFT_STATE,
	TURNSIG_LEFT_PWM,

	// Leave this last entry untouched
	REG_SIZE
};

uint8_t reg[REG_SIZE] = {0x00};

/**
 * \brief Initialize the UART with correct baud rate settings
 *
 * This function will initialize the UART baud rate registers with the correct
 * values using the AVR libc setbaud utility. In addition set the UART to
 * 8-bit, 1 stop and no parity.
 */
/*
static void uart_init(void)
{
#if defined UBRR0H
	// get the values from the setbaud tool
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#else
#error "Device is not supported by the driver"
#endif

#if USE_2X
	UCSR0A |= (1 << U2X0);
#endif

	// enable RX and TX and set interrupts on rx complete
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8-bit, 1 stop bit, no parity, asynchronous UART
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0) |
			(0 << UPM01) | (0 << UPM00) | (0 << UMSEL01) |
			(0 << UMSEL00);

	// initialize the in and out buffer for the UART
	ring_buffer_out = ring_buffer_init(out_buffer, BUFFER_SIZE);
	ring_buffer_in = ring_buffer_init(in_buffer, BUFFER_SIZE);
}
*/
void uartSendTest(void);
void uartreceiveTest(void);
void uartreceiveTest2(void);
void uartreceiveTestInter(void);
void simulateBackLight(void);

/**
 * \brief The main application
 *
 * This application will initialize the UART, send a character and then receive
 * it and check if it is the same character as was sent.
 *
 * \note The RX and TX pins should be externally connected in order to pass the
 * test.
 */
int main(void)
{
	DDRC |= (1<< DDC7);
	PORTC |= (1<< PORTC7);
	DDRD |= (1<< DDD7);
	PORTD |= (1<< PORTD7);
	_delay_ms(1000);
	CLRBIT(PORTD, PORTD7);
	CLRBIT(PORTC, PORTC7);
	_delay_ms(1000);
	
	//simulateBackLight();
	uartSendTest();
	//uartreceiveTest();
	//uartreceiveTest2();
	//uartreceiveTestInter();
	while(1){
		
	}
}

void rs485_transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Clear TXC flag */
	UCSR0A |= (1<<TXC0);
	/*
	PORTD |= (1<< PORTD7);
	if(data){
		PORTC |= (1<< PORTC7);
	}
	*/
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char rs485_receive(void) {
	while ( !(UCSR0A & (1<<RXC0)) ); // Wait for data to be received
	unsigned char temp = UDR0; //not realy needed.
	
	return temp;	// Return received data from buffer
}

void get_reg(unsigned char addr, unsigned char len){
	
	_delay_ms(1);
	SETBIT(PORTB, PORTB0);
	for(int i = 0; i < len; i++){
		rs485_transmit(reg[i+addr]);
	}
	
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Clear TXC flag */
	UCSR0A |= (1<<TXC0);
	
	CLRBIT(PORTB, PORTB0);
}

void set_reg(unsigned char addr, unsigned char len){
	
	for(int i = 0; i < len; i++){
		reg[i+addr] = rs485_receive();
	}
	
}

void simulateBackLight(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	PRR0 &= ~(1 << PRUSART0);
	
	SETBIT(DDRB, DDB0);
	
	
	UBRR0H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR0L = (unsigned char)RS485BAUD;

	UCSR0B = (1<<TXEN0)| (1<<RXEN0);// Enable Transmit

	UCSR0C = (3<<UCSZ00);
	
	while(1){
		CLRBIT(PORTB, PORTB0);
		
		PORTC |= (1<< PORTC7);
		unsigned char data = rs485_receive();
		PORTD |= (1<< PORTD7);
		if(data == 0xA5){
			
			data = rs485_receive();
			if((data >> 1) == 3){
				unsigned char op = 0x01 & data;
				unsigned char addr = rs485_receive();
				unsigned char len = rs485_receive();
				
				if(op){
					set_reg(addr, len);
				}
				else{
					get_reg(addr, len);
				}
			}
		}
	}
}

void uartSendTest(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	PRR0 &= ~(1 << PRUSART0);

	UBRR0H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR0L = (unsigned char)RS485BAUD;
	
	UCSR0B = (1<<TXEN0)| (1<<RXEN0);// Enable Transmit

	UCSR0C = (3<<UCSZ00);//| (1<<USBS0);
	
	SETBIT(DDRB, DDB0);
	SETBIT(PORTB, PORTB0);
	
	unsigned char data = 0x00;
	while(1){
		data = (data % 12) + 1;

		rs485_transmit(data);
		
		_delay_ms(1);
	}
}

void uartreceiveTest(void){ //same as above, just with interrupts
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	PRR0 &= ~(1 << PRUSART0);
	
	SETBIT(DDRB, DDB0);
	CLRBIT(PORTB, PORTB0);
	
	UBRR0H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR0L = (unsigned char)RS485BAUD;
	
	UCSR0B = (1<<TXEN0)| (1<<RXEN0);//|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,
	
	UCSR0C = (3<<UCSZ00);
	
	unsigned char data;
	while(1){
		while( !(UCSR0A & (1<<RXC0)) );
		PORTC = (1<< PORTC7);
		data = UDR0;
		if(data){
			PORTD = (1<< PORTD7);
		}
	}
}

void uartreceiveTest2(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	PRR0 &= ~(1 << PRUSART0);

	UBRR0H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR0L = (unsigned char)RS485BAUD;
	
	UCSR0B = (1<<TXEN0)| (1<<RXEN0);//|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,
	
	UCSR0C = (3<<UCSZ00);
	
	SETBIT(DDRB, DDB0);
	CLRBIT(PORTB, PORTB0);
	
	unsigned char data;
	while(1){
		while( !(UCSR0A & (1<<RXC0)) );
		data = UDR0;
		switch (data){
			case 1:
				PORTC |= (1<< PORTC7);
				break;
			case 2:
				PORTD |= (1<< PORTD7);
				break;
			case 3:
				PORTC &= ~(1<< PORTC7);
				break;
			case 4:
				PORTD &= ~(1<< PORTD7);
				break;
		}
	}
}

void uartreceiveTestInter(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	PRR0 &= ~(1 << PRUSART0);
		
	DDRB = 0x01;
	PORTB = 0x00;
		
	UBRR0H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR0L = (unsigned char)RS485BAUD;
		
	UCSR0B = (1<<RXEN0)|(1<<RXCIE1);// Enable Receiver/ and interrupt when receiving,

		
	UCSR0C = (3<<UCSZ00)| (1<<USBS0);
	while(1){
		
	}
}

/**
 * \brief UART data register empty interrupt handler
 *
 * This handler is called each time the UART data register is available for
 * sending data.
 */
ISR(USART0_UDRE_vect)
{
	// if there is data in the ring buffer, fetch it and send it
	if (!ring_buffer_is_empty(&ring_buffer_out)) {
		UDR0 = ring_buffer_get(&ring_buffer_out);
	}
	else {
		// no more data to send, turn off data ready interrupt
		UCSR0B &= ~(1 << UDRIE0);
	}
}

/**
 * \brief Data RX interrupt handler
 *
 * This is the handler for UART receive data
 */
ISR(USART0_RX_vect)
{
	
	SETBIT(PORTD, PORTD7);
	SETBIT(PORTC, PORTC7);
	/*
	LED1On();
	LED2On();
	LED3On();
	LED4On();
	LED5On();
	LED6On();
	*/
	/*LED6On();
	static int temp = 0;
	unsigned char temp2 = UDR1;
	if(temp2){
		temp++;
		_delay_ms(200);
	}
	switch(temp){
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
		default:
			temp = 0;
			LED1Off();
			LED2Off();
			LED3Off();
			LED4Off();
			LED5Off();
			break;
	}
	*/
}