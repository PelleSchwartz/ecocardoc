#include <stdint.h>

//Public function declarations
void LCDInit(void);
void LCDAllPixelsOn(void);
void LCDAllPixelsOff(void);
void LCDClean(void);
void LCDWriteString(unsigned char y, unsigned char x, char word[], unsigned char size);
void LCDWritePicture(unsigned char y, unsigned char x, unsigned char picture);
void LCDIntro(void);
void LCDColor(unsigned char color);
void SpeedoPage(unsigned char speed, unsigned short rpm, unsigned char min, unsigned char sec, unsigned char lap_min, unsigned char lap_sec, unsigned char water_temp, unsigned char oil_temp, unsigned char gear,unsigned char Kers,unsigned char KersVolt,unsigned char blinkv,unsigned char blinkh);
void SpeedoPage2(unsigned char speed, unsigned char cLoop, unsigned char TomGang, unsigned short rpm, unsigned char min, unsigned char sec, unsigned char lap_min, unsigned char lap_sec, unsigned char water_temp, unsigned char oil_temp, unsigned char gear,unsigned char Kers,unsigned char KersVolt,unsigned char blinkv,unsigned char blinkh);
void SpeedoPage3(unsigned char speed, unsigned char cLoop, unsigned char TomGang, unsigned short rpm, unsigned char min, unsigned char sec, unsigned char lap_min, unsigned char lap_sec, unsigned char water_temp, unsigned char oil_temp, unsigned char gear,unsigned char Kers,uint16_t Volt,unsigned char blinkv,unsigned char blinkh);
void GearPage(unsigned char gear);
void PotPage(unsigned short pot);

//Constants
//These are the basic commands for the EA DOGXL160x-7  LCD

#define Set_Com_End_H					0xF1
#define Set_Com_End_L					0x67
#define Set_LCD_Mapping_Control			0xC2
#define Set_Scroll_Line_LSB				0x40
#define Set_Scroll_Line_MSB				0x50
#define Set_Panel_Loading				0x2B
#define Set_LCD_Bias_Ratio				0xEB
#define Set_Vbias_Potentiometer_H		0x81
#define Set_Vbias_Potentiometer_L		0x5F
#define Set_RAM_Address_Control			0x89
#define Set_Window_Program_Start_CA_H	0xF4
#define Set_Window_Program_Start_CA_L	0x00
#define Set_Window_Program_Start_PA_H	0xF5
#define Set_Window_Program_Start_PA_L	0x00
#define Set_Window_Program_End_CA_H		0xF6
#define Set_Window_Program_End_CA_L		0x9F
#define Set_Window_Program_End_PA_H		0xF7
#define Set_Window_Program_End_PA_L		0x19
#define Set_Display_Enable				0xAF
#define Set_All_Pixels_Off				0xA4
#define Set_All_Pixels_On				0xA5
#define System_Reset					0xE2
#define Page_Address					0x60
#define Column_LSB0						0x00
#define Column_MSB0						0x10
#define CD_LOW							PORTD &= ~(1<<PORTD1)
#define CD_HIGH							PORTD |= (1<<PORTD1)
#define RESET_OFF						PORTD |= (1<<PORTD0);  // Reset high (OFF!)
#define RESET_ON						PORTD &= ~(1<<PORTD0); // Reset low (ON!)
