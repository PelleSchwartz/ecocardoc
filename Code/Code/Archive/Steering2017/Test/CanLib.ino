
#include "canbus.h"


#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

elapsedMillis LEDtimer;


const uint8_t redLEDpin = 13;
boolean redLEDstate;
uint32_t statusData;
float measurementData;

void setup(void)
{

  uint16_t identifier = 0x9481;

  Serial.begin(9600);


  delay(3000);
  Serial.println(F("CAN class Test With Objects."));
  pinMode(redLEDpin, OUTPUT);

  Can0.begin(1000000);
  Can0.attachObj(&CAN);


  CAN_filter_t allPassFilter;
  allPassFilter.id = 4;
  allPassFilter.ext = 1;
  allPassFilter.rtr = 0;

  //leave the first 4 mailboxes to use the default filter. Just change the higher ones
  for (uint8_t filterNum = 4; filterNum < 14; filterNum++) {
    Can0.setFilter(allPassFilter, filterNum);
  }
  for (uint8_t filterNum = 0; filterNum < 14; filterNum++) {
    CAN.attachMBHandler(filterNum);
  }
}

// Sender en status besked med data og en measurement besked. Modtaget besked gemmes og printes. Skift prioritet eller andet når der uploades til andet board (unikt ID)
void loop(void)
{
  if (LEDtimer >250) {
    LEDtimer = 0;
    CAN.sendStatus(1, STEERING_ID, 8);
    CAN.sendMeasurement(2,MOTORBOARD_ID,TEMPERATURE, SENSOR_WATER_TEMP, 22.135f);
    statusData = CAN.getStatus(STEERING_ID);
    measurementData = CAN.getMeasurement(MOTORBOARD_ID);
    Serial.println("Data fra status fra Steering er: ");
    Serial.print(statusData, HEX);                          //Burde printe 8
    Serial.println("Temperatur af vand målt af motorboard er: ");
    Serial.print(measurementData,3);                        //Burde printe 22.135
  } 
}




