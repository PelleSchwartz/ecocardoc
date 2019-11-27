/*
 * jackTransmit.h
 *
 * Created: 23-05-2015 01:03:12
 *  Author: Oliver
 */ 
void transferPackage(char ID, char data1, char data2);

void transferPackage(char ID, char data1, char data2) {
	char freqTable[18] = {33,35,37,40,42,45,48,52,56,61,67,75,84,96,112,134,168,224};

	unsigned char LRC = 0;
	unsigned char i = 0;
	
	if(data1==0&&data2==0)data2=1; //no zero
	
	for(i = data1; i != 0; i = (i>>1)) LRC += (i & 0x01); //LRC
	for(i = data2; i != 0; i = (i>>1)) LRC += (i & 0x01);//LRC
	
	OCR0A = freqTable[0]; //Start Bit
	_delay_ms(SEND_PERIOD);
	
	OCR0A = freqTable[ (ID & 0x0F) + 1]; //transfer ID Low
	_delay_ms(SEND_PERIOD);

	OCR0A = freqTable[(data1 >> 4) + 1]; //transfer 1st High
	_delay_ms(SEND_PERIOD);
	OCR0A = freqTable[ (data1 & 0x0F) + 1]; //transfer 1st Low
	_delay_ms(SEND_PERIOD);
	
	OCR0A = freqTable[(data2 >> 4) + 1]; //transfer 2nd High
	_delay_ms(SEND_PERIOD);
	OCR0A = freqTable[ (data2 & 0x0F) + 1]; //transfer 2nd Low
	_delay_ms(SEND_PERIOD);
	
	OCR0A = freqTable[ (LRC & 0x0F) + 1]; //transfer CheckSum Low
	_delay_ms(SEND_PERIOD);
	
	OCR0A = freqTable[17]; //Stop Bit
	_delay_ms(SEND_PERIOD);
}