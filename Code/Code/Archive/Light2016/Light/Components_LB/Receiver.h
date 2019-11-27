/*
 * Receiver.h
 *
 * Created: 05/05/2016 18:26:43
 *  Author: Kim
 */ 


#ifndef RECEIVER_H_
#define RECEIVER_H_

void receiver_initiate(void);

uint8_t receiver_AnyChanges(void);

uint8_t received_Starter(void);
uint8_t received_Brake(void);
uint8_t received_Normal(void);
uint8_t received_LeftBlink(void);
uint8_t received_RightBlink(void);



#endif /* RECEIVER_H_ */