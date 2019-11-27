///*
 //* Connection.h
 //*
 //* Created: 2016-05-14 20:15:53
 //*  Author: kry
 //*/ 
//
//
//#ifndef RS485SYNC_H_
//#define RS485SYNC_H_
//
////these needs to be initiated outside connection.
//void connection_initiate(uint8_t inDataSize, uint8_t outDataSize, uint8_t unitID);
//uint8_t connection_GetInData(int byteNr);
//uint8_t connection_GetOutData(int bytenr);
//void connection_SetOutData(int byteNr, uint8_t value);
//void connection_SetOutDataBit(int byteNr, int bitNr, uint8_t highLow);
//
//uint8_t connection_AnyChanges(void);
//uint8_t connection_OldOutData(void);
//#endif /* RS485SYNC_H_ */









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

#endif /* RS485SYNC_H_ */