/*
 * CFile1.c
 *
 * Created: 16-03-2015 17:34:18
 *  Author: Kim
 */ 

#include "asf.h"
#include "Hardware/LEDs.h"
#include "BilADC.h"
#include "Components/rs485protocol.h"
#include "rs232.h"
#include "Timer.h"
#include "DutyCycle.h"
#include <string.h>
#include <stdio.h>
#include "EEPROM.h"
#include "Test.h" //bugged
#include "jackTransmit.h"
#include "Hardware/BlueTooth.h"

#define DEBUG 1

uint8_t steering_reg[STEERING_REGSIZE]  = {0};
uint8_t steering_reg_w[STEERING_REGSIZE_W]  = {0};
uint8_t lights_reg[LIGHTS_REGSIZE] = {0};
uint8_t light_F_reg[FRONT_LIGHT_REGSIZE] = {0x01};
uint8_t light_F_out_reg[FL_OUT_BYTESIZE] = {0x00};
uint8_t light_B_reg[BACK_LIGHT_REGSIZE] = {0x00};
uint8_t rio_tx[RIO_REGSIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t rio_temp_rx[RIO_REC_REGSIZE] = {0x00};
uint8_t rio_rx[RIO_REC_REGSIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//uint8_t kers_reg_w[KERS_W_REG_SIZE]  = {0x00,0x00};
//uint8_t kers_reg_r[KERS_R_REG_SIZE]  = {0x00,0x00};
uint32_t timeout;

volatile uint8_t timer0_interupt_flag = 0,timer0_interupt_flag2=0;
volatile uint8_t rs232_interupt_flag = 0;
volatile uint8_t rs232_data[50] = {0};
volatile uint16_t BLINK_TIMER = 0;
volatile uint8_t jackUpdateFlag=0;

//volatile uint8_t kers_burn = 0;
//volatile uint8_t kers_brake = 0;

void _setDrivingLight(uint8_t signal);
void _setStopLight(uint8_t signal);
void _setStarterSignal(uint8_t signal);
void _setLeftSignal(uint8_t signal);
void _setRightSignal(uint8_t signal);
void _setHorn(uint8_t signal);

void initiateEverything(void);

void initiateEverything(void){
	
}

int main(void){
	
	initiateLEDs();
	DDRA |= (1<<4);
	CLRBIT(PORTA,4);
	
	//Start reset by using watchdog
	{//I can't remember why, but a restart was required in the start.
		unsigned char temp = MCUSR;
		if(TESTBIT(temp, WDRF)){
			MCUSR = 0; //needs to be set to 0 before wdt_disable. Could reed this instead of using the eeprom in the start.
			wdt_disable();
		}
		else{
			LED1On();
			LED2On();
			LED3On();
			LED4On();
			LED5On();
			LED6On();
			_delay_ms(5000);
			wdt_enable(WDTO_15MS);
			while(1);
		}
	}
	
	pwm_init();
	adc_init(ADC_PRESCALER_DIV128);
	initiateRS485();
	rs232_init();
	timer_init();
	
	blue_Init();
	sei();
	
	
	//uartreceiveTest2();
	//uartReveiveSpamTest();
	//uartEchoTest();
	
	//_delay_ms(500);
	//blue_Transmit_S("AT+GAPDEVNAME=DTU Dynamo\r");
	//_delay_ms(500);
	//blue_Transmit_S("ATZ\r");
	
	SETBIT(DDRA, DDA2); //Top Light.
	DDRK |= (1<<0); //requested byte // TODO WTF?
	
	unsigned char currenLeftSignal = 0;
	unsigned char currentRightSignal = 0;
	unsigned char currentLightState = 0;
	
	//uint16_t RioLambda = 0;
	//uint16_t RioKmperl = 0;	
		
	//Init minijack transfer - Output on OC3B
	//jack_init();
	
	if(DEBUG) {
		blue_Transmit_S("Motor has started\n\r");
	}

	while(1){
		//Minijack transfer
		//jackTransfer(RioLambda,RioKmperl);
	
		if(timer0_interupt_flag){
			//ask steering wheel.
			rs485_send_packet(ID_STEERING_WHEEL, OP_READ, 0, 0, STEERING_REGSIZE);
			rs485_send_packet(ID_FRONT_LIGHT, OP_READ, 0, 0, FL_OUT_BYTESIZE);
			
			//update rio.
			//Think this way of doing things hilarious so will keep it.
			for(unsigned char i=0; i<(RIO_REGSIZE);i++){ 
				rio_tx[i] =  steering_reg[i];
			}
			rio_tx[RIO_STOP] = 0;
			
			if(rio_tx[RIO_GEAR_UP]) {
				blue_Transmit_S("Gear Up\n\r");
			}
			if(rio_tx[RIO_GEAR_DOWN]) {
				blue_Transmit_S("Gear Down\n\r");
			}
			
			if(steering_reg[STEERING_START_MOTOR]){
				SETBIT(PORTK, PK0);
			}else{
				CLRBIT(PORTK, PK0);
			}
			
			//uint16_t lambda = readLambdaSensor();
			//rio_reg[RIO_LAMBDA_H] = ((uint8_t)((lambda>>8) & 0xff));
			//rio_reg[RIO_LAMBDA_L] = ((uint8_t)(lambda & 0xff));
			
			
			//Fan
			if(steering_reg[STEERING_FAN]){
				FLIPBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_FAN);
				FLIPBIT(lights_reg[LIGHTS_DIVERSE_FLAGS], LDF_FAN);
			}
			//Wiper
			if(steering_reg[STEERING_WIPER]){
				FLIPBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_WIPER);
				FLIPBIT(lights_reg[LIGHTS_DIVERSE_FLAGS], LDF_WIPER);
			}
			
			currentLightState = (steering_reg[STEERING_LIGHT]) ? !currentLightState : currentLightState;
			_setDrivingLight(currentLightState);
			
			_setStopLight(light_F_out_reg[FL_OUT_BRAKE]);
			_setStarterSignal(rio_rx[RIO_REC_STARTER]);
			_setStarterSignal(steering_reg[STEERING_START_MOTOR]);//<<<
			
			
			//Signals
			currentRightSignal = (steering_reg[STEERING_BLINK_R]) ? !currentRightSignal : currentRightSignal;
			currenLeftSignal = (steering_reg[STEERING_BLINK_L]) ? !currenLeftSignal : currenLeftSignal;
			_setRightSignal(currentRightSignal);
			_setLeftSignal(currenLeftSignal);
			
			//HORN
			_setHorn(steering_reg[STEERING_HORN]);
			
			// Top Pressure Tank Light
			if(steering_reg[STEERING_PARTY_TIME]) {
				FLIPBIT(PORTA, PA2);
			}	
			
			//Gear servo
			uint16_t gearServo = rio_rx[RIO_REC_SERVO_GEAR_L] | (rio_rx[RIO_REC_SERVO_GEAR_H] << 8);
			SetPWMDutyGear(gearServo);
			
			
			rs485_send_packet(ID_STEERING_WHEEL, OP_WRITE, 0, 0, STEERING_REGSIZE_W);
				
			//lys
			rs485_send_packet(ID_FRONT_LIGHT, OP_WRITE, 0, 0, FRONT_LIGHT_REGSIZE);
			rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
			
			rs232_sync();
			
			
			//(currentLightState) ? LED2On() : LED2Off();
			//(!(PINF & (1<<1))) ? LED3On() : LED3Off();
			(steering_reg[STEERING_HYPERSPEED]) ? LED4On() : LED4Off();
			(rio_tx[RIO_START]) ? LED5On() : LED5Off();
			
			(steering_reg[STEERING_START_MOTOR]) ? LED1On() : LED1Off();
			(rio_rx[RIO_REC_STARTER]) ? LED2On() : LED2Off();
			//(rio_rx[RIO_REC_GEAR] == 21) ? LED2On() : LED2Off();
			(rio_rx[RIO_REC_WATER_TEMP] == 42) ? LED3On() : LED3Off();
			//(rio_rx[RIO_REC_OIL_TEMP] == 42) ? LED4On() : LED4Off();
			//(rio_rx[RIO_REC_EXCESS_AIR] == 42) ? LED5On() : LED5Off();
			//(rio_rx[RIO_REC_FUEL_CONSUMPTION] == 42) ? LED6On() : LED6Off();
			
			timer0_interupt_flag = 0;
			
		}
		//Lambda and km per L in 16 bit.
		//RioLambda = rio_rx[RIO_REC_LAMBDA_L] | (rio_rx[RIO_REC_LAMBDA_H] << 8);	
		//RioKmperl = rio_rx[RIO_REC_KMPERL_L] | (rio_rx[RIO_REC_KMPERL_H] << 8);
		//Rio
		
		if(rs232_interupt_flag==1){
			rs232_interupt_flag = 0;
		}
		
		//TODO battery voltage
		unsigned char volt = readBatterySensor();
		
		steering_reg_w[STEERING_KPH] = rio_rx[RIO_REC_KPH];
		steering_reg_w[STEERING_RPM_H] = rio_rx[RIO_REC_RPM_H];
		steering_reg_w[STEERING_RPM_L] = rio_rx[RIO_REC_RPM_L];
		steering_reg_w[STEERING_VAND_T] = rio_rx[RIO_REC_WATER_TEMP];
		steering_reg_w[STEERING_OLIE_T] = rio_rx[RIO_REC_OIL_TEMP];
		steering_reg_w[STEERING_GEAR] = rio_rx[RIO_REC_GEAR]; //0 = neutral, 1 = 1, 2 = 2.
		steering_reg_w[STEERING_ENCODER_FAN] = 0;
		steering_reg_w[STEERING_CAP_VOLTAGE_H] = (volt >> 8);
		steering_reg_w[STEERING_CAP_VOLTAGE_L] = volt;

		if(rio_rx[RIO_REC_STARTER]){
			SETBIT(PORTA, PA4);
		}
		else{
			CLRBIT(PORTA, PA4);
		}
	}
	
	return 1;
}

void _setHorn(uint8_t signal){
	if(signal){
		SETBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_HORN);
	}
	else{
		CLRBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_HORN);
	}
}
void _setRightSignal(uint8_t signal){
	if(signal){
		SETBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_TURNRIGHT_LIGHT);
		SETBIT(light_B_reg[BACK_BYTEZERO], BL_B0_TURN_RIGHT);
		
		SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_RIGHT);
	}else{
		CLRBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_TURNRIGHT_LIGHT);
		CLRBIT(light_B_reg[BACK_BYTEZERO], BL_B0_TURN_RIGHT);
		
		CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_RIGHT);
	}
}
void _setLeftSignal(uint8_t signal){
	if(signal){
		SETBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_TURNLEFT_LIGHT);
		SETBIT(light_B_reg[BACK_BYTEZERO], BL_B0_TURN_LEFT);
		
		SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_LEFT);
	}else{
		CLRBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_TURNLEFT_LIGHT);
		CLRBIT(light_B_reg[BACK_BYTEZERO], BL_B0_TURN_LEFT);
		
		CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_TURN_LEFT);
	}
}
void _setStarterSignal(uint8_t signal){
	if(signal){
		SETBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_STARTER);
		SETBIT(light_B_reg[BACK_BYTEZERO], BL_B0_STARTER_LIGHT);
	}else{
		CLRBIT(lights_reg[LIGHTS_SIGNAL_FLAGS], LSF_STARTER);
		CLRBIT(light_B_reg[BACK_BYTEZERO], BL_B0_STARTER_LIGHT);
	}
}

void _setStopLight(uint8_t signal){
	if(signal){
		SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_STOPLIGHT);
		SETBIT(light_B_reg[BACK_BYTEZERO], BL_B0_STOP_LIGHT);
	}else{
		CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_STOPLIGHT);
		CLRBIT(light_B_reg[BACK_BYTEZERO], BL_B0_STOP_LIGHT);
	}
}

void _setDrivingLight(uint8_t signal){
	//rear light
	if(signal){
		SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BACKLIGHT);
		SETBIT(light_B_reg[BACK_BYTEZERO], BL_B0_NORMAL_LIGHT);
		}else{
		CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_BACKLIGHT);
		CLRBIT(light_B_reg[BACK_BYTEZERO], BL_B0_NORMAL_LIGHT);
	}	
	
	//front light
	if(signal){
		SETBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_NORMAL_LIGHT);
		SETBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_FRONTLIGHT_0);
	}else{
		CLRBIT(light_F_reg[FRONT_BYTEZERO], FL_B0_NORMAL_LIGHT);
		CLRBIT(lights_reg[LIGHTS_LIGHT_FLAGS], LLF_FRONTLIGHT_0);
	}
}