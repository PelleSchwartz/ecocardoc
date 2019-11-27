/*
* RS485.c
*
* Created: 01/05/2016 17:32:15
*  Author: Kim
*/

#include "lcd.h"
#include "global.h"
#include "buttons.h"
#include "rs485.h"
#include "rs485sync.h"
#include "buttons.h"
#include "DOGXL-160-7.h"
#include <avr/interrupt.h>

void _rs485_tx(void);

// Evil volatile variables
static volatile uint8_t _inTempData[STEERING_TX_REGSIZE + 10];
static volatile uint8_t state = STATE_WAITING;
static volatile uint8_t dataIndex = 0;

uint8_t connection_GetInData(int byteNr);
uint8_t connection_GetOutData(int bytenr);
void connection_SetOutData(int byteNr, uint8_t newValue);

void rs485_sync(void) {
	static unsigned char StateBlinkL = 1, StateBlinkR = 1, StateBlink = 1, StateFan = 1, StateWiper = 1, StateHBeam = 1, StateNLight = 1, StateGearUp = 1, StateGearDown = 1, StateNeutral = 1, StateGearAuto = 1, StateClosedLoop = 1, StateIdle = 1, StatePotUp = 1, StatePotDown = 1;
	static unsigned char oldGear = 0;
	
	// UPDATE TRANSMIT REGISTER
	// Sync applicable RIO data to RS485
	
	//steering_out[STEERING_BURN] = check_button(BTN_BURN);
	connection_SetOutData(STEERING_BURN, check_button(BTN_BURN));
	
	if(StatePotUp && check_button(BTN_POT_UP)){
		connection_SetOutData(STEERING_POT_UP, 1);
		StatePotUp = 0;
	}
	else{
		connection_SetOutData(STEERING_POT_UP, 0);
	}
	if(!check_button(BTN_POT_UP)){
		StatePotUp = 1;
	}
	
	if(StatePotDown && check_button(BTN_POT_DOWN)){
		connection_SetOutData(STEERING_POT_DOWN, 1);
		StatePotDown = 0;
	}
	else{
		connection_SetOutData(STEERING_POT_DOWN, 0);
	}
	if(!check_button(BTN_POT_DOWN)){
		StatePotDown = 1;
	}
	
	//Idle
	if(StateIdle){
		if(check_button(BTN_IDLE)){
			connection_SetOutData(STEERING_IDLE, !connection_GetOutData(STEERING_IDLE));
			StateIdle = 0;
		}
	}
	if(!check_button(BTN_IDLE)){
		StateIdle = 1;
	}
	
	//ClosedLoop
	if(StateClosedLoop){
		if(check_button(BTN_CLOSED_LOOP)){
			connection_SetOutData(STEERING_CLOSED_LOOP, !connection_GetOutData(STEERING_CLOSED_LOOP));
			StateClosedLoop = 0;
		}
	}
	if(!check_button(BTN_CLOSED_LOOP)){
		StateClosedLoop = 1;
	}
	
	//Auto gear
	if(StateGearAuto)
	{
		if(check_button(BTN_AUTOGEAR))
		{
			if(connection_GetOutData(STEERING_AUTOGEAR) == 0){
				connection_SetOutData(STEERING_NEUTRAL, 0x00);
				connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
				connection_SetOutData(STEERING_GEAR_UP, 0x00);
				connection_SetOutData(STEERING_AUTOGEAR, 0x01);
				SetGearPage(GEAR_AUTO);
			}
			else{
				connection_SetOutData(STEERING_AUTOGEAR, 0x00);
				if(!connection_GetInData(STEERING_GEAR)){
					connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
					connection_SetOutData(STEERING_GEAR_UP, 0x00);
					connection_SetOutData(STEERING_NEUTRAL, 0x01);
					SetGearPage(GEAR_N);
				}
				else{
					if(connection_GetInData(STEERING_GEAR) - 1){
						connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
						connection_SetOutData(STEERING_GEAR_UP, 0x01);
						}else{
						connection_SetOutData(STEERING_GEAR_DOWN, 0x01);
						connection_SetOutData(STEERING_GEAR_UP, 0x00);
					}
					
					SetGearPage(connection_GetInData(STEERING_GEAR));
				}
				
			}
			StateGearAuto = 0;
		}
	}
	if(!check_button(BTN_AUTOGEAR))
	{
		StateGearAuto = 1;
	}
	
	//NEUTRAL
	if(StateNeutral)
	{
		if(check_button(BTN_NEUTRAL))
		{
			if(connection_GetOutData(STEERING_AUTOGEAR) == 0){
				connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
				connection_SetOutData(STEERING_GEAR_UP, 0x00);
				connection_SetOutData(STEERING_NEUTRAL, 0x01);

				SetGearPage(GEAR_N);
				//Set_LED_Color(5,255,255,255);
			}
			StateNeutral = 0;
		}
	}
	if(!check_button(BTN_NEUTRAL))
	{
		StateNeutral = 1;
	}
	
	//Gear up
	if(StateGearUp && check_button(BTN_GEAR_UP) && !connection_GetOutData(STEERING_AUTOGEAR)) {
		connection_SetOutData(STEERING_GEAR_UP, 1);
		connection_SetOutData(STEERING_GEAR_DOWN, 0);
		connection_SetOutData(STEERING_NEUTRAL, 0);
		StateGearUp = 0;
		if(oldGear == 1){
			SetGearPage(GEAR_2);
		}
		else{
			SetGearPage(GEAR_1);
		}
	}
	if(!check_button(BTN_GEAR_UP)) {
		StateGearUp = 1;
	}
	
	//Gear Down
	if(StateGearDown && check_button(BTN_GEAR_DOWN) && !connection_GetOutData(STEERING_AUTOGEAR)) {
		connection_SetOutData(STEERING_GEAR_DOWN, 1);
		connection_SetOutData(STEERING_GEAR_UP, 0);
		connection_SetOutData(STEERING_NEUTRAL, 0);
		StateGearDown = 0;
		if(oldGear != 1){
			SetGearPage(GEAR_1);
		}
	}
	if(!check_button(BTN_GEAR_DOWN)) {
		StateGearDown = 1;
	}
	
	if(connection_GetInData(STEERING_GEAR) != oldGear){
		connection_SetOutData(STEERING_GEAR_DOWN, 0);
		connection_SetOutData(STEERING_GEAR_UP, 0);
		oldGear = connection_GetInData(STEERING_GEAR);
	}
	
	
	// Sync applicable MOTOR data to RS485
	connection_SetOutData(STEERING_HYPERSPEED, check_button(BTN_HYPERSPEED));
	connection_SetOutData(STEERING_PARTY_TIME, check_button(BTN_PARTY_TIME));
	
	if(StateNLight){
		if(check_button(BTN_LIGHT_NORMAL)){
			connection_SetOutData(STEERING_LIGHT_NORMAL, !connection_GetOutData(STEERING_LIGHT_NORMAL));
			StateNLight = 0;
		}
	}
	if(!check_button(BTN_LIGHT_NORMAL)){
		StateNLight = 1;
	}
	
	if(StateHBeam){
		if(check_button(BTN_LIGHT_HIGH_BEAM)){
			connection_SetOutData(STEERING_LIGHT_HIGH_BEAM, !connection_GetOutData(STEERING_LIGHT_HIGH_BEAM));
			StateHBeam = 0;
		}
	}
	if(!check_button(BTN_LIGHT_HIGH_BEAM)){
		StateHBeam = 1;
	}
	
	if(StateFan){
		if(check_button(BTN_FAN)){
			connection_SetOutData(STEERING_FAN, !connection_GetOutData(STEERING_FAN));
			StateFan = 0;
		}
	}
	if(!check_button(BTN_FAN)){
		StateFan = 1;
	}
	
	if(StateWiper){
		if(check_button(BTN_WIPER)){
			connection_SetOutData(STEERING_WIPER, !connection_GetOutData(STEERING_WIPER));
			StateWiper = 0;
		}
	}
	if(!check_button(BTN_WIPER)){
		StateWiper = 1;
	}

	//steering_out[STEERING_HORN] = check_button(BTN_HORN);
	connection_SetOutData(STEERING_HORN, check_button(BTN_HORN));
	
	//R_BLINK
	if(StateBlinkR){
		if(check_button(BTN_BLINK_RIGHT)){
			connection_SetOutData(STEERING_BLINK_R, !connection_GetOutData(STEERING_BLINK_R));
			connection_SetOutData(STEERING_BLINK_L, 0);
			StateBlinkR = 0;
		}
	}
	if(!check_button(BTN_BLINK_RIGHT)){
		StateBlinkR = 1;
	}
	
	//L_BLINK
	if(StateBlinkL){
		if(check_button(BTN_BLINK_LEFT)){
			connection_SetOutData(STEERING_BLINK_L, !connection_GetOutData(STEERING_BLINK_L));
			connection_SetOutData(STEERING_BLINK_R, 0);
			StateBlinkL = 0;
		}
	}
	if(!check_button(BTN_BLINK_LEFT)){
		StateBlinkL = 1;
	}
}

void _send_header(void) {
	rs485_putc(RS485_RESPONSE_START_1);
	rs485_putc(RS485_RESPONSE_START_2);
}

void _rs485_tx(void) {
	// Update buttons and transmit registers
	// These operations take enough cycles that a delay is not necessary.
	// If they are moved, a delay_us(30) is needed here.
	update_buttons();
	rs485_sync();
	// Set Datadir to TX
	rs485_set_tx_mode();
	// Transmit Data	
	_send_header();
	for(int i=0; i<STEERING_RX_REGSIZE; i++) {
		rs485_putc(steering_out[i]);
		//rs485_putc(1);
	}
	// Wait for transmit to complete
	rs485_wait_transmit();
	// Set Datadir to RX
	rs485_set_rx_mode();
}

void _updateData(void){
	for(uint8_t i = 0 ; i < STEERING_TX_REGSIZE ; i++){
		steering_in[i] = _inTempData[i];
	}
}

void rs485_autoReceive(unsigned char data) {		
	switch(state) {
		case STATE_WAITING:
		{
			if(data == RS485_START_1) {
				state = STATE_WAITING_FOR_SECOND_START;
			}
		}
		break;		
		case STATE_WAITING_FOR_SECOND_START:
		{
			if(data == RS485_START_2) {
				state = STATE_CHECKING_ADDRESS;
			}
			else {
				state = STATE_WAITING;
			}
		}
		break;		
		case STATE_CHECKING_ADDRESS: // Checking ID, and if it is write or read.
		{
			if((data >> 1) == RS485_ID_STEERING_WHEEL) {		
				if((data & 0x01) == OP_WRITE) {
					state = STATE_RECEIVING;
					dataIndex = 0;					
				}
				else if((data & 0x01) == OP_READ) {
					_rs485_tx();			
					state = STATE_WAITING;
				}
				else { // Should not be possible
					state = STATE_WAITING;
				}
			}
			else {
				state = STATE_WAITING;
			}
		}
		break;		
		case STATE_RECEIVING: // Receiving data.
		{
			if(dataIndex < STEERING_TX_REGSIZE) {
				_inTempData[dataIndex] = data;
				dataIndex++;
			}			
			if(dataIndex == STEERING_TX_REGSIZE){				
				_updateData();
				state = STATE_WAITING;
			}
		}
		break;		
	}
}


uint8_t connection_GetInData(int byteNr){
	if(byteNr < STEERING_TX_REGSIZE){
		return steering_in[byteNr];
	}
	return 0;
}

uint8_t connection_GetOutData(int bytenr){
	if(bytenr < STEERING_RX_REGSIZE){
		return steering_out[bytenr];
	}
	return 0;
}

void connection_SetOutData(int byteNr, uint8_t newValue){
	if(byteNr < STEERING_RX_REGSIZE){
		steering_out[byteNr] = newValue;
	}
}