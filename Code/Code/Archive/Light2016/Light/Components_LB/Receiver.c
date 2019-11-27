/*
 * Receiver.c
 *
 * Created: 05/05/2016 13:49:52
 *  Author: Kim
 */ 

#include "../Components_Shared/global.h"
#include "rs485protocol.h"
#include "../Components_Shared/Connection.h"
#include "Receiver.h"

void receiver_initiate(void){
	connection_initiate(BACK_LIGHT_TX_REGSIZE, BACK_LIGHT_RX_REGSIZE, RS485_ID_BACK_LIGHT);
}

uint8_t receiver_AnyChanges(void){
	return connection_AnyChanges();
}

uint8_t received_Starter(void){
	return GETBIT(connection_GetInData(0), BL_B0_STARTER_LIGHT);
}

uint8_t received_Brake(void){
	return GETBIT(connection_GetInData(0), BL_B0_BRAKE_LIGHT);
}

uint8_t received_Normal(void){
	return GETBIT(connection_GetInData(0), BL_B0_NORMAL_LIGHT);
}

uint8_t received_LeftBlink(void){
	return GETBIT(connection_GetInData(0), BL_B0_TURN_LEFT);
}

uint8_t received_RightBlink(void){
	return GETBIT(connection_GetInData(0), BL_B0_TURN_RIGHT);
}