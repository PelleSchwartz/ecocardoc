//Header files
//#include <asf.h>
#include "buttons.h"
#include "global.h"
#include "Components/Connection.h"
#include "Hardware/RS485.h"
#include "rs485protocol.h"

#include <header/init.h>
#include <stdio.h>
#include <string.h>
#include <header/DOGXL-160-7.h>
#include <stdint.h>
#include <header/rgb.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Function prototypes
void button_update(void);
void out_reg_update(void);
void RGB_LIGHT_UPDATE(void);
void uartEchoTest(void);
void uartSpamTest(void);

//Global variables
//volatile unsigned char out_reg[OUT_REG_SIZE] = {0};
//volatile unsigned char in_reg[IN_REG_SIZE] = {0};
volatile unsigned char button_reg[BUTTON_REG_SIZE] = {0};

//Lap time
volatile static uint16_t counter = 0;
volatile static uint16_t counter3 = 0;

volatile unsigned char sec = 0;
volatile unsigned char min = 0;
volatile unsigned char sec2 = 0;
volatile unsigned char min2 = 0;

volatile unsigned char page = 1;

	
volatile uint8_t rs485buffer;
volatile uint8_t rs485state = 0;
volatile unsigned char timer0_interupt_flag1000 = 0;
volatile unsigned char timer0_interupt_flag1000_2 = 0;
volatile unsigned char timer0_interupt_flag500 = 0;

volatile static uint32_t millis = 0;

volatile uint16_t pagetimer = 0;
volatile unsigned short pottimer = 100;
unsigned char GearVal = 3;
unsigned char KersVal = 0;
volatile unsigned char firstrun=1;

int main (void)
{
	//_delay_ms(100);
	
	unsigned short rpm;

	//TODO list
	//LCD: Background PWM
	//Piezo: Timer PWM
	//LEDs:
	//Buttons: debounce
	//Analog lysmåler
	//Warnings,
	//Timer til delay
	//Potmeter gemme i eeprom + sætte delay på
	//Aldrig sende hele tiden til led'er
		
	//Initialize
	//Interrupts off
	cli();
	IO_Init();
	timer_init();
	
	SPI_MasterInit();
	_delay_ms(50);
	LCDInit();
	_delay_ms(5);
	LCDClean();
	_delay_ms(5);
	Clear_LEDs();
	_delay_ms(5);

	//Enable interrupts
	_delay_ms(10);
	//connection to motor board
	connection_initiate(STEERING_TX_REGSIZE, STEERING_RX_REGSIZE, RS485_ID_STEERING_WHEEL);
	sei();
	
	//uartSpamTest();
	//uartEchoTest(); //<<<Delete
	//
	//out_reg[POT_METER_H] = (POT1_START>>8)&0xFF;
	//out_reg[POT_METER_L] = POT1_START&0xFF;
	//out_reg[NEUTRAL] = 1;
	connection_SetOutData(STEERING_POT_METER_H, (POT1_START>>8)&0xFF);
	connection_SetOutData(STEERING_POT_METER_L, POT1_START&0xFF);
	connection_SetOutData(STEERING_NEUTRAL, 1);
	
	//Gear color on
	//Set_LED_Color(5,255,255,255);
	/*for(unsigned char i=1;i<27;i++)
	{
		Set_LED_Color(i,255,255,255);
		_delay_ms(500);
	}*/
	
	Clear_LEDs();
	
	LCDIntro();
		
	while(1)
	{
		// Make sure LCD is initialized after 10 seconds
		if(min==0 && sec==10) {
			LCDInit();
		}
		
		// Update registers
		button_update();
		out_reg_update();
		
		if(connection_GetInData(STEERING_F_BLINK_R) || connection_GetInData(STEERING_F_BLINK_L) ){
			LCDColor(1);
		} else {
			LCDColor(3);
		}
		
		if((GearVal==1) && (connection_GetOutData(STEERING_BURN) == 1)){
			if(firstrun == 1){
				//LapTimePage(9,5,4);
				//LapTimePage(0,0,4); //Clear
				firstrun = 2;
				min = 0;
				sec = 0;
			}else{
				//LapTimePage(min,sec,1); //Push
			}
		}			
		
		
		
		//Stop watch timer
		if(timer0_interupt_flag1000)
		{	
			if(sec<59)
			{
				sec++;
			}else
			{
				sec=0;
				if((min<99)){ //&& (firstrun==2)){
					min++;
				}else{
					min = 0;
				}				
			}
			
			timer0_interupt_flag1000 = 0;
		}
		
		if(timer0_interupt_flag1000_2)
		{
			if(sec2<59)
			{
				sec2++;
			}else
			{
				sec2=0;
				if((min2<99)){ //&& (firstrun==2)){
					min2++;
					}else{
					min2 = 0;
				}
			}
			timer0_interupt_flag1000_2 = 0;
			
		}
		
				
		//Screen update timer
		if(timer0_interupt_flag500)
		{
			if(pagetimer==0)
			{
				//if(page==1){
				rpm = (connection_GetInData(STEERING_RPM_H)<<8) | connection_GetInData(STEERING_RPM_L);

				uint16_t volt = connection_GetInData(STEERING_BAT_VOLTAGE_L) | (connection_GetInData(STEERING_BAT_VOLTAGE_H) << 8);
				SpeedoPage3(connection_GetInData(STEERING_KMH),connection_GetOutData(STEERING_CLOSED_LOOP), connection_GetOutData(STEERING_IDLE),rpm,min,sec,min2,sec2,connection_GetInData(STEERING_WATER_TEMP),connection_GetInData(STEERING_OIL_TEMP),GearVal,KersVal,volt ,connection_GetOutData(STEERING_BLINK_L),connection_GetOutData(STEERING_BLINK_R));

				timer0_interupt_flag500 = 0;
			}
		}
	}
}

void RGB_LIGHT_UPDATE(void)
{
	static unsigned char LWiper = 0, LClosedLoop = 0, LHYPERSPEED = 0, LIdle = 0, LLONOFF = 0, LLLength = 0;
	
	if(connection_GetOutData(STEERING_WIPER) && !LWiper)
	{
		Set_LED_Color(6,255,255,255);
		LWiper = 1;
	}else if(!connection_GetOutData(STEERING_WIPER) && LWiper)
	{
		Set_LED_Color(6,0,0,0);
		LWiper = 0;
	}
	
	if(connection_GetOutData(STEERING_CLOSED_LOOP) && !LClosedLoop)
	{
		Set_LED_Color(20,255,255,255);
		LClosedLoop = 1;
	}else if(!connection_GetOutData(STEERING_CLOSED_LOOP) && LClosedLoop)
	{
		Set_LED_Color(20,0,0,0);
		LClosedLoop = 0;
	}
	
	if(connection_GetOutData(STEERING_IDLE) && !LIdle)
	{
		Set_LED_Color(21,255,255,255);
		LIdle = 1;
	}else if(!connection_GetOutData(STEERING_IDLE) && LIdle)
	{
		Set_LED_Color(21,0,0,0);
		LIdle = 0;
	}
	
	if(connection_GetOutData(STEERING_HYPERSPEED) && !LHYPERSPEED)
	{
		Set_LED_Color(22,255,255,255);
		LHYPERSPEED = 1;
	}else if(!connection_GetOutData(STEERING_HYPERSPEED) && LHYPERSPEED)
	{
		Set_LED_Color(22,0,0,0);
		LHYPERSPEED = 0;
	}
	
	if(connection_GetOutData(STEERING_LIGHT_NORMAL) && !LLONOFF)
	{
		Set_LED_Color(4,255,255,255);
		LLONOFF = 1;
	}else if(!connection_GetOutData(STEERING_LIGHT_NORMAL) && LLONOFF)
	{
		Set_LED_Color(4,0,0,0);
		LLONOFF = 0;
	}
	
	if(connection_GetOutData(STEERING_LIGHT_HIGH_BEAM)==2 && LLLength != 2)
	{
		Set_LED_Color(3,255,255,255);
		LLLength = 2;
	}else if(connection_GetOutData(STEERING_LIGHT_HIGH_BEAM) == 0 && LLLength != 0)
	{
		Set_LED_Color(3,0,0,0);
		LLLength = 0;
	}else if(connection_GetOutData(STEERING_LIGHT_HIGH_BEAM) == 1 && LLLength != 1)
	{
		Set_LED_Color(3,128,128,128);
		LLLength = 1;
	}
}	
		
void connection_XorOut(int byteNr, uint8_t value){
	uint8_t temp = connection_GetOutData(byteNr);
	temp ^= value;
	connection_SetOutData(byteNr, temp);
}

void connection_ClearOut(void) {
	for(int i = STEERING_GEAR12; i < STEERING_RX_REGSIZE; i++){
		connection_SetOutData(i, 0);
	}
}

void out_reg_update(void) {
	//static unsigned char R_BLINK_BUTTON_STATE=0, last_R_BLINK_BUTTON_STATE = 0;
	//static unsigned char L_BLINK_BUTTON_STATE=0, last_L_BLINK_BUTTON_STATE = 0;
	//unsigned char KERS_STATE = 0;
	
	static unsigned char StateLightLength = 1, StateLightState = 1, StateNeutral = 1, StateWiper = 1, StateBlinkL = 1, StateBlinkR = 1, StateAutoGear = 1, StateClosedLoop = 1, StateIdle = 1, StateFan = 1, StateGear1 = 1, StateGear2 = 1;
	//static unsigned char R_BLINK_STATE = 1,L_BLINK_STATE = 1,LIGHT_STATE = 1,WIPER_STATE = 1,FAN_STATE = 1,HORN_STATE = 1,AUTO_GEAR_STATE = 1,NEUTRAL_STATE = 1,GEAR12_STATE = 1,MAN_INJECT_STATE = 1,GEARUP_STATE = 1,GEARDOWN_STATE = 1, LAPTIME_STATE;
	//static unsigned char R_BLINK_STATE = 1,L_BLINK_STATE = 1,LIGHT_STATE = 1,WIPER_STATE = 1,FAN_STATE = 1,HORN_STATE = 1,AUTO_GEAR_STATE = 1,NEUTRAL_STATE = 1,GEAR12_STATE = 1,MAN_INJECT_STATE = 1,GEARUP_STATE = 1,GEARDOWN_STATE = 1;
	//static uint16_t POT1 = POT1_START;
	
	/*
	QuickGuide
	The states are used to see if u need to check if the button is pressed down.
	If you have to check and the button is  pressed, then do whatever needs to be done on button press, and set button ready state to 0 (not ready).
	Afterward if button is not pressed, button is ready to be pressed again.
	Exception to this is Pot buttons.
	
	
	*/
	
	//Clear outdata if read since last time.
	if(connection_OldOutData()) {
		connection_ClearOut();
	}
	
	//Start motor
	connection_SetOutData(STEERING_BURN, button_reg[BTN_Start]);
	//HORN
	connection_SetOutData(STEERING_HORN, button_reg[BTN_Horn]);
	//POT
	connection_SetOutData(STEERING_POT_DOWN, button_reg[BTN_Pot_down]);
	connection_SetOutData(STEERING_POT_UP, button_reg[BTN_Pot_up]);
	
	//R_BLINK
	if(StateBlinkR)
	{
		if(button_reg[BTN_BlinkRight])
		{
			connection_SetOutData(STEERING_BLINK_R, 1);
			connection_SetOutData(STEERING_BLINK_L, 0);	
			StateBlinkR = 0;
		}
	}
	if(button_reg[BTN_BlinkRight]==0)
	{
		StateBlinkR = 1;
	}
	

	//L_BLINK
	if(StateBlinkL)
	{
		if(button_reg[BTN_BlinkLeft])
		{
			connection_SetOutData(STEERING_BLINK_L, 1);
			connection_SetOutData(STEERING_BLINK_R, 0);
			StateBlinkL = 0;	
		}	
	}
	if(button_reg[BTN_BlinkLeft]==0)
	{
		StateBlinkL = 1;
	}
	
	
	//LightState
	
	if(StateLightState)
	{
		if(button_reg[BTN_LightS])
		{
			connection_SetOutData(STEERING_LIGHT_NORMAL, 1);
			StateLightState = 0;	
		}
	}
	if(button_reg[BTN_LightS]==0)
	{
		StateLightState = 1;
	}
	
	//LightLength
	if(StateLightLength) //used to control the length?
	{
		if(button_reg[BTN_Light_3st])
		{
			uint8_t temp = (connection_GetOutData(STEERING_LIGHT_HIGH_BEAM) + 1) % 3;
			connection_SetOutData(STEERING_LIGHT_HIGH_BEAM, temp);
			StateLightLength = 0;
		}
	}
	if(button_reg[BTN_Light_3st]==0)
	{
		StateLightLength = 1;
	}
	
	//WIPER
	if(StateWiper)
	{
		if(button_reg[BTN_Wiper])
		{
			connection_SetOutData(STEERING_WIPER, 1);
			StateWiper = 0;
		}
	}
	if(button_reg[BTN_Wiper]==0)
	{
		StateWiper = 1;
	}
	
	//FAN
	if(StateFan)
	{
		if(button_reg[BTN_Fan])
		{
			connection_SetOutData(STEERING_FAN, 1);
			StateFan = 0;
		}
	}
	if(button_reg[BTN_Fan]==0)
	{
		StateFan = 1;
	}
	
	
	//Idle
	if(StateIdle){
		if(button_reg[BTN_Idle]){
			connection_SetOutData(STEERING_IDLE, !connection_GetOutData(STEERING_IDLE));
			StateIdle = 0;
		}
	}
	if(button_reg[BTN_Idle] == 0){
		StateIdle = 1;
	}

	//AUTOGEAR
	if(StateAutoGear)
	{
		if(button_reg[BTN_AutoGear])
		{
			if(connection_GetOutData(STEERING_AUTOGEAR) == 0){
				connection_SetOutData(STEERING_NEUTRAL, 0x00);
				connection_SetOutData(STEERING_AUTOGEAR, 0x01);
				GearVal = 4;
				GearPage(4);
				pagetimer=500;
			}
			else{
				connection_SetOutData(STEERING_AUTOGEAR, 0x00);
				if(!connection_GetInData(STEERING_GEAR)){
					connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
					connection_SetOutData(STEERING_GEAR_UP, 0x00);
					connection_SetOutData(STEERING_NEUTRAL, 0x01);
					GearVal = 3;
					GearPage(3);
					pagetimer=500;
				}
				else{
					if(connection_GetInData(STEERING_GEAR) - 1){
						connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
						connection_SetOutData(STEERING_GEAR_UP, 0x01);
					}else{
						connection_SetOutData(STEERING_GEAR_DOWN, 0x01);
						connection_SetOutData(STEERING_GEAR_UP, 0x00);
					}
					
					GearVal = connection_GetInData(STEERING_GEAR);
					GearPage(GearVal);
				}
				
			}
			StateAutoGear = 0;
		}
	}
	if(button_reg[BTN_AutoGear]==0)
	{
		StateAutoGear = 1;
	}
	
	//NEUTRAL
	if(StateNeutral)
	{
		if(button_reg[BTN_Neutral])
		{
			if(connection_GetOutData(STEERING_AUTOGEAR) == 0){
				connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
				connection_SetOutData(STEERING_GEAR_UP, 0x00);
				connection_SetOutData(STEERING_NEUTRAL, 0x01);

				GearVal = 3;
				GearPage(3);
				pagetimer=500;
				//Set_LED_Color(5,255,255,255);
			}
			StateNeutral = 0;
		}
	}
	if(button_reg[BTN_Neutral]==0)
	{
		StateNeutral = 1;
	}
	
	//GEAR1
	if(StateGear1)
	{
		if(button_reg[BTN_Gear1])
		{
			if(connection_GetOutData(STEERING_AUTOGEAR) == 0){
				connection_SetOutData(STEERING_GEAR_DOWN, 0x01);
				connection_SetOutData(STEERING_GEAR_UP, 0x00);
				connection_SetOutData(STEERING_NEUTRAL, 0x00);
				if(GearVal != 3) {
					GearVal = 1;
					GearPage(1);
				}
				pagetimer=500;
				//Set_LED_Color(5,0,0,0);
			}
			StateGear1 = 0;
		}
	}
	if(button_reg[BTN_Gear1]==0)
	{
		StateGear1 = 1;
		connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
	}
	
	//GEAR2
	if(StateGear2)
	{
		if(button_reg[BTN_Gear2])
		{
			if(connection_GetOutData(STEERING_AUTOGEAR) == 0){
				connection_SetOutData(STEERING_GEAR_DOWN, 0x00);
				connection_SetOutData(STEERING_GEAR_UP, 0x01);
				connection_SetOutData(STEERING_NEUTRAL, 0x00);
				if(GearVal == 3) {
					GearVal = 1;
					GearPage(1);	
				} else {
					GearVal = 2;
					GearPage(2);
				}
				pagetimer=500;
				//Set_LED_Color(5,0,0,0);
			}
			StateGear2 = 0;
		}
	}
	if(button_reg[BTN_Gear2]==0)
	{
		StateGear2 = 1;
		connection_SetOutData(STEERING_GEAR_UP, 0x00);
	}	
	
	//ClosedLoop
	if(StateClosedLoop){
		if(button_reg[BTN_ClosedLoop]){
			connection_SetOutData(STEERING_CLOSED_LOOP, !connection_GetOutData(STEERING_CLOSED_LOOP));
			StateClosedLoop = 0;
		}
	}
	if(button_reg[BTN_ClosedLoop] == 0){
		StateClosedLoop = 1;
	}

	
	//HYPERSPEED
	if(button_reg[BTN_HYPERSPEED]){
		connection_SetOutData(STEERING_HYPERSPEED, 0x01);
		sec2 = 0;
		min2 = 0;
		counter3 = 0;
	}
	
	//Party time
	if(button_reg[BTN_PARTY_TIME]){
		LCDInit();
		connection_SetOutData(STEERING_PARTY_TIME, 0x01);
		sec = 0;
		min = 0;
		counter = 0;
	} else {
		connection_SetOutData(STEERING_PARTY_TIME, 0x00);
	}
	
	//out_reg[POT_METER_H] = (POT1>>8)&0xFF;
	//out_reg[POT_METER_L] = POT1&0xFF;
}

void button_update(void)
{
	button_reg[BTN1] = PINB & (1<<PINB0);
	button_reg[BTN2] = (PINB & (1<<PINB1)) >> 1;
	button_reg[BTN3] = (PINB & (1<<PINB2)) >> 2;
	button_reg[BTN4] = (PINB & (1<<PINB3)) >> 3;

	button_reg[BTN17] = (PINC & (1<<PINC0));
	button_reg[BTN5] = (PINC & (1<<PINC1)) >> 1;
	button_reg[BTN6] = (PINC & (1<<PINC2)) >> 2;
	button_reg[BTN8] = (PINC & (1<<PINC3)) >> 3;
	button_reg[BTN7] = (PINC & (1<<PINC4)) >> 4;
	button_reg[BTN18] = (PINC & (1<<PINC5)) >> 5;
	button_reg[BTN16] = (PINC & (1<<PINC6)) >> 6;
	button_reg[BTN15] = (PINC & (1<<PINC7)) >> 7;

	button_reg[BTN9] = (PINA & (1<<PINA2)) >> 2;
	button_reg[BTN10] = (PINA & (1<<PINA3)) >> 3;
	button_reg[BTN11] = (PINA & (1<<PINA4)) >> 4;
	button_reg[BTN12] = (PINA & (1<<PINA5)) >> 5;
	button_reg[BTN13] = (PINA & (1<<PINA6)) >> 6;
	button_reg[BTN14] = (PINA & (1<<PINA7)) >> 7;

}

void uartSpamTest(void){
	rs485_DisableInterrupt();
	rs485_TransmitMode();
	while(1){
		//rs485_Transmit(counter);
		rs485_Transmit(0x42);
		_delay_us(100);
	}
}

void uartEchoTest(void){
	rs485_DisableInterrupt();
	rs485_ReceiveMode();
	uint8_t received;
	uint8_t index = 0;
	uint8_t allReceived[30];
	while(1){
		index = 0;
		received = rs485_receive();
		if(received != 0x00){
			allReceived[index] = received;
			index = 1;
			for(uint16_t i = 0; i < 20000; i++){
				if(UCSR1A & (1<<RXC1)){
					i = 0;
					received = rs485_receive();
					allReceived[index] = received;
					if(index < 29){
						index++;
					}
				}
			}
			
			rs485_TransmitMode();
			for(char unsigned i = 0; i < 30; i++){
				if(allReceived[i] == 0){
					rs485_Transmit(i);
				}
				else{
					rs485_Transmit(allReceived[i]);
					allReceived[i] = 0;
				}
				
			}
			rs485_WaitForTransmit();
			rs485_ReceiveMode();
		}
	}
}

//rs485 receive
//ISR(USART1_RX_vect) {
	 //static uint16_t op=0, client=0, regaddr=0, len=0,regcount=0,cc=0;
	//uint8_t i;
//
	//rs485buffer = UDR1;
	//rs485state++;
	//switch(rs485state) {
		//case 0:
		//// Do nothing
		//break;
		//case 1: // First byte is received
		//if(rs485buffer != 0xA5) rs485state = 0;
		//break;
		//case 2: // Second byte is received
		//client = (rs485buffer>>1) & 0x7F;
		//op = rs485buffer & 0x01;
		////if(client == 2) PORTD ^= (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD6);
		//
		//break;
		//case 3: // Address
				//
		//if(client == RS485ADDRESS) {
			//regaddr = rs485buffer;
		//}
		//break;
		//case 4: // Package Length
		//
		//len = rs485buffer;
		////if(0){
		//if(op==OP_TX) {	// If operation is respond, no more bytes will be received and hence we must send in state 4
			//PORTB |= (1<<PORTB4);
			//for(i=0;i<len;i++)
			//{
				//rs485_transmit(out_reg[regaddr+i]);
				//out_reg[regaddr+i] = 0x00; //When button state have been send, we will ignore it until its repressed.
			//}
			////_delay_ms(2);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//PORTB &= ~(1<<PORTB4);			
			//rs485state = 0;
		//}
		//break;
		//
		//case 5:
		//rs485state = 0;
		//
		//if(op==OP_RX) {
			//if(client == RS485ADDRESS){	
				//cc++;
				//if(cc > 8){
					///*PORTD |= (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD6);*/
				//}
				//if(regcount<(len-1)){
					//in_reg[regaddr+regcount] = rs485buffer;
					//regcount++;
					//rs485state = 4;
				//}
				//else{
					//if(regcount ==(len-1)){
						////PORTD ^= (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD6);
						//in_reg[regaddr+regcount] = rs485buffer;
						//regcount=0;
						//}else{
						//rs485state = 0;
						//regcount=0;
					//}
				//}			
			//}
		//}					
		//break;
	//}
//}

ISR (TIMER1_COMPA_vect)
{
//	volatile static uint16_t counter = 0;  //made global cause I want to set it to zero when i reset the timer.
	volatile static uint16_t counter2 = 0;
	
	counter++;
	counter2++;	
	counter3++;	
	millis++;
		
	if(pagetimer>0){
		pagetimer--;
	}
	
	if(pottimer>0){
		pottimer--;
	}
	if(counter==500){
		counter = 0;
		timer0_interupt_flag1000 = 1;
	}
	if(counter3==500){
		counter3 = 0;
		timer0_interupt_flag1000_2 = 1;
	}
	if(counter2==50){
		counter2 = 0;
		timer0_interupt_flag500 = 1;
	}
	// action to be done every 1 ms
}

/*
ISR(PCINT0_vect) {
		
}

ISR(PCINT1_vect)
{
	
}

ISR(PCINT2_vect)
{

}
*/