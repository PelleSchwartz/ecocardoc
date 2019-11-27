/*
 * BlueTooth.c
 *
 * Created: 29/05/2016 13:28:25
 *  Author: Henning
 */ 

#include "global.h"
#include "bluetooth.h"
#include "uart.h"
#include "delay.h"

void blue_init(void) {
	uart0_init(UART_BAUD_SELECT(BAUDBLUE, F_CPU));
}

void blue_putc(unsigned char data) {
	uart0_putc(data);
}

void blue_puts(char * s) {
	uart0_puts(s);
}

uint16_t blue_available() {
	return uart0_available();
}

void blue_flush_buffer() {
	uart0_flush();
}

unsigned char blue_read_data() {
	unsigned int c = uart0_getc();
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

void blue_read_buffer(void * buff, uint16_t len) {
	uint16_t i;
	for(i=0; i<len; i++) {
		((char*)buff)[i] = blue_read_data();
	}
}

// To set the Bluetooth name, set Bluefruit in CMD mode and restart, then issue following command
//void blue_set_new_device() {
	//delay_ms(10000);
	//// Name
	//blue_puts("AT+GAPDEVNAME=DTU Innovator\n\r");
	//delay_ms(1000);
	//// Max power
	//blue_puts("AT+BLEPOWERLEVEL=4");
	//delay_ms(1000);
	//// Disable CTS
	//blue_puts("AT+UARTFLOW=0");
	//delay_ms(1000);
	//blue_puts("ATZ\n\r");
//}