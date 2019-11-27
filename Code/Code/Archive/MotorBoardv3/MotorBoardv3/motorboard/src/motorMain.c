/*
 * CFile1.c
 *
 * Created: 16-03-2015 17:34:18
 *  Author: Kim
 */ 

#include "asf.h"
#include "LEDs.h"
#include "BilADC.h"
#include "rs485.h"
#include "rs232.h"
#include "Timer.h"
#include "DutyCycle.h"
#include "EEPROM.h"
#include "Test.h" //bugged

int main(void){
	
	initiateLEDs();
	DDRK |= (1<<0);  //Starter changed from PA4 to PK0
	CLRBIT(PORTK,0); //Starter changed from PA4 to PK0
	
	//Start reset by using watchdog
	{
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
	sei();
	
	DDRA |= (1<<2); //Horn.
	DDRA |= (1<<1); //KERS OUT //no kers
	
	unsigned char old_rio = 1;
	unsigned char OLD_STEERING_BLINK_L = 0;
	unsigned char OLD_STEERING_BLINK_H = 0;
	
	unsigned char volt = 0;
	
	char jackUpdateDataID = 0; //Which data
	//1: Message
	//2: Water
	//3: Oil
	//4: Speed
	//5: RPM
	//6: Lambda
	//7: Battery
	//8: Km/L
	char jackUpdateDataState = 1; //8 parts for each
	uint16_t tempjackUpdateData = 0;	// 16bit DATA
	unsigned char i = 0;
	unsigned char idToggleCounter = 0;
	unsigned char LRC = 0;
	short freqTable[19] = {60,66,70,75,80,90,100,110,121,132,144,158,174,197,224,264,330,450,600};
		
	uint16_t RioLambda = 0;
	uint16_t RioKmperl = 0;	
	char toggle_state = 0;
	////////////////////////////////////////////////////////////////
	//Init 	minijack transfer - Output on OC3B

	DDRE|=(1<<PE4);
	ICR3 = freqTable[0]; //middlebit
	TCCR3A |= (1 << COM3B0);
	TCCR3B |= (1 << CS31) | (1 << WGM33)| (1 << WGM32);
	////////////////////////////////////////////////////////////////
	while(1){
		
		if(jackUpdateFlag){//jackUpdateFlag
			jackUpdateFlag = 0;
			switch(jackUpdateDataState){
				case 1: 
					switch(jackUpdateDataID){ //Read data to temp
						case 1:
							tempjackUpdateData = ('o'<<8) + ('k');
						break;
						case 2:
							tempjackUpdateData = (uint16_t) rio_rx[RIO_REC_WATER_TEMP]; // Watertemp
						break;
						case 3:
							tempjackUpdateData = (uint16_t) rio_rx[RIO_REC_OIL_TEMP]; // Oiltemp
						break;
						case 4:
							tempjackUpdateData = (uint16_t) rio_rx[RIO_REC_KPH]; // Speed
						break;
						case 5:
							tempjackUpdateData = (rio_rx[RIO_REC_RPM_H]<<8) + rio_rx[RIO_REC_RPM_L]; // RPM
						break;
						case 6:
							tempjackUpdateData = RioLambda;	// Lambda
							LED5On();
						break;
						case 7:
							tempjackUpdateData = (uint16_t) volt;	// Battery voltage
						break;
						case 8:
							tempjackUpdateData = RioKmperl;	// Km/l
							LED2On();
						break;
						case 42:
							tempjackUpdateData = 42;	// test
							LED2On();
						break;
						default:
							tempjackUpdateData = 12345;
						break;
					}
					
					if(tempjackUpdateData == 0) tempjackUpdateData = 1;	//Check if zero
					ICR3 = freqTable[0];								//Start Bit//Start bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 2:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 3:
					ICR3 = freqTable[(jackUpdateDataID & 0x000F) + 1];	//transfer ID Low
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 4:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 5:
					ICR3 = freqTable[((tempjackUpdateData >> 12) & 0x000F) + 1];	//transfer 1st High
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 6:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 7:
					ICR3 = freqTable[((tempjackUpdateData >> 8) & 0x000F) + 1]; //transfer 1st Low
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 8:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 9:
					ICR3 = freqTable[((tempjackUpdateData >> 4) & 0x000F) + 1]; //transfer 2nd High
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 10:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 11:
					ICR3 = freqTable[(tempjackUpdateData & 0x000F) + 1];	//transfer 2nd Low
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 12:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 13:
					LRC = 0;
					for(i = tempjackUpdateData; i != 0; i = (i>>1)) LRC += (i & 0x0001); //Calculate Checksum 
					ICR3 = freqTable[(LRC & 0x0F) + 1];				//transfer CheckSum
					TCNT3 = 0;
					LED2Off();
					LED5Off();
					jackUpdateDataState++; //Next step
				break;
				case 14:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 15:
					ICR3 = freqTable[17];		//Stop Bit
					TCNT3 = 0;
					jackUpdateDataState++; //Next step
				break;
				case 16:
					ICR3 = freqTable[18];	//middle bit
					TCNT3 = 0;
					jackUpdateDataState = 1;
					//Select ID			Priotet
						//1: Message	8
						//2: Water		5
						//3: Oil		6
						//4: Speed		3
						//5: RPM		2
						//6: Lambda		1
						//7: Battery	7
						//8: Km/L		4
						
					if(100 < ((rio_rx[RIO_REC_RPM_H]<<8) + rio_rx[RIO_REC_RPM_L])){
						if(jackUpdateDataID == 6){
							jackUpdateDataID = 5;
						}else{
							jackUpdateDataID = 6;
						}
					}
					else{
						idToggleCounter++;
						if(idToggleCounter==4) {
							jackUpdateDataID = 2;
						}
						else if(idToggleCounter==8) {
							jackUpdateDataID = 3;
						}
						else if(idToggleCounter==12) {
							jackUpdateDataID = 7;
						}
						else if(idToggleCounter==16) {
							jackUpdateDataID = 8;
						}
						else if(idToggleCounter==20) {
							jackUpdateDataID = 5;
						}
						else if(idToggleCounter==24) {
							jackUpdateDataID = 6;
							idToggleCounter = 0;
						}
						else{
							jackUpdateDataID = 4;
						}
					}
				break;
				default:
					jackUpdateDataState = 1;
					ICR3 = freqTable[18];	// Middle Bit
					TCNT3 = 0;
				break;
			}
		}		
	////////////////////////////////////////////////////////////////
	
		if(timer0_interupt_flag){
			//ask steering wheel.
			rs485_send_packet(ID_STEERING_WHEEL, OP_READ, 0, 0, STEERING_REGSIZE);
			
			//update rio.
			//TODO actually write on lambda H and L here. gets overwritten after.
			for(unsigned char i=0; i<(RIO_REGSIZE);i++){ 
				rio_reg[i] =  steering_reg[i];
			}
			uint16_t lambda = readLambdaSensor();
			rio_reg[RIO_LAMBDA_H] = ((uint8_t)((lambda>>8) & 0xff));
			rio_reg[RIO_LAMBDA_L] = ((uint8_t)(lambda & 0xff));
			
			rs232_sync();
		
			//Front lys
			light_reg[FRONT_LIGHT_STATE]  = steering_reg[STEERING_LIGHT];
			light_reg[AIRFAN_STATE]  = steering_reg[STEERING_FAN];
		
			//WIPER
			light_reg[WSVIPER_STATE] = steering_reg[STEERING_WIPER];
			
			//BAG lys
			light_B_reg[BACK_LIGHT_STATE]  = steering_reg[STEERING_LIGHT];
			light_B_reg[STARTER_LIGHT_STATE]  = rio_rx[RIO_REC_STARTER];
			light_B_reg[STOP_LIGHT_STATE] = (!(PINF & (1<<1))) ? 1 : 0;
		
			//HORN
			if(steering_reg[STEERING_HORN]){
				SETBIT(PORTA, 2); //Horn out.
			}
			else{
				CLRBIT(PORTA, 2);
			}	


			rs485_send_packet(ID_STEERING_WHEEL, OP_WRITE, 0, 0, STEERING_REGSIZE_W);
				
			//lys
			rs485_send_packet(ID_FRONT_LIGHT, OP_WRITE, 0, 0, FRONT_LIGHT_REGSIZE);
			rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
			
			//steering_reg
			//steering_reg_w
			//rio_reg
			//rio_rx
			//light_reg
			//light_B_reg
			//Testing area
			
			(light_reg[AIRFAN_STATE]) ? LED1On() : LED1Off();
			(light_reg[AIRFAN_STATE]) ? LED2On() : LED2Off();
			(!(PINF & (1<<1))) ? LED3On() : LED3Off();
			(light_reg[WSVIPER_STATE]) ? LED4On() : LED4Off();
			(steering_reg[STEERING_HORN]) ? LED5On() : LED5Off();
			
			_delay_ms(2);
			
			timer0_interupt_flag = 0;
		}
		//Lambda and km per L in 16 bit.
		RioLambda = rio_rx[RIO_REC_LAMBDA_L] | (rio_rx[RIO_REC_LAMBDA_H] << 8);	
		RioKmperl = rio_rx[RIO_REC_KMPERL_L] | (rio_rx[RIO_REC_KMPERL_H] << 8);
		//Rio
		if(rs232_interupt_flag==1){
			rs232_interupt_flag = 0;
		}
		
		//TODO battery voltage
		volt = readBatterySensor();
		
		steering_reg_w[STEERING_KPH] = rio_rx[RIO_REC_KPH];
		steering_reg_w[STEERING_RPM_H] = rio_rx[RIO_REC_RPM_H];
		steering_reg_w[STEERING_RPM_L] = rio_rx[RIO_REC_RPM_L];
		steering_reg_w[STEERING_VAND_T] = rio_rx[RIO_REC_WATER_TEMP];
		steering_reg_w[STEERING_OLIE_T] = rio_rx[RIO_REC_OIL_TEMP];
		steering_reg_w[STEERING_GEAR] = rio_rx[RIO_REC_GEAR]; //0 = neutral, 1 = 1, 2 = 2.
		steering_reg_w[STEERING_ENCODER_FAN] = 0;
		steering_reg_w[STEERING_CAP_VOLTAGE_L] = rio_rx[RIO_REC_KMPERL_L];	// Steering wheel number
		steering_reg_w[STEERING_CAP_VOLTAGE_H] = rio_rx[RIO_REC_KMPERL_H];
		
		//Gear servo
		uint16_t gearServo = rio_rx[RIO_REC_SERVO_GEAR_L] | (rio_rx[RIO_REC_SERVO_GEAR_H] << 8);
		SetPWMDutyGearMicro(gearServo);
		uint16_t speedServo = rio_rx[RIO_REC_SERVO_SPLAELD_L] | (rio_rx[RIO_REC_SERVO_SPLAELD_H] << 8);
		SetPWMDutySpeedMicro(speedServo);

		if(rio_rx[RIO_REC_STARTER]){
			SETBIT(PORTK,0); //Starter changed from PA4 to PK0
		}
		else{
			CLRBIT(PORTK, 0);//Starter changed from PA4 to PK0
		}
		
		
		if(steering_reg[STEERING_BLINK_L] != OLD_STEERING_BLINK_L){
			BLINK_TIMER = 0;
			OLD_STEERING_BLINK_L = steering_reg[STEERING_BLINK_L];
			light_B_reg[TURNSIG_B_LEFT_STATE] = 1;
			light_reg[TURNSIG_LEFT_STATE] = 1;
			steering_reg_w[STEERING_F_BLINK_L] = 1;
			timer0_interupt_flag2 =0;
		}
		if(steering_reg[STEERING_BLINK_L]){
			if(timer0_interupt_flag2){
				light_B_reg[TURNSIG_B_LEFT_STATE]  ^= 1;
				light_reg[TURNSIG_LEFT_STATE]  ^= 1;
				steering_reg_w[STEERING_F_BLINK_L]  ^= 1;
				timer0_interupt_flag2 =0;
			}
		}
		else{
			light_B_reg[TURNSIG_B_LEFT_STATE]  = 0;
			light_reg[TURNSIG_LEFT_STATE]  = 0;
			steering_reg_w[STEERING_F_BLINK_L] = 0;
		}
		
		if(steering_reg[STEERING_BLINK_H] != OLD_STEERING_BLINK_H){
			BLINK_TIMER = 0;
			OLD_STEERING_BLINK_H = steering_reg[STEERING_BLINK_H];
			light_B_reg[TURNSIG_B_RIGHT_STATE] = 1;
			light_reg[TURNSIG_RIGHT_STATE] = 1;
			steering_reg_w[STEERING_F_BLINK_R] = 1;
			timer0_interupt_flag2 =0;
		}
		if(steering_reg[STEERING_BLINK_H]){
			if(timer0_interupt_flag2){
				light_B_reg[TURNSIG_B_RIGHT_STATE]  ^= 1;
				light_reg[TURNSIG_RIGHT_STATE]  ^= 1;
				steering_reg_w[STEERING_F_BLINK_R] ^= 1;
				timer0_interupt_flag2 =0;
			}
		}
		else{
			light_B_reg[TURNSIG_B_RIGHT_STATE]  = 0;
			light_reg[TURNSIG_RIGHT_STATE]  = 0;
			steering_reg_w[STEERING_F_BLINK_R] = 0;
		}
	}
	
	return 1;
}

