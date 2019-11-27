#include "Screen.h"
#include "canbusKeywords.h"

//temp




/*
	functions to implement:
	--Status bar / warnings -> eg. if temp are out of nominel values, canbus is down (or devices are offline)
	--Dancing woman for party mode
	--
*/	

//Used for clearing text before writing new text
int16_t x1, y2;
uint16_t w, h;

void updateSpeed();
void updateWater();
void updateOil();
void updateDistance();
void updateGear();
void updateLambda();
void updateRPM();
void updateTime();
void updateBatteryVoltage();
void updateAVGSpeed();
void updateKmL();
void updateAuto();

float speed, waterTemp, oilTemp, dist, gear, lambda, rpm, batteryVoltage, avgSpeed, kmL; //dunno, what's needed?
uint8_t error;
																						 //gps data... 
//boolean variables to detect changes. If no change, display does not have to be updated... (execept first time... )
uint8_t speedChange, waterTempChange, oilTempChange, distChange, gearChange, lambdaChange, rpmChange, batteryVoltageChange,avgSpeedChange, kmLChange, errorChange;
uint8_t displayMode = 0;
// true if current display mode is initialized
uint8_t  displayInit = 0;
//Temp time
uint32_t temp_time,time_offset;
//Burner flag
uint8_t timer_flag = 1;
//Auto state
uint8_t autoState;


//ex in button interrupt. 
void toggleMode () {
	displayMode++;
  displayInit = 0;
	if (displayMode >= NUM_MODES)
	{
		displayMode = 0;
	}
}
void resetDisplay() {
	screen_init();
	displayInit = 0;
	displayMode = NORMAL_DISP;
	updateDisplay();
}
void resetTimer() {
//	time_offset = millis();
	timer_flag = 0;
	//Clear screen
	tft.setTextSize(2);
	tft.setFont();
	tft.setCursor(TIME_X_OFFSET, TIME_Y_OFFSET);
	tft.setTextColor((uint16_t)0xFFFFF, (uint16_t)0x00000);
	tft.println("      ");
	//tft.setCursor(TIME_X_OFFSET, TIME_Y_OFFSET);
	//tft.println("0:00");
}
void startTimer() {
	if (!timer_flag) {
		time_offset = millis();
		timer_flag = 1;
	}
}


void screen_init() {
	tft.begin(0x9481);
	delay(20);
	tft.setRotation(2);
	tft.fillScreen(0x0000);
	//drawWelcome();
}

void drawInfoBox() {
	if (!errorChange && displayInit) {
		return;
	}
	errorChange = 0;
	tft.setFont(&FreeMono18pt7b);
	tft.setTextSize(1);
	tft.getTextBounds("2", INFO_X_OFFSET, INFO_Y_OFFSET, &x1, &y2, &w, &h);
	tft.fillRect(0, y2, 320, h + 5, 0x00000);

	switch (error)
	{
	case ERROR_OK:
		break;
	case ERROR_DEFAULT:
		tft.setCursor(INFO_X_OFFSET, INFO_Y_OFFSET);
		tft.println("DEFAULT ERROR");
		break;
	case ERROR_BATTERY:
		tft.setCursor(INFO_X_OFFSET, INFO_Y_OFFSET);
		tft.println("BATTERY ERROR");
		break;
	case ERROR_EXTERNAL_BUTTON:
		tft.setCursor(INFO_X_OFFSET, INFO_Y_OFFSET);
		tft.println("EXT. SWITCH");
		break;
	}
	

}

void updateAuto() {
	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setTextSize(2);
		tft.setFont();
		tft.setCursor(AUTO_X_OFFSET, AUTO_Y_OFFSET);
		if (autoState) {
			tft.println("AUTO ON  ");
		}
		else {
			tft.println("AUTO OFF");
		}
	}
	
	
}

void updateDisplay() {
	switch (displayMode) {
	case NORMAL_DISP:	
		if (!displayInit) {
			drawFrameNormal();
		}
		updateSpeed();
		updateTime(); 
		updateBatteryVoltage();
		updateWater();
		updateOil();
		updateLambda();
		updateDistance();
		updateRPM();
		updateGear();
		updateKmL();
		updateAVGSpeed();
		drawInfoBox();
		updateAuto();
		displayInit = 1;
		break;
	case GPS_DISP:
		 
		
		break;
	case DEBUG_DISP:
		if (!displayInit) {
			drawFrameDebug();
		}
		updateSpeed();
		updateWater();
		updateOil();
		updateDistance();
		updateGear();
		updateLambda();
		updateRPM();
		displayInit = 1;
		break;

	default:
		
		break;
	
	}
}





//The update/values/ functions will redraw numbers on change. 
void updateSpeed() {
	if (!speedChange && displayInit) {
		return;
	} 
	speedChange = 0;
 
	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono24pt7b);
		tft.setTextSize(2);
		tft.getTextBounds("22", SPEED_X_OFFSET, SPEED_Y_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1-5, y2, w+20, h+5, 0x00000);
		if (speed < 10) {
			tft.setCursor(SPEED_X_OFFSET + 30, SPEED_Y_OFFSET);
		}
		else {
			tft.setCursor(SPEED_X_OFFSET, SPEED_Y_OFFSET);
		}
		tft.println((int) speed);
		break;
	case DEBUG_DISP:
		tft.setCursor(indent, debugTextOffset);
		tft.setTextSize(debugTextSize);
		tft.getTextBounds("22.22", indent, debugTextOffset, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
		tft.setFont(&FreeMono12pt7b);
		tft.println(speed);
		break;
	default:
		break;
	}
}
void updateWater() {
  if (!waterTempChange && displayInit) {
    return;
  } 
  waterTempChange = 0;
 
  switch (displayMode)
  {
  case NORMAL_DISP:
	  tft.setFont(&FreeMono12pt7b);
	  tft.setTextSize(1);
	  tft.getTextBounds("22.22", MS_X_OFFSET, MS_Y_OFFSET+MS_Y_LINE_OFFSET, &x1, &y2, &w, &h);
	  tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
	  tft.setCursor(MS_X_OFFSET, MS_Y_OFFSET+MS_Y_LINE_OFFSET);
	  tft.println((int) waterTemp);
	  break;
  case DEBUG_DISP:
    tft.setCursor(indent, debugTextOffset + letterHeight);
    tft.setTextSize(debugTextSize);
	tft.getTextBounds("22.22", indent, debugTextOffset + letterHeight, &x1, &y2, &w, &h);
	tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
	tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
	tft.setFont(&FreeMono12pt7b);
    tft.println(waterTemp);
    break;
  default:
    break;
  }
}
void updateOil() {
	if (!oilTempChange && displayInit) {
		return;
	}
	oilTempChange = 0;

	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("22.22", 2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET+MS_Y_LINE_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
		tft.setCursor(2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET+MS_Y_LINE_OFFSET);
		tft.println((int) oilTemp);
		break;
	case DEBUG_DISP:
		tft.setCursor(indent, debugTextOffset + 2*letterHeight);
		tft.setTextSize(debugTextSize);
		tft.getTextBounds("22.22", indent, debugTextOffset + 2*letterHeight, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t)0x00000);
		tft.setFont(&FreeMono12pt7b);
		tft.println(oilTemp);
		break;
	default:
		break;
	}
}
void updateDistance() {
	if (!distChange && displayInit) {
		return;
	}
	distChange = 0;

	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("2222", MS_X_OFFSET, MS_Y_OFFSET + 2*MS_Y_LINE_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
		tft.setCursor(MS_X_OFFSET, MS_Y_OFFSET + 2*MS_Y_LINE_OFFSET);
		tft.println((int)dist);
		break;
	case DEBUG_DISP:
		tft.setCursor(indent, debugTextOffset + 3*letterHeight);
		tft.setTextSize(debugTextSize);
		tft.getTextBounds("22.22", indent, debugTextOffset + 3*letterHeight, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
		tft.setFont(&FreeMono12pt7b);
		tft.println(dist);
		break;
	default:
		break;
	}
}
void updateGear() {
	if (!gearChange && displayInit) {
		return;
	}
	gearChange = 0;

	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono18pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("22",GEAR_X_OFFSET, GEAR_Y_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
		tft.setCursor(GEAR_X_OFFSET, GEAR_Y_OFFSET);
		if (gear == 1) {
			tft.println("1");
		}
		else if(gear == 0){
			tft.println("N");
		}
		else if (gear == 2) {
			tft.println("2");
		}
		
		break;
		
	case DEBUG_DISP:
		tft.setCursor(indent, debugTextOffset + 4*letterHeight);
		tft.setTextSize(debugTextSize);
		tft.getTextBounds("22.22", indent, debugTextOffset + 4*letterHeight, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
		tft.setFont(&FreeMono12pt7b);
		tft.println(gear);
		break;
	default:
		break;
	}
}


void updateLambda() {
	if (!lambdaChange && displayInit) {
		return;
	}
	lambdaChange = 0;

	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("22.22", 2*MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
		tft.setCursor(2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET);
		tft.println(lambda);
		break;
	case DEBUG_DISP:
		tft.setCursor(indent, debugTextOffset + 5*letterHeight);
		tft.setTextSize(debugTextSize);
		tft.getTextBounds("22.22", indent, debugTextOffset + 5*letterHeight, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
		tft.setFont(&FreeMono12pt7b);
		tft.println(lambda);
		break;
	default:
		break;
	}
}


void updateRPM() {//update RPM values on change
	if (!rpmChange && displayInit) {
		return;
	}
	rpmChange = 0;

	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("22.22", 2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET + 2*MS_Y_LINE_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
		tft.setCursor(2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET + 2*MS_Y_LINE_OFFSET);
		tft.println((int)rpm);
		break;
	case DEBUG_DISP:
		tft.setCursor(indent, debugTextOffset + 6*letterHeight);
		tft.setTextSize(debugTextSize);
		tft.getTextBounds("22.22", indent, debugTextOffset + 6*letterHeight, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 20, h + 5, 0x00000);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
		tft.setFont(&FreeMono12pt7b);
		tft.println(rpm);
		break;
	default:
		break;
	}
}



void loadValues(float newSpeed, float newWaterTemp, float newOilTemp, float newDist, float newGear, float newLambda, float newRpm, float newBatteryVoltage, float newKmL, uint8_t newError) {
	if (newSpeed != speed) {
		speed = newSpeed;
		speedChange = 1;
	}
	if (newWaterTemp != waterTemp) {
		waterTemp = newWaterTemp;
		waterTempChange = 1;
	}
	if (newOilTemp != oilTemp) {
		oilTemp = newOilTemp;
		oilTempChange = 1;
	}
	if (newDist != dist) {
		dist = newDist;
		distChange = 1;
	}
	if (newGear != gear) {
		gear = newGear;
		gearChange = 1;
	}
	if (newLambda != lambda) {
		lambda = newLambda;
		lambdaChange = 1;
	}
	if (newRpm != rpm) {
		rpm = newRpm;
		rpmChange = 1;
	}
	if ( newBatteryVoltage != batteryVoltage) {
		batteryVoltage = newBatteryVoltage;
		batteryVoltageChange = 1;
	}
	/*if (newAvgSpeed != avgSpeed) {
		avgSpeed = newAvgSpeed;
		avgSpeedChange = 1;
	}*/
	if (newKmL != kmL) {
		kmL = newKmL;
		kmLChange = 1;
	}
	if (newError != error) {
		error = newError;
		errorChange = 1;
	}
	//add function for gps data. CHECK if distance > x meter. Else return.
}

void loadAutoState(uint8_t autoButton)
{
	autoState = autoButton;
}



void updateTime() {
	uint32_t time_min, timer;

	if (!timer_flag) {
		return;
	}

	if (millis() - temp_time < 1000) {
		return;
	}
	temp_time = millis();
	timer = millis() - time_offset;

	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setTextSize(2);
		tft.setFont();
		tft.setCursor(TIME_X_OFFSET, TIME_Y_OFFSET);
		tft.setTextColor((uint16_t) 0xFFFFF, (uint16_t) 0x00000);
		time_min = timer / (1000 * 60); //Get time in min
		tft.print(time_min);
		tft.print(":"); 
		tft.print(timer / 1000 - time_min * 60);
		if (timer / 1000 - time_min * 60 < 10) { 
			tft.print(" ");
		}
	
		break;
	}



}




void updateBatteryVoltage() {
	if (!batteryVoltageChange && displayInit) {
		return;
	}
	batteryVoltageChange = 0; 
	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("22.22", MS_X_OFFSET, MS_Y_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 10, h + 5, 0x00000);
		tft.setCursor(MS_X_OFFSET, MS_Y_OFFSET);
		tft.println(batteryVoltage);
		break;
	}
}





void updateAVGSpeed() {
	if (!avgSpeedChange && displayInit) {
		return;
	}
	avgSpeedChange = 0;
	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("222", MS_X_OFFSET, MS_Y_OFFSET + 3 * MS_Y_LINE_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 5, h + 5, 0x00000);
		tft.setCursor(MS_X_OFFSET, MS_Y_OFFSET + 3 * MS_Y_LINE_OFFSET);
		tft.println((int) avgSpeed);
		break;
	}
}

void updateKmL() {
	if (!kmLChange && displayInit) {
		return;
	}
	kmLChange = 0;
	switch (displayMode)
	{
	case NORMAL_DISP:
		tft.setFont(&FreeMono12pt7b);
		tft.setTextSize(1);
		tft.getTextBounds("2222", 2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET + 3 * MS_Y_LINE_OFFSET, &x1, &y2, &w, &h);
		tft.fillRect(x1 - 5, y2, w + 5, h + 5, 0x00000);
		tft.setCursor(2 * MS_X_OFFSET + MS_X_TEXT_OFFSET, MS_Y_OFFSET + 3 * MS_Y_LINE_OFFSET);
		tft.println((int)kmL);
		break;
	}
}