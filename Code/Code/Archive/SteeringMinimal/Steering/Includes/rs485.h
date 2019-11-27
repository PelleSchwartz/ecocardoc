///*
 //* RS485.h
 //*
 //* Created: 01/05/2016 23:20:17
 //*  Author: Kim
 //*/ 
//
//#ifndef RS485_H_
//#define RS485_H_
//
//#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)
//
//void rs485_init(void);
//
//void rs485_TransmitMode(void);
//void rs485_ReceiveMode(void);
//
//void rs485_Transmit(unsigned char data);
//void rs485_WaitForTransmit(void);
//
//unsigned char rs485_receive(void);
//unsigned char rs485_anyReceived(void);
//
//void rs485_EnableInterrupt(void);
//void rs485_DisableInterrupt(void);
//
///*
//Needs to be implemented somewhere else.
//Will be called when data is received.
//*/
//void rs485_autoReceive(unsigned char data);
//
//#endif /* RS485_H_ */
//






















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
#define RS485_RX_WAIT_TIMEOUT 5
// Main while loop waits for RS485 transmit timeout [ms] (warning, a value too low might corrupt transmit data)
#define RS485_TX_WAIT_TIMEOUT 100

void rs485_init(void);
void rs485_set_tx_mode(void);
void rs485_set_rx_mode(void);
void rs485_putc(unsigned char data);
void rs485_puts(char * s);
void rs485_wait_transmit(void);
uint16_t rs485_available(void);
void rs485_flush_buffer(void);
unsigned char rs485_read_data(void);
void rs485_read_buffer(void * buff, uint16_t len);

#endif /* RS485_H_ */