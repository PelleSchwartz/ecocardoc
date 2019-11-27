/*
 * BlueTooth.h
 *
 * Created: 29/05/2016 13:40:54
 *  Author: Henning
 */ 


#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

void blue_EnableInterrupt(void);

void blue_DisableInterrupt(void);

void blue_Init(void);

void blue_Transmit(unsigned char data);
void blue_Transmit_S(const char *s );

void blue_WaitTransmit(void);

unsigned char blue_receive(void);

#endif /* BLUETOOTH_H_ */