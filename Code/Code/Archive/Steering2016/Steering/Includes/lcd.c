/*
 * lcd.c
 *
 * Created: 23/06/2016 03:04:01
 * Author: Henning
 */ 

#include "global.h"
#include "rs485protocol.h"
#include "buttons.h"
#include "lcd.h"

#define STD_SCREEN_COUNT 10
#define STD_PARTY_STATE_LENGTH 3

void _GearPage(uint8_t gear);

uint8_t party_state = 0;
int16_t last_party_change = 0;
int16_t keep_gear_screen_counter = 0;

static uint8_t _gearScreenGear = 0;

char speed_str[3] = {0};
char water_temp_str[4] = {0};
char oil_temp_str[4] = {0};
char lambda_str[7] = {0};
char rpm_str[5] = {0};
char batvolt_str[5] = {0};

void lcd_update(void) {	
	if(check_button(BTN_FAN)) {
		LCDClean();	// TODO Debug
	}
	else if(keep_gear_screen_counter <= 0) {		
		uint8_t min = 0;	//(uint8_t)((time / 60000) % 60);		// TODO Time
		uint8_t sec = 0;	//(uint8_t)(time / 1000) % 60;
		uint32_t voltage_multi = (steering_in[STEERING_BAT_VOLTAGE_H] << 8 | steering_in[STEERING_BAT_VOLTAGE_L]) * 75UL;
		voltage_multi  = (voltage_multi >> 9);
		uint16_t deci_voltage = (uint16_t)voltage_multi;
		SpeedoPage(steering_in[STEERING_KMH],
			steering_out[STEERING_CLOSED_LOOP],
			steering_out[STEERING_IDLE],
			steering_in[STEERING_RPM_H] << 8 | steering_in[STEERING_RPM_L],
			min,
			sec,
			0, 
			0, 
			steering_in[STEERING_WATER_TEMP],
			steering_in[STEERING_OIL_TEMP],
			steering_in[STEERING_GEAR],
			deci_voltage,
			steering_in[STEERING_F_BLINK_L],
			steering_in[STEERING_F_BLINK_R],
			steering_in[STEERING_LAMBDA],
			steering_out[STEERING_AUTOGEAR]
			);		
	}
	else{
		if(keep_gear_screen_counter == STD_SCREEN_COUNT) {
			_GearPage(_gearScreenGear);
		}
		keep_gear_screen_counter--;
	}
	party_update();
}

/*
 * FUNCTION:	SpeedoPage
 * DESCRIPTION:	Updates the speedometer page
 */
void SpeedoPage(uint8_t speed, uint8_t closed_loop, uint8_t idle, uint16_t rpm, uint8_t minf, uint8_t secf, uint8_t lap_min, uint8_t lap_sec, uint8_t water_temp, uint8_t oil_temp, uint8_t gear, uint16_t batvolt, uint8_t blink_left, uint8_t blink_right, uint8_t lambda, uint8_t autogear) {
	if(party_state == 0) {
		// Temperatures
		LCDWritePicture(0, 5, 3);
		// Water temperature
		sprintf(water_temp_str, "%2i", water_temp);
		LCDWriteString(3, 0, water_temp_str, 1);
		// Oil temperature
		sprintf(oil_temp_str, "%2i", oil_temp);
		LCDWriteString(3, 14, oil_temp_str, 1);
	}
	
	// Timers
	//LCDWritePicture(12, 5, 5);
	// Total time
	sprintf(lambda_str, "%04u", lambda);
	lambda_str[0] = lambda_str[1];
	lambda_str[1] = '.';
	lambda_str[4] = '\0';
	LCDWriteString(16, 0, "|", 1);
	LCDWriteString(18, 0, lambda_str, 1);
	// Lap time
	//sprintf(timer_str, "%02i:%02i", lap_min, lap_sec);
	//LCDWriteString(16, 14, timer_str, 1);	
	// Speed
	sprintf(speed_str, "%2i", speed);
	LCDWriteString(2, 40, speed_str, 3);	
	// RPM
	sprintf(rpm_str, "%4i", rpm);
	LCDWriteString(5, 107, rpm_str, 2);
	
	if(autogear){
		LCDWriteString(6, 146, "A", 1);
	}else{
		LCDWriteString(6, 146, " ", 1);
	}
	
	if(gear == 1) {
		LCDWriteString(8, 146, "1", 1);
	}
	else if(gear == 2) {
		LCDWriteString(8, 146, "2", 1);
	}
	else if(gear == 0) {
		LCDWriteString(8, 146, "N", 1);
	}
	else if(gear == 4) {
		LCDWriteString(8, 146, "A", 1);
	}
	else{
		LCDWriteString(8, 146, "U", 1);
	}
	LCDWriteString(11, 146, "GEAR", 1);
	
	uint16_t Ivolt = (batvolt<1000) ? batvolt : 999;
	sprintf(batvolt_str, "%3u", Ivolt);	
	batvolt_str[3] = batvolt_str[2];
	batvolt_str[2] = '.';	
	LCDWriteString(16, 26, "V", 1);
	LCDWriteString(8, 26, batvolt_str, 1);	
	
	if(closed_loop) {
		LCDWriteString(0, 146, "CL", 1);
	}
	else {
		LCDWriteString(0, 146, "  ", 1);
	}
	
	if(idle) {
		LCDWriteString(21, 146, "TG", 1);
	}
	else {
		LCDWriteString(21, 146, "  ", 1);
	}
	
	//Blink
	if(blink_right) {
		LCDWriteString(22, 25, "->", 1);
	}
	else {
		LCDWriteString(22, 25, "  ", 1);
	}
	if(blink_left) {
		LCDWriteString(0, 25, "<-", 1);
	}
	else {
		LCDWriteString(0, 25, "  ", 1);
	}
}

/*
FUNCTION:		GearPage

DESCRIPTION:	Updates the gear page

PARAMETERS:		uint8_t gear - 1, 2 or N gear (1, 2, 3)

RETURN:			None
*/
void _GearPage(uint8_t gear) {
	LCDClean();
	if(gear == GEAR_1) {
		LCDWriteString(7, 40, "1", 3);
	}
	else if(gear == GEAR_2) {
		LCDWriteString(7, 40, "2", 3);
	}
	else if(gear == GEAR_N) {
		LCDWriteString(7, 40, "N", 3);
	}
	else if(gear == GEAR_AUTO) {
		LCDWriteString(5, 40, "AUTO", 2);
	}
	LCDWriteString(5, 107, "GEAR", 2);
}

void SetGearPage(uint8_t gear){
	if(gear == GEAR_1 || gear == GEAR_2 || gear == GEAR_N ||gear == GEAR_AUTO) {
		_gearScreenGear = gear;
		keep_gear_screen_counter = STD_SCREEN_COUNT;
	}
}

/*
FUNCTION:		PotPage

DESCRIPTION:	Updates the pot page

PARAMETERS:		uint16_t pot

RETURN:			None
*/
void PotPage(uint16_t pot) {
	char pot_str[5];
	sprintf(pot_str, "%4i", pot);
	LCDWriteString(5, 67, pot_str, 2);
}

void party_update(void) {
	if(party_state == 0 && check_button(BTN_PARTY_TIME)) {
		party_state = 1;
		last_party_change = STD_PARTY_STATE_LENGTH;
	}
	if(party_state > 0) {
		switch(party_state) {
			case 1:
			case 4:
			LCDColor(AQUA);
			LCDWritePicture(0, 0, 6);
			if(last_party_change-- <= 0) {
				party_state++;
				last_party_change = STD_PARTY_STATE_LENGTH;
			}
			break;
			case 2:
			case 5:
			LCDColor(YELLOW);
			LCDWritePicture(0, 0, 7);
			if(last_party_change-- <= 0) {
				party_state++;
				last_party_change = STD_PARTY_STATE_LENGTH;
			}
			break;
			case 3:
			case 6:
			LCDColor(FUCHSIA);
			LCDWritePicture(0, 0, 8);
			if(last_party_change-- <= 0) {
				party_state++;
				last_party_change = STD_PARTY_STATE_LENGTH;
			}
			break;
			default:
			LCDClean();
			LCDColor(BACKLIGHT_COLOR);
			party_state = 0;
			break;
		}
	}
}