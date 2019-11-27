/*
* rs485.h
*
* Created: 21/06/2016 17:33:12
*  Author: Henning
*/

#ifndef RS485_H_
#define RS485_H_

#include "rs485protocol.h"

#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)

// Main while loop waits for RS485 receive timeout [ms] (no minimum value)
#define RS485_RX_WAIT_TIMEOUT 50
// Main while loop waits for RS232 transmit complete or timeout [clock cycles] (warning, a value too low might corrupt transmit data)
// This is a worst case timeout and if this happens, things have probably gone to shit anyway.
#define RS485_TX_WAIT_TIMEOUT_CK (F_CPU / 8)

void rs485_init(void);
void rs485_set_tx_mode(void);
void rs485_set_rx_mode(void);
void rs485_putc(uint8_t data);
void rs485_puts(char * s);
void rs485_wait_transmit(void);
uint16_t rs485_available(void);
void rs485_flush_buffer(void);
unsigned char rs485_read_data(void);
void rs485_read_buffer(void * buff, uint16_t len);

#endif /* RS485_H_ */