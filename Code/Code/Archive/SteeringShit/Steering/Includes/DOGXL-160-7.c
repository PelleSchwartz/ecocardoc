
#include "DOGXL-160-7.h"
#include "global.h"
#include <string.h>
#include "BIG_NUMBERS.h"
#include "LOGOS.h"

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

DESCRIPTION:	Initialize LCD Service

PARAMETERS:		init - pointer to void that contains the initialized value

RETURN:			None
*/
void LCDInit(void) {
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
void LCDSendCmd(unsigned char* SCmd, unsigned char numBytes) {
	CD_HIGH;
	unsigned char i;
	for(i=0;i<numBytes;i++)	{
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
void LCDSendData(unsigned char* SData, unsigned char numBytes) {
	CD_LOW;
	unsigned char i;
	for(i=0;i<numBytes;i++)	{
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
void LCDAllPixelsOn(void) {
	unsigned char cmd[] = {Set_All_Pixels_On};
	LCDSendCmd(cmd, 1);
}


/*
FUNCTION:		LCDAllPixelsOff

DESCRIPTION:	Sets all Pixels off

PARAMETERS:		None

RETURN:			None
*/
void LCDAllPixelsOff(void) {
	unsigned char cmd[] = {Set_All_Pixels_Off};
	LCDSendCmd(cmd, 1);
}


/*
FUNCTION:		LCDSetPageAddress

DESCRIPTION:	Sets Page Address of the LCD RAM memory

PARAMETERS:		unsigned char PA - Page Address of the LCD RAM memory to be written

RETURN:			None
*/
void LCDSetPageAddress(unsigned char PA) {
	unsigned char Cmd[] = {Page_Address + PA};  //Page Address Command = Page Address Initial Command + Page Address
	LCDSendCmd(Cmd, 1);
}


/*
FUNCTION:		LCDSetColumnAdress

DESCRIPTION:	Sets Column Address of the LCD RAM memory

PARAMETERS:		unsigned char CA - Column Address of the LCD RAM memory to be written

RETURN:			None
*/
void LCDSetColumnAdress(unsigned char CA) {
	unsigned char H = 0x00;
	unsigned char L = 0x00;
	unsigned char ColumnAddress[]  = {Column_LSB0 , Column_MSB0};
	L = (CA & 0x0F);                // Separate Command Address to low and high
	H = (CA & 0xF0);
	H = (H >> 4);
	ColumnAddress[0] = (Column_LSB0 + L);    //Column Address CommandLSB = Column Address Initial Command
	// + Column Address bits 0..3 */
	ColumnAddress[1] = (Column_MSB0 + H);    //Column Address CommandMSB = Column Address Initial Command
	// + Column Address bits 4..7
	LCDSendCmd(ColumnAddress, 2);
}


/*
FUNCTION:		LCDClean

DESCRIPTION:	Sets All pixels Off

PARAMETERS:		None

RETURN:			None
*/
void LCDClean(void) {
	unsigned char LcdData[] = {0x00};
	for(unsigned char p=0; p<26; p++) {                  /* 25  Pages */
		for(unsigned char c=0; c<160; c++) {               /* 160 Columns */
			LCDSetPageAddress(0x00 + p);
			LCDSetColumnAdress(0x00 + c);
			LCDSendData(LcdData, 1);
		}
	}
}

/**************************************************************************************************
* @fn      LCDWrite
*
* @brief   Writes a font from bitmap to the LCD
*
* @param   uint8 row -   Page Address
*          uint8 col -   Column Address
*          uint16 f  -   Pointer to the font to be written to the LCD
*
* @return  None
**************************************************************************************************/
void LCDWrite(unsigned char row, unsigned char col, unsigned char width, unsigned char height, unsigned short fstart, const unsigned char* bitmap) {
	/* Each Font consists of 5 Columns of 2 Pages */
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

void LCDWriteString (unsigned char y, unsigned char x, char word[], unsigned char size) {
	unsigned char k;
	unsigned char a = 0;
	const unsigned char* bitmap;
	const unsigned short* descriptors;
	if(size==1) {
		bitmap = SmallFontBitmaps;
		descriptors = SmallFontDescriptors;
	}
	else if(size==2) {
		bitmap = MediumFontBitmaps;
		descriptors = MediumFontDescriptors;
	}
	else if(size==3) {
		bitmap = BigNumbersBitmaps;
		descriptors = BigNumbersDescriptors;
	}
	else {	// Default if invalid size
		bitmap = SmallFontBitmaps;
		descriptors = SmallFontDescriptors;
	}

	do {
		k = word[a] - descriptors[0];
		LCDWrite(y, x, descriptors[(k+1)*3-1], descriptors[(k+1)*3], descriptors[(k+1)*3+1],bitmap);
		y = y + descriptors[(k+1)*3-1]*2;
		a++;
		k = word[a];
	} while(k!=0);
}


void LCDWritePicture(unsigned char y, unsigned char x, unsigned char picture) {
	#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	const unsigned char* bitmap;
	const unsigned short* descriptors;
	if(picture==1)	{
		bitmap = DTULogoBitmaps;
		descriptors = DTULogoDescriptors;
	}
	else if(picture==2)	{
		// Has been deleted
	}
	else if(picture==3)	{
		bitmap = WaterTempBitmaps;
		descriptors = WaterTempDescriptors;
	}
	else if(picture==4)	{
		bitmap = OilTempBitmaps;
		descriptors = OilTempDescriptors;
	}
	else if(picture==5)	{
		bitmap = TimerBitmaps;
		descriptors = TimerDescriptors;
	}
	else if(picture==6)	{
		bitmap = DanceOne;
		descriptors = DanceOneDescriptors;
	}
	else if(picture==7)	{
		bitmap = DanceTwo;
		descriptors = DanceTwoDescriptors;
	}
	else if(picture==8)	{
		bitmap = DanceThree;
		descriptors = DanceThreeDescriptors;
	}
	else {	// Invalid picture ID
		return;
	}

	LCDWrite(y, x, descriptors[0], descriptors[1], descriptors[2],bitmap);
}

void LCDIntro(void) {
	LCDColor(RED);
	LCDWritePicture(0, 39, 1);
	delay_ms(2000);
	LCDClean();
	LCDInit();
}

// Currently only full color blends are available. For full custom mixes, a PWM needs to be used.
void LCDColor(unsigned char color) {
	if(color==RED) {		// Red
		PORTD |= (1<<PORTD6);
		PORTD &= ~((1<<PORTD4) | (1<<PORTD5));
	}
	else if(color==GREEN) {	// Green
		PORTD |= (1<<PORTD5);
		PORTD &= ~((1<<PORTD4) | (1<<PORTD6));
	}
	else if(color==BLUE) {	// Blue
		PORTD |= (1<<PORTD4);
		PORTD &= ~((1<<PORTD5) | (1<<PORTD6));
	}
	else if(color==WHITE) {	// White
		PORTD |=  (1<<PORTD4)| (1<<PORTD5) | (1<<PORTD6);
	}
	else if(color==AQUA) {	// Blue-Green ("Aqua")
		PORTD |=  (1<<PORTD4)| (1<<PORTD5);
		PORTD &= ~(1<<PORTD6);
	}
	else if(color==YELLOW) {	// Red-Green ("Yellow")
		PORTD |=  (1<<PORTD6)| (1<<PORTD5);
		PORTD &= ~(1<<PORTD4);
	}
	else if(color==FUCHSIA) {	// Red-Blue ("Fuchsia")
		PORTD |=  (1<<PORTD4)| (1<<PORTD6);
		PORTD &= ~(1<<PORTD5);
	}
}