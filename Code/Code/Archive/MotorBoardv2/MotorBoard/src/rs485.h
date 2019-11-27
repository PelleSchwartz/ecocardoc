/*
 * rs485.h
 *
 * Created: 23-03-2015 10:44:20
 *  Author: Kim
 *	Note: Mostly copied and modified from RS232.c from old master board.
 */ 


#ifndef RS485_H_
#define RS485_H_

#include "adc.h"
#include "DataValidation.h"

void initiateRS485(void);
void rs485_transmit(unsigned char data);
unsigned char rs485_receive(void);
void rs485_send_packet(uint8_t id, uint8_t operation, uint8_t local, uint8_t dest, uint8_t len);

void rs485_send_packet2(uint8_t id, uint8_t operation, uint8_t dest, uint8_t len);

void RS485ReadySend(void);
void RS485ReadyReci(void);

void initiateRS485(void) {
	// Make sure I/O clock to USART1 is enabled //taken from steering wheel
	PRR1 &= ~(1 << PRUSART1);
	
	DDRA |= (1 << DDA3); //datadir port set as an output.
	CLRBIT(PORTA, DDA3); //set to receive data.
	
	UBRR1H = (unsigned char)(RS485BAUD>>8);	// Set BAUD rate
	UBRR1L = (unsigned char)RS485BAUD;
	
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);			// Enable Transmitter/Receiver
	UCSR1C = (3<<UCSZ10);		// Set frame format: 8bit data, 1bit stop  (1<<USBS1)|
}

void rs485_transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Clear TXC flag */
	UCSR1A |= (1<<TXC1);
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

unsigned char rs485_receive(void) {
	while ( !(UCSR1A & (1<<RXC1)) ); // Wait for data to be received
	unsigned char temp = UDR1; //not realy needed.
	return temp;	// Return received data from buffer
}

void rs485_send_packet(uint8_t id, uint8_t operation, uint8_t local, uint8_t dest, uint8_t len) {
	unsigned char r = 0x00;
	unsigned char i, p[6];
	p[0] = 0xA5;
	p[1] = ((id & 0x7F) << 1) | (operation & 0x01);
	p[2] = dest;
	p[3] = len;
	
	/*************************
	** Send header
	*************************/
	SETBIT(UCSR1A, TXC1);	// Clear TXC flag if it is already set
	//while(TESTBIT(UCSR3A, RXC3)) rs485_receive(); //while(UCSR3A & ) rs485_receive();
	SETBIT(PORTA, PA3);
	for(i=0;i<4;i++) { 
		rs485_transmit(p[i]);
	}	
	
	while(!(UCSR1A & (1 << TXC1)));					// Wait for TX Complete Flag
	UCSR1A |= 1<<TXC1;								// Clear TX Complete Flag
	CLRBIT(PORTA, PA3);
	r = UDR1;
	
	/*************************
	** Handle specific task
	*************************/
	if(operation) { //Write task
		
		SETBIT(PORTA, PA3);
		
		for(i=0;i<len;i++){
				
			switch (id){
				case ID_FRONT_LIGHT:
				rs485_transmit(light_reg[local+i]);	
				break;
				
				case ID_REAR_LIGHT:
				rs485_transmit(light_B_reg[local+i]);
				break;
				
				case ID_STEERING_WHEEL:
				rs485_transmit(steering_reg_w[local+i]);	
				break;
				
				/*
				case ID_KERS:
				rs485_transmit(kers_reg_w[local+i]);
				break;
				*/
			}				
		} 
		while(!TESTBIT(UCSR1A, TXC1));
		UCSR1A |= 1<<TXC1;
		CLRBIT(PORTA, PA3);
		r = UDR1;
	}
	else { //Read task
		for(i=0;i<len;i++) {
			timeout = RX_TIMEOUT;
			while(!TESTBIT(UCSR1A, RXC1)){
				if(--timeout<2){
					//LED3On();
					//_delay_ms(500);
					//LED3Off();
					return;
					}
				}	// If timeout occur, ignore this client until next round
			switch (id){
				case ID_FRONT_LIGHT:
					light_reg[local+i] = rs485_receive();
				break;
				
				case ID_REAR_LIGHT:
					light_B_reg[local+i] = rs485_receive();
				break;
				
				case ID_STEERING_WHEEL:
					steering_reg[local+i] = rs485_receive();
				break;	
				/*						
				case ID_KERS:
					kers_reg_r[local+i] = rs485_receive();
				break;	
				*/			
			}
			
		}
		_delay_ms(1);
	}

}

//will get that local and dest removed later....
void rs485_send_packet2(uint8_t id, uint8_t operation, uint8_t dest, uint8_t len) {
	unsigned char r = 0x00;
	unsigned char i, p[4];
	p[0] = 0xA5;
	p[1] = ((id & 0x7F) << 1) | (operation & 0x01);
	p[2] = dest;
	p[3] = len;  
	
	unsigned char inTemp[22] = {0x00};
	unsigned char checkSum[3] = {0x00};
	
	/*************************
	** Send header
	*************************/
	SETBIT(UCSR1A, TXC1);	// Clear TXC flag if it is already set
	//while(TESTBIT(UCSR3A, RXC3)) rs485_receive(); //while(UCSR3A & ) rs485_receive();
	SETBIT(PORTA, PA3);
	for(i=0;i<4;i++) { 
		rs485_transmit(p[i]);
	}	
	if(!operation){//if read
		addCheckValue2(p, 4, NULL, 0, checkSum, 0);
		for(i=0;i<2;i++) {
			rs485_transmit(checkSum[i]);
		}
	}
	
	while(!(UCSR1A & (1 << TXC1)));					// Wait for TX Complete Flag
	UCSR1A |= 1<<TXC1;								// Clear TX Complete Flag
	CLRBIT(PORTA, PA3);
	r = UDR1;
	
	/*************************
	** Handle specific task
	*************************/
	if(operation) { //Write task
		
		SETBIT(PORTA, PA3);
		
		for(i=0;i<len;i++){	
			switch (id){
				case ID_FRONT_LIGHT:
				rs485_transmit(light_reg[i]);
				break;
				
				case ID_REAR_LIGHT:
				rs485_transmit(light_B_reg[i]);
				break;
				
				case ID_STEERING_WHEEL:
				rs485_transmit(steering_reg_w[i]);	
				break;
				
				/*
				case ID_KERS:
				rs485_transmit(kers_reg_w[i]);
				break;
				*/
			}				
		} 
		switch (id){
			case ID_FRONT_LIGHT:
			addCheckValue2(p, 4, light_reg, len, checkSum, 0);
			break;
			case ID_REAR_LIGHT:
			addCheckValue2(p, 4, light_B_reg, len, checkSum, 0);
			break;
			case ID_STEERING_WHEEL:
			addCheckValue2(p, 4, steering_reg_w, len, checkSum, 0);
			break;
			/*
			case ID_KERS:
			addCheckValue2(p, 4, kers_reg_w, len, checkSum, 0);
			break;
			*/
		}
		int temp = calcExstraSpace(len);
		for(int i = 0; i < temp; i++){
			rs485_transmit(checkSum[i]);
		}
		
		
		while(!TESTBIT(UCSR1A, TXC1));
		UCSR1A |= 1<<TXC1;
		CLRBIT(PORTA, PA3);
		r = UDR1;
	}
	else { //Read task
		int tot = len + calcExstraSpace(len);
		for(i=0;i<tot;i++) {
			timeout = RX_TIMEOUT;
			while(!TESTBIT(UCSR1A, RXC1)){
				if(--timeout<2){
					return;
					}
				}	// If timeout occur, ignore this client until next round
			
			inTemp[i] = rs485_receive();
			
		}
		_delay_ms(1);
		
		if(validateCheckValue(inTemp, len, (tot-len))){ //checks if checksum is correct.
			for(int i = 0; i < len; i++){
				switch (id){
					case ID_FRONT_LIGHT:
					light_reg[i] = inTemp[i];
					break;
					
					case ID_REAR_LIGHT:
					light_B_reg[i] = inTemp[i];
					break;
					
					case ID_STEERING_WHEEL:
					steering_reg[i] = inTemp[i];
					break;
					
					/*
					case ID_KERS:
					kers_reg_r[i] = inTemp[i];
					break;
					*/
				}
			}
		}
		
	}

}

void RS485ReadySend(void){
	//PORTA |= (1 << PA3);
	SETBIT(PORTA, PA3);
}
void RS485ReadyReci(void){
	CLRBIT(PORTA, PA3);
	//PORTA &= ~(1 << PA3);
}

#endif /* RS485_H_ */