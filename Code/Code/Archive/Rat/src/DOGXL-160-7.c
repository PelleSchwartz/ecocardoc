#include "header/DOGXL-160-7.h"
#include "header/init.h"
#include "global.h"
#include <string.h>
#include <util/delay.h>
#include <header/BIG_NUMBERS.h>
#include <header/LOGOS.h>




void HalLcd_Init(void);
void LCDSendCmd(unsigned char* SCmd, unsigned char numBytes);
void LCDSendData(unsigned char* SData, unsigned char numBytes);
void LCDSetPageAddress(unsigned char PA);
void LCDSetColumnAdress(unsigned char CA);
void LCDWrite(unsigned char row, unsigned char col, unsigned char width, unsigned char height, unsigned short fstart, const unsigned char* bitmap);
void LCDWritePicture(unsigned char y, unsigned char x, unsigned char picture);
void LCDClean(void);

//DogXL160 Initialization Commands
unsigned char LcdInitMacro[] = {
	Set_Com_End_H,
	Set_Com_End_L,
	Set_LCD_Mapping_Control,
	Set_Scroll_Line_LSB,
	Set_Scroll_Line_MSB,
	Set_Panel_Loading,
	Set_LCD_Bias_Ratio,
	Set_Vbias_Potentiometer_H,
	Set_Vbias_Potentiometer_L,
	Set_RAM_Address_Control,
	Set_Display_Enable
};


/*
FUNCTION:		LCDInit

DESCRIPTION:	Initilize LCD Service 

PARAMETERS:		init - pointer to void that contains the initialized value

RETURN:			None
*/
void LCDInit(void)
{
	RESET_OFF;
	LCDSendCmd(LcdInitMacro, 11);
}


/*
FUNCTION:		LCDSendCmd

DESCRIPTION:	Sends commands to LCD via 3 wire SPI

PARAMETERS:		unsigned char* SCmd[] - Pointer to the commands to be written to the LCD
				unsigned char numBytes - Number of the commands  to be written to the LCD

RETURN:			None
*/
void LCDSendCmd(unsigned char* SCmd, unsigned char numBytes)
{
	CD_HIGH;
	unsigned char i;
	for(i=0;i<numBytes;i++)
	{
		CD_LOW;
		SPDR = SCmd[i];
		while(!(SPSR & (1<<SPIF)));		//Skal den sidde før?
		CD_HIGH;
	}		
}


/*
FUNCTION:		LCDSendData

DESCRIPTION:	Sends data to LCD via 3 wire SPI

PARAMETERS:		unsigned char* SData - Pointer to the Data to be written to the LCD
				unsigned char numBytes - Number of Datas  to be written to the LCD

RETURN:			None
*/
void LCDSendData(unsigned char* SData, unsigned char numBytes)
{
	CD_LOW;
	unsigned char i;
	for(i=0;i<numBytes;i++)
	{
		CD_HIGH;
		SPDR = SData[i];
		while(!(SPSR & (1<<SPIF)));		//TX buffer ready?
		CD_LOW;
	}	
}


/*
FUNCTION:		LCDAllPixelsOn

DESCRIPTION:	Sets all Pixels on

PARAMETERS:		None

RETURN:			None
*/
void LCDAllPixelsOn(void)
{
	unsigned char cmd[] = {Set_All_Pixels_On};
	LCDSendCmd(cmd, 1);
}


/*
FUNCTION:		LCDAllPixelsOff

DESCRIPTION:	Sets all Pixels off

PARAMETERS:		None

RETURN:			None
*/
void LCDAllPixelsOff(void)
{
unsigned char cmd[] = {Set_All_Pixels_Off};
LCDSendCmd(cmd, 1);
}


/*
FUNCTION:		LCDSetPageAddress

DESCRIPTION:	Sets Page Address of the LCD RAM memory

PARAMETERS:		unsigned char PA - Page Address of the LCD RAM memory to be written

RETURN:			None
*/
void LCDSetPageAddress(unsigned char PA)
{
  unsigned char Cmd[] = {Page_Address + PA};  //Page Adress Command = Page Address Initial Command + Page Address
  LCDSendCmd(Cmd, 1);
}


/*
FUNCTION:		LCDSetColumnAdress

DESCRIPTION:	Sets Column Address of the LCD RAM memory

PARAMETERS:		unsigned char CA - Column Address of the LCD RAM memory to be written

RETURN:			None
*/
void LCDSetColumnAdress(unsigned char CA)
{
  unsigned char H = 0x00;
  unsigned char L = 0x00;
  unsigned char ColumnAddress[]  = {Column_LSB0 , Column_MSB0 };
  L = (CA & 0x0F);                // Seperate Command Adress to low and high
  H = (CA & 0xF0);
  H = (H >> 4);
  ColumnAddress[0] = (Column_LSB0 + L );    //Column Adress CommandLSB = Column Address Initial Command
                                            // + Column Address bits 0..3 */
  ColumnAddress[1] = (Column_MSB0 + H );    //Column Adress CommandMSB = Column Address Initial Command
                                            // + Column Address bits 4..7
  LCDSendCmd(ColumnAddress, 2);
}


/*
FUNCTION:		LCDClean

DESCRIPTION:	Sets All pixels Off

PARAMETERS:		None

RETURN:			None
*/
void LCDClean(void)
{
  unsigned char LcdData[] = {0x00};
 
  for(unsigned char p=0; p<26; p++){                  /* 25  Pages */
    for(unsigned char c=0; c<160; c++){               /* 160 Columns */
      LCDSetPageAddress(0x00 + p);
      LCDSetColumnAdress(0x00 + c);       
      LCDSendData(LcdData, 1);
    }
  }
}


/**************************************************************************************************
 * @fn      LCDWrite
 *
 * @brief   Writes a font from font5x7int[] Array to the LCD
 *
 * @param   uint8 row -   Page Address
 *          uint8 col -   Column Address
 *          uint16 f  -   Pointer to the font to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void LCDWrite(unsigned char row, unsigned char col, unsigned char width, unsigned char height, unsigned short fstart, const unsigned char* bitmap)
{                                           /* Each Font consists of 5 Columns of 2 Pages */
	/* Each Page presents 4 pixels */
	unsigned char LcdData[2];                       /* Pointer to the both pages of a column  */
	unsigned char* ptr;
	unsigned char H = 0x00;
	unsigned char L = 0x00;
	
	for(unsigned char c=0; c<height; c++)
	{
		for(unsigned char r=0; r<width; r++)
		{             /* Font Block Size 5 Columns */
			
			L = (bitmap[fstart + r + c*width] & 0x0F);       /* LSB 4 Pixels are saved to L */
			H = (bitmap[fstart + r + c*width] & 0xF0);       /* MSB 4 Pixels are saved to H */
			H = (H >> 4);
			LcdData[0] = 0x00;
			LcdData[1] = 0x00;
			if((H & 0x01) == 0x01)
			{               /* Each Pixel in the H is converted to 2 Bits, 11 Pixel On - 00 Pixel Off    */
				LcdData[0] = LcdData[0] + 0xC0;
			};

			if((H & 0x02) == 0x02)
			{
				LcdData[0] = LcdData[0] + 0x30;
			};

			if((H & 0x04) == 0x04)
			{
				LcdData[0] = LcdData[0] + 0x0C;
			};


			if((H & 0x08) == 0x08)
			{
				LcdData[0] = LcdData[0] + 0x03;
			};


			if((L & 0x01) == 0x01)
			{                /* Each Pixel in the L is converted to 2 Bits,  11 Pixel On - 00 Pixel Off  */
				LcdData[1] = LcdData[1] + 0xC0;
			};
			
			if((L & 0x02) == 0x02)
			{
				LcdData[1] = LcdData[1] + 0x30;
			};
			
			if((L & 0x04) == 0x04)
			{
				LcdData[1] = LcdData[1] + 0x0C;
			};
			
			
			if((L & 0x08) == 0x08)
			{
				LcdData[1] = LcdData[1] + 0x03;
			};
			
			for(unsigned char p=0; p<2; p++)
			{                   /* Font Page Size 2 Pages */
				ptr = LcdData+p;
				LCDSetPageAddress(0x00 + p + row + r*2);
				LCDSetColumnAdress(0x00 + c + col);
				LCDSendData(ptr, 1);                  /* A Column that consists of two Pages is sent to the LCD  */
			};
			//_delay_ms(500);
		};
		
	};
};



void LCDWriteString (unsigned char y, unsigned char x, char word[], unsigned char size)
{
	unsigned char k;
	unsigned char a = 0;
	const unsigned char* bitmap;
	const unsigned short* descriptors;
	if(size==1)
	{
		bitmap = SmallFontBitmaps;
		descriptors = SmallFontDescriptors;
	}
	else if(size==2)
	{
		bitmap = MediumFontBitmaps;
		descriptors = MediumFontDescriptors;
	}
	else if(size==3)
	{
		bitmap = BigNumbersBitmaps;
		descriptors = BigNumbersDescriptors;
	} else {	// Default if invalid size
		bitmap = SmallFontBitmaps;
		descriptors = SmallFontDescriptors;
	}

	do
	{                     
		k = word[a] - descriptors[0];
		LCDWrite(y, x, descriptors[(k+1)*3-1], descriptors[(k+1)*3], descriptors[(k+1)*3+1],bitmap);
		y = y + descriptors[(k+1)*3-1]*2;             
		a++;
		k = word[a];
	}while(k!=0);           
}


void LCDWritePicture(unsigned char y, unsigned char x, unsigned char picture)
{
	const unsigned char* bitmap;
	const unsigned short* descriptors;
	if(picture==1)
	{
		bitmap = DTULogoBitmaps;
		descriptors = DTULogoDescriptors;
	}
	else if(picture==2)
	{
		bitmap = RoadLogoBitmaps;
		descriptors = RoadLogoDescriptors;
	}
	else if(picture==3)
	{
		bitmap = WaterTempBitmaps;
		descriptors = WaterTempDescriptors;
	}
	else if(picture==4)
	{
		bitmap = OilTempBitmaps;
		descriptors = OilTempDescriptors;
	}
	else if(picture==5)
	{
		bitmap = TimerBitmaps;
		descriptors = TimerDescriptors;
	} else {	// Invalid picture ID
		return;
	}

	LCDWrite(y, x, descriptors[0], descriptors[1], descriptors[2],bitmap);	
}

void LCDIntro(void)
{
	LCDColor(0);
	LCDWritePicture(0,5,1);
	_delay_ms(2000);
	LCDClean();
	LCDInit();
}

//Her skal der laves noget PWM. Det virker ikke rigtigt som det skal!!!
void LCDColor(unsigned char color)
{
	if(color==0) //Rød
	{
		PORTD |= (1<<PORTD6);
		PORTD &= ~((1<<PORTD4) | (1<<PORTD5));
	}
	else if(color==1)//Grøn
	{
		PORTD |= (1<<PORTD5);
		PORTD &= ~((1<<PORTD4) | (1<<PORTD6));
	}
	else if(color==2)//Blå
	{
		PORTD |= (1<<PORTD4);
		PORTD &= ~((1<<PORTD5) | (1<<PORTD6));
	}
	else if(color==3)
	{
		PORTD |=  (1<<PORTD4)| (1<<PORTD5) | (1<<PORTD6);
	}
}


/*
FUNCTION:		SpeedoPage

DESCRIPTION:	Updates the speedometer page

PARAMETERS:		unsigned char speed - speed to be written to display
				unsigned char oil_temp - oil_temp to be written to display
				unsigned char water_temp - water_temp to be written to display

RETURN:			None
*/
	
void SpeedoPage3(unsigned char speed, unsigned char cLoop, unsigned char TomGang, unsigned short rpm, unsigned char minf, unsigned char secf, unsigned char lap_min, unsigned char lap_sec, unsigned char water_temp, unsigned char oil_temp, unsigned char gear,unsigned char Kers,uint16_t Volt,unsigned char blinkv,unsigned char blinkh)
{
	char speed_str[3]={0};
	char water_temp_str[4]={0};
	char oil_temp_str[4]={0};
	char timer_str[6]={0};
	char rpm_str[5] ={0};
	char KersVolt_str[5];
	
	//Temperatures
	LCDWritePicture(0,5,3);
	//Water temperature
	sprintf(water_temp_str, "%2i", water_temp);
	LCDWriteString(3,0,water_temp_str,1);
	//Oil temperature
	sprintf(oil_temp_str, "%2i", oil_temp);
	LCDWriteString(3,14,oil_temp_str,1);
	
	//Timers
	LCDWritePicture(12,5,5);
	//Total time
	sprintf(timer_str, "%02i:%02i", minf,secf);
	LCDWriteString(16,0,timer_str,1);
	//Lap time
	sprintf(timer_str, "%02i:%02i", lap_min,lap_sec);
	LCDWriteString(16,14,timer_str,1);
	
	//Speed
	sprintf(speed_str, "%2i", speed);
	LCDWriteString(2,40,speed_str,3);
	
	//RPM
	sprintf(rpm_str, "%4i", rpm);
	LCDWriteString(5,107,rpm_str,2);
	
	if(gear == 1)
	{
		LCDWriteString(7,146,"1",1);
	}
	else if(gear == 2)
	{
		LCDWriteString(7,146,"2",1);
	}
	else if(gear == 3)
	{
		LCDWriteString(7,146,"N",1);
	}
	else if(gear == 4)
	{
		LCDWriteString(7,146,"A",1);
	}
	LCDWriteString(10,146,"gear",1);

    //KRES
// 	if(Kers==1){
// 		LCDWriteString(16,146,"ST",1);
// 	}else if(Kers==2){
// 		LCDWriteString(16,146,"BR",1);
// 	}else{
// 		LCDWriteString(16,146,"  ",1);
// 	}

    //KRES
	//Voltage
//	double Fvolt = (240.0 * 5.0 * 3.0) / 255.0 ; //kersvolt
//	uint32_t Ivolt = (KersVolt * 5l * 3l * 10l) >> 8 ;
	uint32_t Ivolt = (Volt<1000l) ? Volt : 999l;
//	sprintf(KersVolt_str, "%02.1f", Fvolt);
	sprintf(KersVolt_str, "%3ld", Ivolt);
	
	LCDWriteString(15,26,"dV",1);
	LCDWriteString(8,26,KersVolt_str,1);
	//LCDWriteString(10,26,AnotherString,1);
	
	
	if(cLoop){
		LCDWriteString(0,146,"CL",1);
	}
	else{
		LCDWriteString(0,146,"  ",1);
	}
	
	if(TomGang){
		LCDWriteString(21,146,"TG",1);
	}
	else{
		LCDWriteString(21,146,"  ",1);
	}
	
	/*
	//KERS
	if(Kers==1){
	   	LCDWriteString(3,130,"KERS START",1);
    }else if(Kers==2){
	 	LCDWriteString(3,130,"KERS BREAK",1);
    }else{
	 	LCDWriteString(3,130,"          ",1);
    } 	
	*/
	
	//Blink
	if(blinkh){
		LCDWriteString(22,25,"->",1);	
	}else{
		LCDWriteString(22,25,"  ",1);	
	}
	if(blinkv){
		LCDWriteString(0,25,"<-",1);	
	}else{
		LCDWriteString(0,25,"  ",1);
	}
		
}

/*
FUNCTION:		GearPage

DESCRIPTION:	Updates the gear page

PARAMETERS:		unsigned char gear - 1, 2 or N gear (1,2,3)

RETURN:			None
*/
void GearPage(unsigned char gear)
{
	LCDClean();
	if(gear == 1)
	{
		LCDWriteString(7,40,"1",3);
	}
	else if(gear == 2)
	{
		LCDWriteString(7,40,"2",3);
	}
	else if(gear == 3)
	{
		LCDWriteString(7,40,"N",3);
	}
	else if(gear == 4)
	{
		LCDWriteString(5,40,"AUTO",2);
	}
	LCDWriteString(5,107,"GEAR",2);
}


/*
FUNCTION:		PotPage

DESCRIPTION:	Updates the pot page

PARAMETERS:		unsigned char pot

RETURN:			None
*/
void PotPage(unsigned short pot)
{
	char pot_str[5];
	sprintf(pot_str, "%4i", pot);
	LCDWriteString(5,67,pot_str,2);
}



/*
FUNCTION:		LapTimePage

DESCRIPTION:	Updates the pot page

PARAMETERS:		unsigned char min
				unsigned char sec
				unsigned char func
							//1 push time
							//2
							//3
							//4 clear time
							//5 min
							//6 sec

RETURN:			min if func==5
				sec if func==6
*/
unsigned char LapTimePage(unsigned char minfu,unsigned char secfu,unsigned char func)
{
		//1 push time
		//2
		//3 write
		//4 clear time
		//5 min
		//6 sec
		static unsigned char numb = 0;
		static unsigned char lap_time_reg_min[15] = {0};
		static unsigned char lap_time_reg_sec[15] = {0};

		char timer_str[6] = {0};
		char number_str[6] = {0};
		unsigned char mincalc = 0;
		unsigned char seccalc = 0;
		uint16_t vidre = 0;


		//Push time
		if(func == 1){
			if(numb<16){
				if(numb > 0){
					mincalc  = minfu-lap_time_reg_min[numb-1];
	
					if(lap_time_reg_sec[numb-1]<=secfu){
						seccalc  = secfu-lap_time_reg_sec[numb-1];	
					}else{
						seccalc  = 60-lap_time_reg_sec[numb-1];	
					}						
					vidre = seccalc;
					if(vidre){ //(lap_time_reg_sec[numb-1] != minfu) && (lap_time_reg_min[numb-1] != secfu) && 
							lap_time_reg_min[numb] = minfu;
							lap_time_reg_sec[numb] = secfu;
							numb++;
					}						
				}else{
					lap_time_reg_min[numb] = minfu;
					lap_time_reg_sec[numb] = secfu;
					numb++;
					
				}	
									
			}
		}
		//clear
		if(func == 4){
			for(unsigned i=0; i<15;i++){
				lap_time_reg_min[i] = 0;
				lap_time_reg_sec[i] = 0;
			}
		}		
	
		if(func == 3){
			if(numb>0){
				
				sprintf(timer_str, "%02i:%02i", minfu-lap_time_reg_min[numb-1],secfu-lap_time_reg_sec[numb-1]);
			}else{
				sprintf(timer_str, "%02i:%02i", minfu,secfu);
			}
			
			LCDWriteString(5,0,timer_str,1);
			for(unsigned i=0; i<14;i++){
				if(i==0){
					sprintf(timer_str, "%02i:%02i", lap_time_reg_min[i],lap_time_reg_sec[i]);
				}else{
					/*
					mintemp = lap_time_reg_min[i-1]*60;
					sectemp = lap_time_reg_sec[i-1];
					timetemp = mintemp*60+sectemp;
					timetemp = (timetemp-((lap_time_reg_min[i-1]*60))+lap_time_reg_sec[i-1]);	
					mintemp = timetemp/60;
					sectemp = timetemp%60;
					sprintf(timer_str, "%02i:%02i",mintemp ,sectemp);	
					*/
					
					if(lap_time_reg_sec[i]<lap_time_reg_sec[i-1]){   //tjek om i > 0
						if((lap_time_reg_min[i]-lap_time_reg_min[i-1])>0){
							sprintf(timer_str, "%02i:%02i", lap_time_reg_min[i]-lap_time_reg_min[i-1]-1,60-lap_time_reg_sec[i]);
						}else{
							sprintf(timer_str, "%02i:%02i", 0,0);	
						}
						
					}else{
						sprintf(timer_str, "%02i:%02i", lap_time_reg_min[i]-lap_time_reg_min[i-1],lap_time_reg_sec[i]-lap_time_reg_sec[i-1]);
					}
					
				}
				
				sprintf(number_str, "%02i", i+1);
				LCDWriteString(5,10*i+10,number_str,1);
				LCDWriteString(10,10*i+10,timer_str,1);

			}	
		}		
			
		if(func == 5){
			return lap_time_reg_min[numb-1];
		}else if(func == 6){
			return lap_time_reg_sec[numb-1];
		}else{
			return 1;	
		}
		
}