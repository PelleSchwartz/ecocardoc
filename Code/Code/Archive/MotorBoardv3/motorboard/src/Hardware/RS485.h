/*
 * RS485.h
 *
 * Created: 01/05/2016 23:20:17
 *  Author: Kim
 */ 


#ifndef RS485_H_
#define RS485_H_

void rs485_Init(void);

void rs485_TransmitMode(void);
void rs485_ReceiveMode(void);

void rs485_Transmit(unsigned char data);
void rs485_Transmit_S(const char *s );
void rs485_WaitTransmit(void);

unsigned char rs485_receive(void);

void rs485_EnableInterrupt(void);
void rs485_DisableInterrupt(void);

/*
Needs to be implemented somewhere else.
Will be called when data is received.
*/
void rs485_interruptReceive(unsigned char data);

#endif /* RS485_H_ */