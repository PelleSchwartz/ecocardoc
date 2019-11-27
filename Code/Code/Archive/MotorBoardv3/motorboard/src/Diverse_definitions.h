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
//#define BAUD485		19200
#define BAUD232		19200
#define BAUDEPOS232 115200
#define BAUDBLUE	9600
#define BLUETOOTH_USED_BAUD	(F_CPU/16/BAUDBLUE-1)
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

#define FRONT_BYTEZERO 0
#define FRONT_LIGHT_REGSIZE 1

#define FL_B0_EXTRA				7
#define FL_B0_WIPER				6
#define FL_B0_FAN				5
#define FL_B0_TURNLEFT_LIGHT	4
#define FL_B0_TURNRIGHT_LIGHT	3
#define FL_B0_HORN				2
#define FL_B0_HIGH_BEAM_LIGHT	1
#define FL_B0_NORMAL_LIGHT		0

#define FL_OUT_BYTESIZE			1
//front light out bits
//First Byte
#define FL_OUT_BRAKE			0


#define BACK_BYTEZERO			0
#define BACK_LIGHT_REGSIZE		1

#define BL_B0_STARTER_LIGHT		4
#define BL_B0_STOP_LIGHT		3
#define BL_B0_NORMAL_LIGHT		2
#define BL_B0_TURN_LEFT			1
#define BL_B0_TURN_RIGHT		0

enum {//BOTH LIGHTS
	LIGHTS_SIGNAL_FLAGS,
	LIGHTS_LIGHT_FLAGS,
	LIGHTS_DIVERSE_FLAGS,
	// Leave this last entry untouched
	LIGHTS_REGSIZE
	};
//Bits of the light registers.
#define LSF_TURN_RIGHT	0
#define LSF_TURN_LEFT	1
#define LSF_STARTER		2

#define LLF_FRONTLIGHT_0	0
#define LLF_FRONTLIGHT_1	1
#define LLF_BACKLIGHT		2
#define LLF_STOPLIGHT		3

#define LDF_WIPER	0
#define LDF_FAN		1
#define LDF_HORN	2

enum {//RIO REGISTER
	RIO_START,
	RIO_IDLE,
	RIO_CLOSED_LOOP,
	RIO_AUTOGEAR,
	RIO_POT_UP,
	RIO_POT_DOWN,
	RIO_NEUTRAL,
	RIO_GEAR_UP,
	RIO_GEAR_DOWN,
	RIO_STOP,
	
	//RIO_LAMBDA_H,
	//RIO_LAMBDA_L,
	
	//RIO_GEAR,	// 0 = 1. gear, 1 = 2. gear
	//RIO_HYPERSPEED,
	//RIO_POTMETER_H,
	//RIO_POTMETER_L,
	
	//add lambda sometime.
	// Leave this last entry untouched
	RIO_REGSIZE
};

enum {//RIO RECEIVE REGISTER
	RIO_REC_SAFE_SHUTDOWN,
	RIO_REC_WATER_TEMP,
	RIO_REC_OIL_TEMP,
	RIO_REC_RPM_L,
	RIO_REC_RPM_H,
	RIO_REC_EXCESS_AIR,
	RIO_REC_GEAR,	//to steering
	RIO_REC_KPH,//speed? 
	RIO_REC_FUEL_CONSUMPTION_L,
	RIO_REC_FUEL_CONSUMPTION_H,
	RIO_REC_REMAINING_FUEL_L,
	RIO_REC_REMAINING_FUEL_H,
	RIO_REC_POTENTIOMETER,
	RIO_REC_STARTER,
	RIO_REC_SERVO_GEAR_L,
	RIO_REC_SERVO_GEAR_H,
	
	//old stuff missing for reasons
	//RIO_REC_STARTER,
	//RIO_REC_FANSTART,
	
	//RIO_REC_SERVO_SPLAELD_H,
	//RIO_REC_SERVO_SPLAELD_L,
	//RIO_REC_LAMBDA_H,
	//RIO_REC_LAMBDA_L,
	//RIO_REC_KMPERL_H,	
	//RIO_REC_KMPERL_L,	
	// Leave this last entry untouched
	RIO_REC_REGSIZE
};

enum {//STEERING WHEEL REGISTER
	STEERING_START_MOTOR, //all this should be send to rio
	STEERING_IDLE,
	STEERING_CLOSED_LOOP,
	STEERING_AUTO_GEAR,
	STEERING_POT_UP,
	STEERING_POT_DOWN,
	STEERING_NEUTRAL,
	STEERING_GEAR_UP,
	STEERING_GEAR_DOWN,
	
	//not currently used
	STEERING_GEAR12,
	STEERING_POT_METER_H,
	STEERING_POT_METER_L,
	
	STEERING_HYPERSPEED,
	STEERING_PARTY_TIME,
	
	STEERING_LIGHT,
	STEERING_LIGHT_LENGTH,
	STEERING_WIPER,
	STEERING_FAN,
	STEERING_HORN,
	STEERING_BLINK_L,
	STEERING_BLINK_R,
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
	STEERING_CAP_VOLTAGE_H,
	STEERING_CAP_VOLTAGE_L,
	STEERING_W_CRC1,
	STEERING_W_CRC2,
	// Leave last untouched
	STEERING_REGSIZE_W
};

/*
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
*/

extern uint8_t steering_reg[];
extern uint8_t steering_reg_w[];
extern uint8_t lights_reg[];
extern uint8_t light_F_reg[];
extern uint8_t light_F_out_reg[];
extern uint8_t light_B_reg[];
extern uint8_t rio_tx[];
extern uint8_t rio_temp_rx[];
extern uint8_t rio_rx[];
//uint8_t kers_reg_w[KERS_W_REG_SIZE]  = {0x00,0x00};
//uint8_t kers_reg_r[KERS_R_REG_SIZE]  = {0x00,0x00};
extern uint32_t timeout;

extern volatile uint8_t timer0_interupt_flag,timer0_interupt_flag2;
extern volatile uint8_t rs232_interupt_flag;
extern volatile uint8_t rs232_data[];
extern volatile uint16_t BLINK_TIMER;
extern volatile uint8_t jackUpdateFlag;

//volatile uint8_t kers_burn = 0;
//volatile uint8_t kers_brake = 0;

#endif /* INCFILE1_H_ */