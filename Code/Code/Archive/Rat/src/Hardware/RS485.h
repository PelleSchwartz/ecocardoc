/*
 * RS485.h
 *
 * Created: 01/05/2016 23:20:17
 *  Author: Kim
 */ 

#ifndef RS485_H_
#define RS485_H_

#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)

void rs485_Init(void);

void rs485_TransmitMode(void);
void rs485_ReceiveMode(void);

void rs485_Transmit(unsigned char data);
void rs485_WaitForTransmit(void);

unsigned char rs485_receive(void);
unsigned char rs485_anyReceived(void);

void rs485_EnableInterrupt(void);
void rs485_DisableInterrupt(void);

/*
Needs to be implemented somewhere else.
Will be called when data is received.
*/
void rs485_autoReceive(unsigned char data);

#endif /* RS485_H_ */