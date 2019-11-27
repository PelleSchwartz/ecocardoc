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

enum {
	RS485_STATE_WAITING,
	RS485_STATE_WAITING_FOR_SECOND_START,
	RS485_STATE_RECEIVING,
	RS485_STATE_RECEIVE_COMPLETE
};

#endif /* RS485SYNC_H_ */