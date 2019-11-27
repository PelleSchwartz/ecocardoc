/*
 * rs232.h
 *
 * Created: 04-04-2015 02:33:14
 *  Author: Kim
 */ 


#ifndef RS232_H_
#define RS232_H_

void rs232_sync(void);
void rs232_init(void);
unsigned char rs232_receive(void);
uint8_t rs232_EPOS_receive(void);
void rs232_transmit(unsigned char data);
void rs232_EPOS_transmit(unsigned char data);
unsigned char rs232_sync_f_rio(unsigned char datain);
void writeObject(uint8_t opcode, int16_t index, int8_t subindex, int32_t data, int16_t CRC);
void sendFrame(int16_t *frame);

void rs232_init(void) { //
	
	// RIO
	PRR1 &=~(1 << PRUSART2); //should not be necessary, is defeault at 1.

	UBRR2H = (unsigned char)(RS232BAUD>>8) & 0xff;	// Set BAUD rate
	UBRR2L = (unsigned char) RS232BAUD & 0xff;

	UCSR2A = (1<< TXC2) | (0 << U2X2); //0 shifted....
	 
	UCSR2B = (1<<RXEN2)|(1<<TXEN2)|(1<<RXCIE2);			// Enable Transmitter/Receiver interrupts

	UCSR2C = (1<<UCSZ20) |(1<<UCSZ21) |(0<<UCPOL2);
	
	/*
	UBRR1H = (unsigned char)(RS232BAUD>>8)&0xff;	// Set BAUD rate
	UBRR1L = (unsigned char)RS232BAUD&0xff;
	//UCSR1B = (1<<RXEN1)|(1<<TXEN1); //this is the difference.
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);			// Enable Transmitter/Receiver interrupts
	UCSR1C = (3<<UCSZ10);
	*/
	// EPOS2
	
	PRR1 &=~(1 << PRUSART3);

	//UBRR2H = (unsigned char)(RS232EPOS2BAUD>>8)&0xff;	// Set BAUD rate
	//UBRR2L = (unsigned char)RS232EPOS2BAUD&0xff; //this is calculated to 7.
	
	UBRR3L = 16;//why 16 instead of what calculated above???

	UCSR3A = (1<< TXC3) | (1 << U2X3); //maybe mistake u2x3 is not written to zero.
	
	//for some reason this RXCIE is not set to 1?
	UCSR3B = (1<<RXEN3)|(1<<TXEN3);			// Enable Transmitter/Receiver interrupts

	UCSR3C = (1<<UCSZ30) |(1<<UCSZ31) |(0<<UCPOL3);
	
}

void rs232_sync(void) {
	// Send then receive
	// 2 byte sync
	// 2 byte Length
	// 1 byte Type
	// x byte Data
	// 2 byte CRC
	uint16_t dataleng = RIO_REGSIZE + 7;
	rs232_transmit(0x55);									//sync
	rs232_transmit(0xAA);									//sync
	rs232_transmit((unsigned char)(dataleng>>8)&0xff);	//Length HI
	rs232_transmit((unsigned char)(dataleng)&0xff);	    //Length LO
	rs232_transmit(0x01);									//Type
	for(unsigned char i=0;i<RIO_REGSIZE;i++)
	{
		rs232_transmit(rio_reg[i]);							//data
	}
	rs232_transmit(0x00);
	rs232_transmit(0x00);
	
}


/*
I have no clue about this method.... /Kim
*/
unsigned char rs232_sync_f_rio(unsigned char datain){
	static volatile unsigned char receive_state = 0;
	static uint16_t sizehi=0;
	static uint16_t sizeofpac = 0;
	static uint16_t regcount = 0;
	
	switch (receive_state){
		case 0:
		if(datain==0x55){
			receive_state = 1;
			
		}
		break;
		
		case 1:
		if(datain==0xAA){
			receive_state = 2;
			
			}else{
			receive_state = 0;
		}
		break;
		
		case 2:
		sizehi = datain;
		receive_state = 3;
		
		break;
		
		case 3:
		sizeofpac = ((sizehi<<8) & 0xFF00) + (datain & 0xFF);
		//Test
		if(sizeofpac==0x0008){
			//SETBIT(PORTD,LED2_PIN_D);
		}
		receive_state = 4;
		break;
		//Function code
		case 4:
		
		if(datain==0x01){
			
			receive_state = 5;
			}else{
			receive_state = 5;	//--------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		
		break;
		
		case 5:
		if(regcount<(sizeofpac-8)){
			rio_temp_rx[regcount] = datain;
			regcount++;
			}else{
			if(regcount == (sizeofpac-8)){
				rio_temp_rx[regcount] = datain;
				receive_state = 6;
				regcount = 0;
				
				}else{
				receive_state = 0; //fejl
				regcount = 0;
			}
		}
		
		break;
		//CRC 0000
		case 6:
		if(datain == 0x00){
			receive_state = 7;
			}else{
			receive_state = 0; //fejl gå til start
		}
		break;

		case 7:
		//SETBIT(PORTD,LED2_PIN_D);
		if(datain == 0x00){
			for(unsigned char jj = 0; jj<(sizeofpac-7) ;jj++){
				rio_rx[jj] = rio_temp_rx[jj];
			}

		}
		
		receive_state = 0;
		
		break;

	}
	
	return 1;
}

void rs232_transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR2A & (1<<UDRE2)) );
	/* Clear TXC flag */
	UCSR2A |= (1<<TXC2);
	/* Put data into buffer, sends the data */
	UDR2 = data;
}

void rs232_EPOS_transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR3A & (1<<UDRE3)) );
	/* Clear TXC flag */
	UCSR3A |= (1<<TXC3);
	/* Put data into buffer, sends the data */
	UDR3 = data;
}

unsigned char rs232_receive(void) {
	while ( !(UCSR2A & (1<<RXC2)) ); // Wait for data to be received
	return UDR2;	// Return received data from buffer
}

uint8_t rs232_EPOS_receive(void) {
	while( !(UCSR3A & (1<<RXC3)) ); // Wait for data to be received
	return UDR3;	// Return received data from buffer
}

void sendFrame(int16_t *frame) {
	uint16_t i;
	uint8_t length = ((frame[0] & 0xFF00) >> 8) + 3;  // Frame length
	
	// CRC is hardcoded in *frame for now! DO NOT USE
	//frame[length-1] = computeCRC(frame, length);
	rs232_EPOS_transmit(frame[0] & 0x00FF); // Serial.write(frame[0] & 0x00FF);
	while(!(UCSR3A & (1<<RXC3))); rs232_EPOS_receive(); // while(Serial.available() <= 0); if(Serial.read() == 'O') digitalWrite(13, HIGH);
	rs232_EPOS_transmit((frame[0] & 0xFF00)>>8);	// Serial.write((frame[0] & 0xFF00) >> 8);
	for(i=1;i<length;i++) {
		rs232_EPOS_transmit(frame[i] & 0x00FF); //Serial.write(frame[i] & 0x00FF);
		rs232_EPOS_transmit((frame[i] & 0xFF00) >> 8); //Serial.write((frame[i] & 0xFF00) >> 8);
	}
	
}

void writeObject(uint8_t opcode, int16_t index, int8_t subindex, int32_t data, int16_t CRC) {
	int16_t req_frame[6]={0,0,0,0,0,0};

	req_frame[0] = 0x0300 | opcode;                   // Opcode & Length (Write)
	req_frame[1] = index;                             // Object Index
	req_frame[2] = ((0x0000 | 0x0001)<<8) | subindex; // Node ID & subindex
	req_frame[3] = data & 0x0000FFFF;                 // Data LSB
	req_frame[4] = data >> 16;                        // Data MSB
	req_frame[5] = CRC;                               // CRC

	sendFrame(req_frame);

	// Error check and response
	_delay_ms(5);
	//while(Serial.available() <= 0);
	while( !(UCSR3A & (1<<RXC3)));	// Wait for data to be received
	
	//if((Serial.read() != 'O') || (Serial.read() != 0x00)) digitalWrite(13, HIGH);
	
	//if((rs232_EPOS_receive() != 'O') || (rs232_EPOS_receive() != 0x00))
	
	// Send Acknowledge
	//Serial.write((byte)0x00);
	rs232_EPOS_transmit(0x00);
	//Serial.write('O');
	rs232_EPOS_transmit('O');
	_delay_ms(1);
	//Serial.write('O');
	rs232_EPOS_transmit('O');
	_delay_ms(5);
	// Clear buffer
	//while(Serial.available() > 0) Serial.read();
	rs232_EPOS_receive();
}

ISR(USART2_RX_vect){
	LED6On();
	
	//static unsigned char counter23 = 0;
	rs232_interupt_flag = 1;
	//rs232_data[0] = UDR1;
	rs232_sync_f_rio(UDR2);
}

ISR(USART3_RX_vect){
	
	// gør noget
}

#endif /* RS232_H_ */