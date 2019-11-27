/*
 * rs232sync.h
 *
 * Created: 20/06/2016 01:56:17
 *  Author: Henning
 */ 

#ifndef RS232SYNC_H_
#define RS232SYNC_H_

#include <stdbool.h>
#include "uart.h"

// Buffer size based on UART buffer size
//#define RS232_RX_BUFFER_SIZE UART_RX2_BUFFER_SIZE

void rs232_set_car(uint8_t carId);
void rs232_tx(void);

enum {
	RS232_STATE_WAITING,
	RS232_STATE_WAITING_FOR_SECOND_START,
	RS232_STATE_GET_PACKET_SIZE,
	RS232_STATE_RECEIVING
};

#endif /* RS232SYNC_H_ */