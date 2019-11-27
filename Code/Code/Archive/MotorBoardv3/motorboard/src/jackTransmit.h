/*
* jackTransmit.h
*
* Created: 26/04/2016 20:46:21
*  Author: Henning
*/


#ifndef JACKTRANSMIT_H_
#define JACKTRANSMIT_H_

void jack_init(void);
void jackTransfer(uint16_t RioLambda, uint16_t RioKmperl);

#include "asf.h"

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
unsigned short tempjackUpdateData = 0;
short freqTable[19] = {60,66,70,75,80,90,100,110,121,132,144,158,174,197,224,264,330,450,600};

void jack_init(void){
	DDRE|=(1<<PE4);
	ICR3 = freqTable[18]; //middlebit
	TCCR3A |= (1 << COM3B0);
	TCCR3B |= (1 << CS31) | (1 << WGM33)| (1 << WGM32);
}

void jackTransfer(uint16_t RioLambda, uint16_t RioKmperl){
	if(jackUpdateFlag){
		jackUpdateFlag = 0;
		switch(jackUpdateDataState){
			case 1:
			{
				switch(jackUpdateDataID){ //Read data to temp
					case 8:
					tempjackUpdateData = RioKmperl;
					break;
					case 6:
					tempjackUpdateData = RioLambda;
					break;
					default:
					tempjackUpdateData = 12345;
					break;
				}
				
				if(tempjackUpdateData == 0) tempjackUpdateData = 1;	//Check if zero
				ICR3 = freqTable[0];								//Start Bit//Start bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 2:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 3:
			{
				ICR3 = freqTable[(jackUpdateDataID & 0x0F) + 1];	//transfer ID Low
				jackUpdateDataState++; //Next step
			}
			break;
			case 4:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 5:
			{
				ICR3 = freqTable[(tempjackUpdateData >> 12) + 1];	//transfer 1st High
				jackUpdateDataState++; //Next step
			}
			break;
			case 6:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 7:
			{
				ICR3 = freqTable[((tempjackUpdateData >> 8) & 0x0F) + 1]; //transfer 1st Low
				jackUpdateDataState++; //Next step
			}
			break;
			case 8:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 9:
			{
				ICR3 = freqTable[((tempjackUpdateData >> 4) & 0x0F) + 1]; //transfer 2nd High
				jackUpdateDataState++; //Next step
			}
			break;
			case 10:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 11:
			{
				ICR3 = freqTable[(tempjackUpdateData & 0x0F) + 1];	//transfer 2nd Low
				jackUpdateDataState++; //Next step
			}
			break;
			case 12:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 13:
			{
				unsigned char LRC = 0;
				for(unsigned char i = tempjackUpdateData; i != 0; i = (i>>1)) LRC += (i & 0x01); //Calculate Checksum
				ICR3 = freqTable[(LRC & 0x0F) + 1];				//transfer CheckSum
				LED2Off();
				LED5Off();
				jackUpdateDataState++; //Next step
			}
			break;
			case 14:
			{
				ICR3 = freqTable[18];	//middle bit
				jackUpdateDataState++; //Next step
			}
			break;
			case 15:
			{
				ICR3 = freqTable[17];		//Stop Bit
				jackUpdateDataState = 1;
				if(jackUpdateDataID >= 8){	//Set state for next
					jackUpdateDataID = 1;
					}else{
					jackUpdateDataID++;
				}
			}
			break;
			default:
			{
				jackUpdateDataState = 1;
				ICR3 = freqTable[18];//Stop Bit
			}
			break;
		}
	}
}


#endif /* JACKTRANSMIT_H_ */