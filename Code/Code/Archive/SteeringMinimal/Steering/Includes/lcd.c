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

char speed_str[3] = {0};
char water_temp_str[4] = {0};
char oil_temp_str[4] = {0};
char timer_str[6] = {0};
char rpm_str[5] = {0};
char batvolt_str[5] = {0};

void lcd_update(void) {
	if(check_button(BTN_PARTY_TIME)) {
		PartyDance();
	}
	else if(check_button(BTN_GEAR_DOWN)) {
		GearPage(1);
	}
	else if(check_button(BTN_GEAR_UP)) {
		GearPage(2);
	} 
	else {
		uint8_t min = (uint8_t)((time / 60000) % 60);
		uint8_t sec = (uint8_t)(time / 1000) % 60;
		SpeedoPage(steering_in[STEERING_KMH],
			steering_in[STEERING_CLOSED_LOOP],
			steering_in[STEERING_IDLE],
			steering_in[STEERING_RPM_H] << 8 | steering_in[STEERING_RPM_L],
			min,
			sec,
			0, 
			0, 
			steering_in[STEERING_WATER_TEMP],
			steering_in[STEERING_OIL_TEMP],
			steering_in[STEERING_GEAR],
			steering_in[STEERING_BAT_VOLTAGE_H] << 8 | steering_in[STEERING_BAT_VOLTAGE_L],
			steering_in[STEERING_F_BLINK_L],
			steering_in[STEERING_F_BLINK_R]);
	}
}

/*
 * FUNCTION:		SpeedoPage
 * DESCRIPTION:	Updates the speedometer page
 */
void SpeedoPage(uint8_t speed, uint8_t closed_loop, uint8_t idle, uint16_t rpm, uint8_t minf, uint8_t secf, uint8_t lap_min, uint8_t lap_sec, uint8_t water_temp, uint8_t oil_temp, uint8_t gear, uint16_t batvolt, uint8_t blink_left, uint8_t blink_right) {
	// Temperatures
	LCDWritePicture(0, 5, 3);
	// Water temperature
	sprintf(water_temp_str, "%2i", water_temp);
	LCDWriteString(3, 0, water_temp_str, 1);
	// Oil temperature
	sprintf(oil_temp_str, "%2i", oil_temp);
	LCDWriteString(3, 14, oil_temp_str, 1);
	
	// Timers
	LCDWritePicture(12, 5, 5);
	// Total time
	sprintf(timer_str, "%02i:%02i", minf, secf);
	LCDWriteString(16, 0, timer_str, 1);
	// Lap time
	sprintf(timer_str, "%02i:%02i", lap_min, lap_sec);
	LCDWriteString(16, 14, timer_str, 1);	
	// Speed
	sprintf(speed_str, "%2i", speed);
	LCDWriteString(2, 40, speed_str, 3);	
	// RPM
	sprintf(rpm_str, "%4i", rpm);
	LCDWriteString(5, 107, rpm_str, 2);
	
	if(gear == 1) {
		LCDWriteString(7, 146, "1", 1);
	}
	else if(gear == 2) {
		LCDWriteString(7, 146, "2", 1);
	}
	else if(gear == 3) {
		LCDWriteString(7, 146, "N", 1);
	}
	else if(gear == 4) {
		LCDWriteString(7, 146, "A", 1);
	}
	LCDWriteString(10, 146, "GEAR", 1);
	
	uint32_t Ivolt = (batvolt<1000l) ? batvolt : 999l;
	sprintf(batvolt_str, "%3ld", Ivolt);	
	LCDWriteString(15, 26, "dV", 1);
	LCDWriteString(8, 26, batvolt_str, 1);	
	
	if(closed_loop) {
		LCDWriteString(0, 146, "CL", 1);
	}
	else {
		LCDWriteString(0, 146, " ", 1);
	}
	
	if(idle) {
		LCDWriteString(21, 146, "TG", 1);
	}
	else {
		LCDWriteString(21, 146, " ", 1);
	}
	
	//Blink
	if(blink_right) {
		LCDWriteString(22, 25, "->", 1);
	}
	else {
		LCDWriteString(22, 25, " ", 1);
	}
	if(blink_left) {
		LCDWriteString(0, 25, "<-", 1);
	}
	else {
		LCDWriteString(0, 25, " ", 1);
	}
}

/*
FUNCTION:		GearPage

DESCRIPTION:	Updates the gear page

PARAMETERS:		uint8_t gear - 1, 2 or N gear (1, 2, 3)

RETURN:			None
*/
void GearPage(uint8_t gear) {
	LCDClean();
	if(gear == 1) {
		LCDWriteString(7, 40, "1", 3);
	}
	else if(gear == 2) {
		LCDWriteString(7, 40, "2", 3);
	}
	else if(gear == 3) {
		LCDWriteString(7, 40, "N", 3);
	}
	else if(gear == 4) {
		LCDWriteString(5, 40, "AUTO", 2);
	}
	LCDWriteString(5, 107, "GEAR", 2);
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

void PartyDance(void) {
	for(int i=0; i<=5; i++) {
		LCDColor(AQUA);
		LCDWritePicture(0, 0, 6);
		delay_ms(500);
		LCDColor(YELLOW);
		LCDWritePicture(0, 0, 7);
		delay_ms(500);
		LCDColor(FUCHSIA);
		LCDWritePicture(0, 0, 8);
		delay_ms(500);
	}
	LCDClean();
	LCDColor(BACKLIGHT_COLOR);
}