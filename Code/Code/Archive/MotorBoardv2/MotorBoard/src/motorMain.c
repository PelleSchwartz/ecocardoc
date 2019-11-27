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
	
//	_delay_ms(5000); //First thing,  5s delay, maybe solves problem.
	

	{
		//Start reset by using watchdog
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
	
//	testLED();
	
	pwm_init();
	adc_init(ADC_PRESCALER_DIV128);
	initiateRS485();
	rs232_init();
	timer_init();
	sei();
	
	DDRA |= (1<<2); //Horn.
	DDRA |= (1<<1); //KERS OUT //no kers
	
//	testDataValidation1();
//	testDataValidation2();
//	uartreceiveTest();
//	uartreceiveTestInterrupt();	//receive w. interrupt test
//	uartSendTest();
//	uartreceiveTest2();
//	test_rs485();
//	test2_rs485();
//	test3_rs485();
//	test4_rs485();
//	freqTest();
//	dutyCycleTest();
//	batteryTest();

	unsigned char old_rio = 1;
	unsigned char OLD_STEERING_BLINK_L = 0;
	unsigned char OLD_STEERING_BLINK_H = 0;
	
	
	
	while(1){
		
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
			light_B_reg[STOP_LIGHT_STATE] = (!(PINF & (1<<1))) ? 1 : 0;//TODO see if it works PINF1 is the brake.
		
			//HORN
			if(steering_reg[STEERING_HORN]){
				SETBIT(PORTA, 2); //Horn out.
			}
			else{
				CLRBIT(PORTA, 2);
			}	

			/*
			//KERS
			if(steering_reg[STEERING_KRES_START] && (kers_burn == 0)) {
				// Activate KERS burn
				//rs232_EPOS_transmit(0x21);
				//SETBIT(PORTD, LED2_PIN_D);
				writeObject(0x11, 0x6040, 0x00, 0x0080, 0xF839);	// Clear Fault
				writeObject(0x11, 0x6060, 0x00, 0x0003, 0xF4C5);	// Set Operation Mode (Profile Velocity)
				writeObject(0x11, 0x6402, 0x00, 0x000A, 0xB6C0);	// Set Motor Type
				writeObject(0x11, 0x6410, 0x01, 0x16DA, 0x12A3);	// Set Nominal Current
				writeObject(0x11, 0x6410, 0x02, 0x2DB4, 0x4EED);	// Set Max Output Current
				writeObject(0x11, 0x6410, 0x05, 0x0153, 0x74E4);	// Set time Winding Constant
				writeObject(0x11, 0x6083, 0x00, 0x0073, 0xECAC);	// Set Profile Acceleration
				writeObject(0x11, 0x6084, 0x00, 0x0073, 0xF5E8);	// Set Profile Deceleration
				writeObject(0x11, 0x6040, 0x00, 0x0006, 0x71C3);	// Set Enable 1
				writeObject(0x11, 0x6040, 0x00, 0x000F, 0xEF52);	// Set Enable 2
				
				//writeObject(0x11, 0x6040, 0x00, 0x0080, 0xF839);	// Clear Fault
				writeObject(0x11, 0x2079, 0x01, 0x000F, 0xC192);	// Configure Clutch
				writeObject(0x11, 0x2078, 0x01, 0x8005, 0x630A);	// Configure Clutch State
				writeObject(0x11, 0x2078, 0x02, 0x8000, 0x6628);	// Configure Clutch Mask
				writeObject(0x11, 0x2078, 0x03, 0x0000, 0x1141);	// Configure Clutch Polarity
				writeObject(0x11, 0x2078, 0x01, 0x8000, 0x88FA);	// Activate Clutch
				
				writeObject(0x11, 0x60FF, 0x00, 0xFFFFFDF3, 0xBA38);	// Set Move Velocity
				writeObject(0x11, 0x6040, 0x00, 0x000F, 0xEF52);	// Set Enable 2 (Move!)
				//kers_reg_w[KERS_W_BOOST] = 1;
				
				kers_burn = 1;
			}
			
			if(!steering_reg[STEERING_KRES_START] && (kers_burn == 1)) {
				//CLRBIT(PORTA, 6);//was apparantly just an LED
				// Deactivate KERS burn
				writeObject(0x11, 0x6040, 0x00, 0x0080, 0xF839);	// Clear Fault
				writeObject(0x11, 0x2079, 0x01, 0x000F, 0xC192);	// Configure Clutch
				writeObject(0x11, 0x2078, 0x01, 0x8005, 0x630A);	// Configure Clutch State
				writeObject(0x11, 0x2078, 0x02, 0x8000, 0x6628);	// Configure Clutch Mask
				writeObject(0x11, 0x2078, 0x03, 0x0000, 0x1141);	// Configure Clutch Polarity
				writeObject(0x11, 0x2078, 0x01, 0x0000, 0x55C2);	// Deactivate Clutch
				
				writeObject(0x11, 0x6040, 0x00, 0x0000, 0xC363);	// Set Disable State
				
				//kers_reg_w[KERS_W_BOOST] = 0;
				kers_burn = 0;
			}

			if(steering_reg[STEERING_KRES_STOP] && (kers_brake == 0)){
				// Activate KERS brake
				
				//kers_reg_w[KERS_W_STORE] = 1;
				//SETBIT(PORTA, 6);//TODO need to know what this is
				writeObject(0x11, 0x6040, 0x00, 0x0080, 0xF839);	// Clear Fault
				writeObject(0x11, 0x2079, 0x01, 0x000F, 0xC192);	// Configure Clutch
				writeObject(0x11, 0x2078, 0x01, 0x8005, 0x630A);	// Configure Clutch State
				writeObject(0x11, 0x2078, 0x02, 0x8000, 0x6628);	// Configure Clutch Mask
				writeObject(0x11, 0x2078, 0x03, 0x0000, 0x1141);	// Configure Clutch Polarity
				writeObject(0x11, 0x2078, 0x01, 0x8000, 0x88FA);	// Activate Clutch
				
				kers_brake = 1;
			}
			
			if(!steering_reg[STEERING_KRES_STOP] && (kers_brake == 1)){
				
				//kers_reg_w[KERS_W_STORE] = 0;
				// De-activate KERS brake
				writeObject(0x11, 0x6040, 0x00, 0x0080, 0xF839);	// Clear Fault
				writeObject(0x11, 0x2079, 0x01, 0x000F, 0xC192);	// Configure Clutch
				writeObject(0x11, 0x2078, 0x01, 0x8005, 0x630A);	// Configure Clutch State
				writeObject(0x11, 0x2078, 0x02, 0x8000, 0x6628);	// Configure Clutch Mask
				writeObject(0x11, 0x2078, 0x03, 0x0000, 0x1141);	// Configure Clutch Polarity
				writeObject(0x11, 0x2078, 0x01, 0x0000, 0x55C2);	// Deactivate Clutch
				kers_brake = 0;
			}
			*/
			
			//
			rs485_send_packet(ID_STEERING_WHEEL, OP_WRITE, 0, 0, STEERING_REGSIZE_W);
			
			//KERS
//			rs485_send_packet(ID_KERS, OP_WRITE, 0, 0, KERS_W_REG_SIZE);
			
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
		steering_reg_w[STEERING_CAP_VOLTAGE] = volt; //TODO need to know what this is. (KERS)
		
		//Gear servo
		uint16_t gearServo = rio_rx[RIO_REC_SERVO_GEAR_L] | (rio_rx[RIO_REC_SERVO_GEAR_H] << 8);
		SetPWMDutyGearMicro(gearServo);
		uint16_t speedServo = rio_rx[RIO_REC_SERVO_SPLAELD_L] | (rio_rx[RIO_REC_SERVO_SPLAELD_H] << 8);
		SetPWMDutySpeedMicro(speedServo);
		//TODO other servo. rio_rx[RIO_REC_SERVO_SPLAELD]
		//TODO starter rio_rx[RIO_REC_STARTER], line 141 should be deleted if this is used i believe.
		
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

