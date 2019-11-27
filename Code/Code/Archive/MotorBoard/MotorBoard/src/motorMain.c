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

void test_rs485(void);
void test2_rs485(void);
void test3_rs485(void);
void test4_rs485(void);
void freqTest(void);
void dutyCycleTest(void);
void batteryTest(void);
void uartreceiveTest(void);
void uartreceiveTest2(void);
void uartreceiveTestInterrupt(void);
void uartSendTest(void);
void testLED(void);

void testDataValidation1(void);
void testDataValidation2(void);

int main(void){
	
	initiateLEDs();
	DDRA |= (1<<4); //Starter
	CLRBIT(PORTA,4);
	
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

//	testDataValidation1();
//	testDataValidation2();
	
//	uartreceiveTest();

//	uartreceiveTestInterrupt();	//receive w. interrupt test
	
	
	pwm_init();
	adc_init(ADC_PRESCALER_DIV128);
	initiateRS485();
	rs232_init();
	timer_init();
	sei();
	
	DDRA |= (1<<2); //Horn.
	DDRA |= (1<<1); //KERS OUT //no kers
	

/*	
	while(1){
		_delay_ms(1000);
		LED6Toggle();
		FLIPBIT(PORTA, 4);
	}
*/
	
/*
	LED1On();
	LED2On();
	LED3On();
	LED4On();
	LED5On();
	LED6On();
*/

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

			//KERS test
			//kers_reg_w[KERS_W_BOOST] = 1;
			kers_reg_w[KERS_W_BOOST] = steering_reg[STEERING_KRES_START];
			kers_reg_w[KERS_W_STORE] = steering_reg[STEERING_KRES_STOP];
			
			//WIPER
			light_reg[WSVIPER_STATE] = steering_reg[STEERING_WIPER];
			
			
			//BAG lys
			light_B_reg[BACK_LIGHT_STATE]  = steering_reg[STEERING_LIGHT];
			light_B_reg[STARTER_LIGHT_STATE]  = steering_reg[STEERING_START_MOTOR];
			light_B_reg[STOP_LIGHT_STATE] = ( (PINF & (1<<1))) ? 1 : 0;//TODO see if it works PINF1 is the brake.
		
			//TODO Gear
			if(steering_reg[STEERING_GEAR12]){
				//SetPWMDutyGearMicro(1500);
				//SetPWMDutyGear(1249); //2499 = 100%
			}
			else{
				//SetPWMDutyGearMicro(1000);
				//SetPWMDutyGear(250);
			}
			
			//testing brake light purpose.
			if (PINF & (1<<1)){
				LED2On();
			}
			else{
				LED2Off();
			}
		
			//HORN
			if(steering_reg[STEERING_HORN]){
				SETBIT(PORTA, 2); //Horn out.
				LED3On();
			}
			else{
				CLRBIT(PORTA, 2);
				LED3Off();
			}
			
			
			//TODO Starter
			/*
			if(steering_reg[STEERING_START_MOTOR]){
				SETBIT(PORTA, 4);//starter
			}
			else{
				CLRBIT(PORTA, 4);
			}
			*/
			
			
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
			
			_delay_ms(2);
			
			timer0_interupt_flag = 0;
		}
		
		//Rio
		if(rs232_interupt_flag==1){
			rs232_interupt_flag = 0;
		}
		
		steering_reg_w[STEERING_KPH] = rio_rx[RIO_REC_KPH];
		steering_reg_w[STEERING_RPM_H] = rio_rx[RIO_REC_RPM_H];
		steering_reg_w[STEERING_RPM_L] = rio_rx[RIO_REC_RPM_L];
		steering_reg_w[STEERING_VAND_T] = rio_rx[RIO_REC_WATER_TEMP];
		steering_reg_w[STEERING_OLIE_T] = rio_rx[RIO_REC_OIL_TEMP];
		steering_reg_w[STEERING_GEAR] = rio_rx[RIO_REC_GEAR]; //was 0 in old code
		steering_reg_w[STEERING_ENCODER_FAN] = 0;
		steering_reg_w[STEERING_CAP_VOLTAGE] = 0; //TODO need to know what this is.
		
		//TODO battery voltage
		uint16_t volt = 0;
		volt = readBatterySensor();
		volt= (volt*500)/0xFF;//volt should be 0 to 500 now. 500 = 5V.
		
		//Gear servo
		uint16_t gearServo = rio_rx[RIO_REC_SERVO_GEAR_L] | (rio_rx[RIO_REC_SERVO_GEAR_H] << 8);
		SetPWMDutyGearMicro(gearServo);
		uint16_t speedServo = rio_rx[RIO_REC_SERVO_SPLAELD_L] | (rio_rx[RIO_REC_SERVO_SPLAELD_H] << 8);
		SetPWMDutySpeedMicro(speedServo);
		//TODO other servo. rio_rx[RIO_REC_SERVO_SPLAELD]
		//TODO starter rio_rx[RIO_REC_STARTER], line 141 should be deleted if this is used i believe.
		
		if(rio_rx[RIO_REC_STARTER]){
			SETBIT(PORTA,4);
		}
		else{
			CLRBIT(PORTA, 4);
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

void freqTest(void){
	while(1){
		LED1Toggle();
		_delay_ms(1000);
	}
}

void test_rs485(void){
		light_reg[FRONT_LIGHT_STATE] = 0x01;
		light_reg[TURNSIG_RIGHT_STATE] = 0x01;
		light_reg[TURNSIG_LEFT_STATE] = 0x01;
		light_reg[LED1_STATE] = 0x01;
		light_reg[LED2_STATE] = 0x01;
		LED1On();
		LED2On();
		
		//And finally the program.
		while(1){
			//skal bare blinke med alle lys
			rs485_send_packet(ID_FRONT_LIGHT, OP_WRITE, 0, 0, FRONT_LIGHT_REGSIZE);
			
			light_reg[FRONT_LIGHT_STATE] ^= 0x01;
			light_reg[TURNSIG_RIGHT_STATE] ^= 0x01;
			light_reg[TURNSIG_LEFT_STATE] ^= 0x01;
			light_reg[LED1_STATE] ^= 0x01;
			light_reg[LED2_STATE] ^= 0x01;
			LED1Toggle();
			
			_delay_ms(2000);
		}
}

void test2_rs485(void){
	light_B_reg[BACK_LIGHT_STATE] = 0x01;
	light_B_reg[STOP_LIGHT_STATE] = 0x01;
	light_B_reg[STARTER_LIGHT_STATE] = 0x01;
	light_B_reg[TURNSIG_B_RIGHT_STATE] = 0x01;
	light_B_reg[TURNSIG_B_LEFT_STATE] = 0x01;
	LED1On();
	
	//And finally the program.
	while(1){
		//skal bare blinke med alle lys
		rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
		
		light_B_reg[BACK_LIGHT_STATE] ^= 0x01;
		light_B_reg[STOP_LIGHT_STATE] ^= 0x01;
		light_B_reg[STARTER_LIGHT_STATE] ^= 0x01;
		light_B_reg[TURNSIG_B_RIGHT_STATE] ^= 0x01;
		light_B_reg[TURNSIG_B_LEFT_STATE] ^= 0x01;
		LED1Toggle();
		
		_delay_ms(200);
	}
}

void test3_rs485(void){
	
	//And finally the program.
	while(1){
		
		for(int i = 0; i < BACK_LIGHT_REGSIZE; i++){
			light_B_reg[i] = 0x01;
		}
		/*
		light_B_reg[BACK_LIGHT_STATE] = 0x01;
		light_B_reg[STOP_LIGHT_STATE] = 0x01;
		light_B_reg[STARTER_LIGHT_STATE] = 0x01;
		light_B_reg[TURNSIG_B_RIGHT_STATE] = 0x01;
		light_B_reg[TURNSIG_B_LEFT_STATE] = 0x01;
		*/
	
		rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
		
		
		for(int i = 0; i < BACK_LIGHT_REGSIZE; i++){
			light_B_reg[i] = 0x00;
		}
		
		
		_delay_ms(100);
		LED1On();
		//skal bare blinke med alle lys
//	while(1){
		rs485_send_packet(ID_REAR_LIGHT, OP_READ, 0, 0, BACK_LIGHT_REGSIZE);
	
		rs485_send_packet(ID_REAR_LIGHT, OP_WRITE, 0, 0, BACK_LIGHT_REGSIZE);
		
		if(light_B_reg[BACK_LIGHT_STATE]){
			LED2On();
		}
		else{
			LED2Off();
		}
		if(light_B_reg[STOP_LIGHT_STATE]){
			LED3On();
		}
		else{
			LED3Off();
		}
		if(light_B_reg[STARTER_LIGHT_STATE]){
			LED4On();
		}
		else{
			LED4Off();
		}
		if(light_B_reg[TURNSIG_B_RIGHT_STATE]){
			LED5On();
		}
		else{
			LED5Off();
		}
		if(light_B_reg[TURNSIG_B_LEFT_STATE]){
			LED6On();
		}
		else{
			LED6Off();
		}
		
		_delay_ms(100);
		LED1Off();
		LED2Off();
		LED3Off();
		LED4Off();
		LED5Off();
		LED6Off();
		
	}
}

void test4_rs485(void){
	
	while(1){
		//ask steering wheel.
		rs485_send_packet(ID_STEERING_WHEEL, OP_READ, 0, 0, STEERING_REGSIZE);
		
		if(steering_reg[STEERING_BLINK_L]){
			LED1On();
		}
		else{
			LED1Off();
		}
		if(steering_reg[STEERING_BLINK_H]){
			LED2On();
		}
		else{
			LED2Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED3On();
		}
		else{
			LED3Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED4On();
		}
		else{
			LED4Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED5On();
		}
		else{
			LED5Off();
		}
		if(steering_reg[STEERING_HORN]){
			LED6On();
		}
		else{
			LED6Off();
		}
	}
	
}


void dutyCycleTest(void){
//	uint16_t i1 = (7*2499)/100; //2499 = 100%
//	uint32_t i2 = (50*2499)/100;
//	uint16_t i3 = i2;
//	int i = 2400;
	while(1){		
		SetPWMDutyGear(1249);
		_delay_ms(3000);
		
		SetPWMDutyGear(833);
		_delay_ms(3000);
	}
	
}

void LED1Blink(int times);
void LED1Blink(int times) {
	LED1Off();
	_delay_ms(500);
	for(int i=0; i<times; i++) {
		LED1On();
		_delay_ms(200);
		LED1Off();
		_delay_ms(200);
	}
}

void batteryTest(){
	readBatterySensor();
	_delay_ms(2000);
	uint32_t volt = 0;
	volt = readBatterySensor();
	int blink = (volt*15)/0xFF;
	LED1Blink(blink);
	while(1){
	}
}


void uartreceiveTest(void){ //same as above, just with interrupts
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	cli();
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1);//|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,

	
	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	
	unsigned char data;
	while(1){
		while( !(UCSR1A & (1<<RXC1)) );
		LED1On();
		data = UDR1;
		if(data){
			LED2On();
		}
	}
}

void uartreceiveTest2(void){ //same as above, just with interrupts
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	/*
	cli();
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1);//|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,

	
	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	*/
	
	CLRBIT(PORTA, DDA3);
	unsigned char data;
	while(1){
		data = rs485_receive();
		switch (data){
			case 1:
				LED1On();
				break;
			case 2:
				LED2On();
				break;
			case 3:
				LED3On();
				break;
			case 4:
				LED4On();
				break;
			case 5:
				LED5On();
				break;
			case 6:
				LED6On();
				break;
			case 7:
				LED1Off();
				break;
			case 8:
				LED2Off();
				break;
			case 9:
				LED3Off();
				break;
			case 10:
				LED4Off();
				break;
			case 11:
				LED5Off();
				break;
			case 12:
				LED6Off();
				break;
		}
	}
}

void uartreceiveTestInterrupt(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	cli();
	
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1)|(1<<RXCIE1);// Enable Receiver/ adn interupt when receiving,

	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	while(1);
}

void uartSendTest(void){
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	/*
	cli();
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<TXEN1);// Enable Transmit

	UCSR1C = (3<<UCSZ10)| (1<<USBS1);
	
	sei();
	*/
	
	SETBIT(PORTA, DDA3); //set to send data.
	unsigned char data = 0x00;
	while(1){
		data = (data % 4) + 1;
		
		rs485_transmit(data);
		/*
		while ( !( UCSR1A & (1<<UDRE1)) );
		
		UDR1 = data;
		*/
		_delay_ms(1);
	}
}

void testLED(void){
	LED1On();
	LED2On();
	LED3On();
	LED4On();
	LED5On();
	LED6On();
	
	while(1){
		_delay_ms(500);
		LED1Toggle();
		LED2Toggle();
		LED3Toggle();
		LED4Toggle();
		LED5Toggle();
		LED6Toggle();
	}
}

void testDataValidation1(void){
	uint8_t dataLength = 7;
	uint8_t eLength = calcExstraSpace(dataLength);
	uint8_t fullLength = dataLength + eLength;
	uint8_t testArr[9] = {0x00};
	
	testArr[0] = 0x03;
	testArr[1] = 0x31;
	testArr[2] = 0x42;
	testArr[3] = 0xF2;
	testArr[4] = 0x9A;
	testArr[5] = 0x17;
	testArr[6] = 0xCC;
//	testArr[7] = 0x10;
	
	addCheckValue(testArr, fullLength, eLength);
	
	if(testArr[7] == 0b11000011){
		LED6On();
	}
	if(testArr[8] == 0b01010000){
		LED5On();
	}
	if(eLength == 2){
		LED4On();
	}
	
	uint8_t check1 = validateCheckValue(testArr, fullLength, eLength);
	
	testArr[8] = 0b01010001;
	
	uint8_t check2 = validateCheckValue(testArr, fullLength, eLength);
	
	if(check1){
		LED3On();
	}
	
	if(!check2){
		LED2On();
	}
	
	while(1){
		
	}
}

void testDataValidation2(void){
//	uint8_t dataLength = 7;
//	uint8_t eLength = calcExstraSpace(dataLength);
//	uint8_t fullLength = dataLength + eLength;
	uint8_t testArr1[2] = {0x00};
	uint8_t testArr2[5] = {0x00};
	uint8_t testArr3[2] = {0x00};
	
	testArr1[0] = 0x03;
	testArr1[1] = 0x31;
	testArr2[0] = 0x42;
	testArr2[1] = 0xF2;
	testArr2[2] = 0x9A;
	testArr2[3] = 0x17;
	testArr2[4] = 0xCC;
	
	addCheckValue2(testArr1, 2, testArr2, 5, testArr3, 0);
	
	if(testArr3[0] == 0b11000011){
		LED6On();
	}
	if(testArr3[1] == 0b01010000){
		LED5On();
	}
	
	/*
	if(eLength == 2){
		LED4On();
	}
	
	uint8_t check1 = validateCheckValue(testArr, fullLength, eLength);
	
	testArr[8] = 0b01010001;
	
	uint8_t check2 = validateCheckValue(testArr, fullLength, eLength);
	
	if(check1){
		LED3On();
	}
	
	if(!check2){
		LED2On();
	}
	*/
	while(1){
		
	}
}

ISR(USART1_RX_vect)
{
	
	LED1On();
	unsigned char data = UDR1;
	if(data){
		LED2On();
	}
}

