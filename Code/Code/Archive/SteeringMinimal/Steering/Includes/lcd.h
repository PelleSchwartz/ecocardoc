/*
 * lcd.h
 *
 * Created: 23/06/2016 03:04:13
 *  Author: Henning
 */ 

#ifndef LCD_H_
#define LCD_H_

#include "DOGXL-160-7.h"

#define BACKLIGHT_COLOR WHITE

void lcd_update(void);
void SpeedoPage(uint8_t speed, uint8_t closed_loop, uint8_t tomgang, uint16_t rpm, uint8_t minf, uint8_t secf, uint8_t lap_min, uint8_t lap_sec, uint8_t water_temp, uint8_t oil_temp, uint8_t gear, uint16_t batvolt, uint8_t blink_left, uint8_t blink_right);
void GearPage(uint8_t gear);
void PotPage(uint16_t pot);
void PartyDance(void);

#endif /* LCD_H_ */