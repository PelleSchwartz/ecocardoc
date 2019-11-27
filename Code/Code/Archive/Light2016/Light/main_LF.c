/*
 * LightFront.c
 *
 * Created: 27/04/2016 00:39:57
 * Author : Henning
 */

#include "Components_Shared/global.h"
#include <avr/io.h>
#include <util/delay.h>

#include "Components_LF/PortControl.h"
#include "Components_Shared/Connection.h"
#include "rs485protocol.h"
#include "timer.h"
#include "Test.h"

uint8_t con_NormalLight(void);
uint8_t con_StrongLight(void);
uint8_t con_Horn(void);
uint8_t con_Fan(void);
uint8_t con_Wiper(void);
uint8_t con_LeftSignal(void);
uint8_t con_RightSignal(void);
void con_Brake(uint8_t onOff);

int main(void) {		
	// INITIALIZE
	connection_initiate(FRONT_LIGHT_TX_REGSIZE, FRONT_LIGHT_RX_REGSIZE, RS485_ID_FRONT_LIGHT);
	portControl_Initiate();	
	//timer_init(); //front light doesn't use the timer, for now.
	
	DDRD &= ~(1 << DDD0);
	
	// START OF MAIN WHILE
	
	DDRC |= (1<<5);
	while(1) {
		// Update light
		//portControl_LeftSignal(con_LeftSignal());
		//portControl_RightSignal(con_RightSignal());
		//portControl_Wiper(con_Wiper());
		//portControl_Fan(con_Fan());
		//portControl_Horn(con_Horn());
		//portControl_NormalLight(con_NormalLight());
		//
		portControl_LeftSignal(portControl_ReadBrake());
		portControl_RightSignal(portControl_ReadBrake());
		portControl_Fan(portControl_ReadBrake());
		//set output
		con_Brake(portControl_ReadBrake());
	}
	// END OF MAIN WHILE
	return 0;
}

uint8_t con_NormalLight(void) {
	return GETBIT(connection_GetInData(0), FL_B0_NORMAL_LIGHT);
}

uint8_t con_StrongLight(void) {
	return GETBIT(connection_GetInData(0), FL_B0_HIGHBEAM_LIGHT);
}

uint8_t con_Horn(void) {
	return GETBIT(connection_GetInData(0), FL_B0_HORN);
}

uint8_t con_Fan(void) {
	return GETBIT(connection_GetInData(0), FL_B0_FAN);
}

uint8_t con_Wiper(void) {
	return GETBIT(connection_GetInData(0), FL_B0_WIPER);
}

uint8_t con_LeftSignal(void) {
	return GETBIT(connection_GetInData(0), FL_B0_TURNLEFT_LIGHT);
}

uint8_t con_RightSignal(void) {
	return GETBIT(connection_GetInData(0), FL_B0_TURNRIGHT_LIGHT);
}

void con_Brake(uint8_t onOff) {
	if(onOff == LIGHT_ON){
		//connection_SetOutDataBit(0, FL_RX_BRAKE, 1);
		connection_SetOutData(0, 1);
	}
	else if(onOff == LIGHT_OFF) {
		//connection_SetOutDataBit(0, FL_RX_BRAKE, 0);
		connection_SetOutData(0, 0);
	}
}