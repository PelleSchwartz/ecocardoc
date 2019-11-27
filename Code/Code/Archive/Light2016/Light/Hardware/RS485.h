/*
 * RS485.h
 *
 * Created: 01/05/2016 23:20:17
 *  Author: Kim
 */ 

#ifndef RS485_H_
#define RS485_H_

#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)

// Main while loop waits for RS485 receive timeout [ms] (no minimum value)
#define RS485_RX_WAIT_TIMEOUT 50
// Main while loop waits for RS232 transmit complete or timeout [clock cycles] (warning, a value too low might corrupt transmit data)
// This is a worst case timeout and if this happens, things have probably gone to shit anyway.
#define RS485_TX_WAIT_TIMEOUT_CK (F_CPU / 8)

void rs485_Init(void);

void rs485_TransmitMode(void);
void rs485_ReceiveMode(void);

unsigned char rs485_anyReceived(void);
unsigned char rs485_receive(void);
void rs485_Transmit(unsigned char data);
void rs485_WaitForTransmit(void);

void rs485_EnableInterrupt(void);
void rs485_DisableInterrupt(void);

#endif /* RS485_H_ */