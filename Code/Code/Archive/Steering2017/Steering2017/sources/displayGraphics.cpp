#include "displayGraphics.h"

Adafruit_TFTLCD tft;

//#define 1tczb-4ys6h_width 40
//#define 1tczb-4ys6h_height 80



void drawWelcome() {
	tft.fillScreen(0x0000);
	tft.setCursor(0, 30);
	tft.println("Display initialized");
}
void drawFrameDebug() {
	tft.fillScreen(0x00000);
	tft.setCursor(0, 20);
	tft.setTextSize(1);
	tft.setTextColor(0x4Ad72, 0x00000);
	tft.setFont(&FreeMonoBold18pt7b);
	tft.println("DEBUG");
	tft.setFont(&FreeMono12pt7b);
	tft.setCursor(0, 45);
	tft.setTextSize(1);
	tft.setTextColor(0xFFFFF, 0x00000);
	tft.println(" Speed:");
	tft.println(" Water Temp:");
	tft.println(" Oil Temp:");
	tft.println(" Distance:");
	tft.println(" Gear:");
	tft.println(" Lambda:");
	tft.println(" RPM:");

	tft.setCursor(MODE_X_OFFSET, MODE_Y_OFFSET);
	tft.println("Mode: Debug");


	//tft.drawXBitmap(1, 1, hellokitty, 279, 363, 0xFFFFF);
	
}

void drawFrameNormal() {
	//Speed
	tft.fillScreen(0x00000);
	tft.setFont(&FreeMono18pt7b);
	tft.setCursor(SPEED_X_OFFSET + 20,SPEED_Y_OFFSET + 40);
	tft.setTextSize(1);
	tft.println("km/h");
	
	//Battery Voltage
	tft.setFont(&FreeMono12pt7b);
	tft.setCursor(8, MS_Y_OFFSET);
	tft.println("BV");
	//Lambda
	tft.setCursor(MS_X_TEXT_OFFSET +MS_X_OFFSET, MS_Y_OFFSET);
	tft.println("La");
	//Water temp
	tft.setCursor(8, MS_Y_OFFSET + MS_Y_LINE_OFFSET);
	tft.println("WT");
	// Oil temp
	tft.setCursor(MS_X_TEXT_OFFSET + MS_X_OFFSET, MS_Y_OFFSET + MS_Y_LINE_OFFSET);
	tft.println("OT");
	//Distance
	tft.setCursor(8, MS_Y_OFFSET + 2*MS_Y_LINE_OFFSET);
	tft.println("Dis");
	//RPM
	tft.setCursor(MS_X_TEXT_OFFSET + MS_X_OFFSET, MS_Y_OFFSET + 2*MS_Y_LINE_OFFSET);
	tft.println("RPM");
	//Average speed
	tft.setCursor(8, MS_Y_OFFSET + 3 * MS_Y_LINE_OFFSET);
	tft.println("AVG");
	// km/L
	tft.setCursor(MS_X_TEXT_OFFSET + MS_X_OFFSET, MS_Y_OFFSET + 3 * MS_Y_LINE_OFFSET);
	tft.println("km/L");

	//Mode
	tft.setCursor(MODE_X_OFFSET, MODE_Y_OFFSET);
	tft.println("Mode: Normal");


	//DTU frame:
	//tft.drawXBitmap(5, 5, DTU, 40, 80, (uint16_t)0xF800);
	tft.drawXBitmap(5, 5, DTU2, 45, 65, (uint16_t)0xF800);
}



