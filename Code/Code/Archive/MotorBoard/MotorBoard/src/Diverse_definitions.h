/*
 * IncFile1.h
 *
 * Created: 23-03-2015 10:46:05
 *  Author: Kim
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

/* General Definitions */
#define F_CPU 		16000000UL
//#define F_CPU 		1000000UL
//#define F_CPU 		8000000UL
#define BAUD485 	250000
#define BAUD232		19200
#define BAUDEPOS232 115200
#define RS485BAUD 	(F_CPU/16/BAUD485-1)
#define RS232BAUD	(F_CPU/16/BAUD232-1)
#define RS232EPOS2BAUD (F_CPU/16/BAUDEPOS232)-1

#define DIR_RX		0
#define DIR_TX		1
#define RX_TIMEOUT	100000 		// Checks, ~x5-ish clock cycles
#define CMD_DELAY	100000
#define OP_READ		0
#define OP_WRITE	1

#define ID_FRONT_LIGHT	  1
#define ID_REAR_LIGHT	  3
#define ID_STEERING_WHEEL 2
#define ID_KERS			  4

#define TESTBIT(var, bit)	(var  & (1<<bit))
#define SETBIT(var, bit)	(var |= (1<<bit))
#define CLRBIT(var, bit)	(var &= ~(1<<bit))
#define FLIPBIT(var, bit)	(var ^= (1<<bit))

/* Includes */
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Master Specific Definitions */
#define LED1_PIN_D	4	// Untested
#define LED2_PIN_D	5	// Untested
#define DIR_PIN_J	2

enum {//FRONT LIGHT REGISTER
	FRONT_LIGHT_STATE,
	FRONT_LIGHT_PWM,
	TURNSIG_RIGHT_STATE,
	TURNSIG_RIGHT_PWM,
	TURNSIG_LEFT_STATE,
	TURNSIG_LEFT_PWM,
	AIRFAN_STATE,
	AIRFAN_PWM,
	WSVIPER_STATE,
	WSVIPER_FREQ,
	WSVIPER_SPEED,
	LED1_STATE,
	LED2_STATE,
	// Leave this last entry untouched
	FRONT_LIGHT_REGSIZE
};

enum {//BACK LIGHT REGISTER
	BACK_LIGHT_STATE,
	BACK_LIGHT_PWM,
	STOP_LIGHT_STATE,
	STOP_LIGHT_PWM,
	STARTER_LIGHT_STATE,
	STARTER_LIGHT_PWM,
	TURNSIG_B_RIGHT_STATE,
	TURNSIG_B_RIGHT_PWM,
	TURNSIG_B_LEFT_STATE,
	TURNSIG_B_LEFT_PWM,
	// Leave this last entry untouched
	BACK_LIGHT_REGSIZE
};

enum {//RIO REGISTER
	RIO_START,
	RIO_STOP,
	RIO_AUTOGEAR,
	RIO_NEUTRAL,
	RIO_GEAR,	// 0 = 1. gear, 1 = 2. gear
	RIO_MANINJ,
	RIO_POTMETER_H,	
	RIO_POTMETER_L,
	RIO_LAMBDA_H,
	RIO_LAMBDA_L,
	//add lambda sometime.
	// Leave this last entry untouched
	RIO_REGSIZE
};

enum {//RIO RECIVE REGISTER
	RIO_REC_KPH,//speed? 
	RIO_REC_RPM_H,
	RIO_REC_RPM_L,
	RIO_REC_WATER_TEMP,
	RIO_REC_OIL_TEMP,
	RIO_REC_GEAR,	//to steering
	RIO_REC_FANSTART,
	RIO_REC_SERVO_GEAR_H,
	RIO_REC_SERVO_GEAR_L,
	RIO_REC_SERVO_SPLAELD_H,
	RIO_REC_SERVO_SPLAELD_L,
	RIO_REC_STARTER,
	// Leave this last entry untouched
	RIO_REC_REGSIZE
};

enum {//STEERING WHEEL REGISTER
	STEERING_START_MOTOR,
	STEERING_STOP_MOTOR,
	STEERING_AUTO_GEAR,
	STEERING_NEUTRAL,
	STEERING_GEAR12,
	STEERING_MAN_INJECT,
	STEERING_POT_METER_hi,
	STEERING_POT_METER_lo,
	STEERING_LIGHT,
	STEERING_WIPER,
	STEERING_FAN,
	STEERING_HORN,
	STEERING_BLINK_L,
	STEERING_BLINK_H,
	STEERING_KRES_START,
	STEERING_KRES_STOP,
	STEERING_CRC1,
	STEERING_CRC2,
	// Leave last untouched
	STEERING_REGSIZE
};

enum {//Steering wheel register 2.
	STEERING_KPH,
	STEERING_RPM_H,
	STEERING_RPM_L,
	STEERING_VAND_T,
	STEERING_OLIE_T,
	STEERING_GEAR,
	STEERING_ENCODER_FAN,
	STEERING_F_BLINK_R,
	STEERING_F_BLINK_L,
	STEERING_CAP_VOLTAGE,
	STEERING_W_CRC1,
	STEERING_W_CRC2,
	// Leave last untouched
	STEERING_REGSIZE_W
};

enum {
	KERS_W_BOOST,
	KERS_W_STORE,
	KERS_W_REG_SIZE
};

enum {
	KERS_BAT_VOLTAGE,
	KERS_KERS_VOLTAGE,
	KERS_R_REG_SIZE
};

uint8_t steering_reg[STEERING_REGSIZE]  = {0};
uint8_t steering_reg_w[STEERING_REGSIZE_W]  = {0};
uint8_t light_reg[FRONT_LIGHT_REGSIZE] = {0x01, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x3F, 0x00, 0x3F, 0x3F, 0x00, 0x00};
uint8_t light_B_reg[BACK_LIGHT_REGSIZE] = {0x01};
uint8_t rio_reg[RIO_REGSIZE] = {0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01};
uint8_t rio_temp_rx[RIO_REC_REGSIZE] = {0x00};
uint8_t rio_rx[RIO_REC_REGSIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t kers_reg_w[KERS_W_REG_SIZE]  = {0x00,0x00};
uint8_t kers_reg_r[KERS_R_REG_SIZE]  = {0x00,0x00};
uint32_t timeout;

volatile uint8_t timer0_interupt_flag = 0,timer0_interupt_flag2=0;
volatile uint8_t rs232_interupt_flag = 0;
volatile uint8_t rs232_data[50] = {0};
volatile uint16_t BLINK_TIMER = 0;

volatile uint8_t kers_burn = 0;
volatile uint8_t kers_brake = 0;

#endif /* INCFILE1_H_ */