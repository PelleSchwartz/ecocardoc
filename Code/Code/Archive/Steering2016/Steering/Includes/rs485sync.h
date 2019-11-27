/*
 * rs485sync.h
 *
 * Created: 20/06/2016 01:56:17
 *  Author: Henning
 */ 

#ifndef RS485SYNC_H_
#define RS485SYNC_H_

#include "uart.h"

// Buffer size based on UART buffer size
#define RS485_RX_BUFFER_SIZE UART_RX1_BUFFER_SIZE

void rs485_sync(void);
void rs485_autoReceive(unsigned char data);

enum {
	STATE_WAITING,
	STATE_WAITING_FOR_SECOND_START,
	STATE_CHECKING_ADDRESS,
	STATE_RECEIVING
};

#endif /* RS485SYNC_H_ */