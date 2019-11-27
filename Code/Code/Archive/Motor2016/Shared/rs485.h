/*
* rs485.h
*
* Created: 21/06/2016 17:33:12
*  Author: Henning
*/

#ifndef RS485_H_
#define RS485_H_

#include "rs485protocol.h"

// Main while loop waits for RS485 receive timeout [ms] (no minimum value)
#define RS485_RX_WAIT_TIMEOUT 50
// Main while loop waits for RS485 transmit complete or timeout [clock cycles] (warning, a value too low might corrupt transmit data)
// This is a worst case timeout and if this happens, things have probably gone to shit anyway.
#define RS485_TX_WAIT_TIMEOUT_CK (F_CPU / 8)
// The RS485 RX vector
#define RS485_RX_vect USART1_RX_vect

void rs485_init(void);
void rs485_set_tx_mode(void);
void rs485_set_rx_mode(void);
void rs485_putc(unsigned char data);
void rs485_wait_transmit(void);

#endif /* RS485_H_ */