#include "stdint.h"

#define BAUD        250000
#define TURNSIG_LEFT    9
#define TURNSIG_RIGHT  	13
#define FRONT_LIGHT_L	6
#define FRONT_LIGHT_R   12
#define BEAM_L_PWM 	11
#define BEAM_R_PWM  5
#define WSVIPER        	10
#define AIRFAN        	3
#define DIR_PIN     	8
#define DIR_RX      	0
#define DIR_TX      	1

enum {
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
  REG_SIZE
};

uint8_t reg[REG_SIZE] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void bus_update();
void get_reg(uint8_t addr, uint8_t dir);
void set_reg(uint8_t addr, uint8_t dir);

void setup() {
  Serial.begin(BAUD);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, DIR_RX);
  pinMode(TURNSIG_LEFT, OUTPUT);
  pinMode(TURNSIG_RIGHT, OUTPUT);
  pinMode(BEAM_L_PWM, OUTPUT);
  pinMode(BEAM_R_PWM, OUTPUT);
  pinMode(FRONT_LIGHT_L, OUTPUT);
  pinMode(FRONT_LIGHT_R, OUTPUT);
  pinMode(WSVIPER, OUTPUT);
  pinMode(AIRFAN, OUTPUT);
}

void loop() {
  bus_update();
  pin_update();
}

void bus_update() {
  uint8_t op, addr, len, crc;
  
  if(Serial.available() > 0) {
    if(Serial.read() == 0xA5) {              // New frame received
      while(Serial.available() <= 0);
      if((Serial.peek()>>1) == 0x01) {       // Is it talking to us?
        op = Serial.read() & 0x01;
        while(Serial.available() <= 0);
        addr = Serial.read();
        while(Serial.available() <= 0);
        len = Serial.read();
        // Add CRC here
        (op==1) ? set_reg(addr, len) : get_reg(addr, len);
      }
    }
  }
}
void pin_update(){
   // Front light
  if(reg[FRONT_LIGHT_STATE]==1){
    digitalWrite(FRONT_LIGHT_R, 1);
    digitalWrite(FRONT_LIGHT_L, 1);
    analogWrite(BEAM_R_PWM, 20);
	analogWrite(BEAM_L_PWM, 20);
  }else{
	digitalWrite(FRONT_LIGHT_R, 0);
    digitalWrite(FRONT_LIGHT_L, 0);
    analogWrite(BEAM_R_PWM, 0);
	analogWrite(BEAM_L_PWM, 0);
  }
  
  // Left turn
  if(reg[TURNSIG_LEFT_STATE]==1){
	digitalWrite(TURNSIG_LEFT,1);
  }
  else{
	digitalWrite(TURNSIG_LEFT,0);
  }
  
  // Right turn
  if(reg[TURNSIG_RIGHT_STATE]==1){
	digitalWrite(TURNSIG_RIGHT,1);
  }
  else{
	digitalWrite(TURNSIG_RIGHT,0);
  }	
}
/* get_reg: Return register(s) to master module */
void get_reg(uint8_t addr, uint8_t len) {
  reg[LED2_STATE] = !reg[LED2_STATE];
  uint8_t i;
                                                // Procedure:
  delay(1);                                    // 1:   Wait for Master to enter Receive mode
  digitalWrite(DIR_PIN, DIR_TX);                // 2:   Change direction to TX
  while(Serial.available() > 0) Serial.read();// 3:   Clear receive buffer
  for(i=0;i<len;i++) Serial.write(reg[i+addr]);// 4:   Send data
  //Serial.flush();                              // 5:   Wait for transmission to finish
  //delay(10);                                    // 5.5: Wait for shift register cleared
  if((UCSR0A & (1 << TXC0))) UCSR0A |= 1<<TXC0;
  while(!(UCSR0A & (1 << TXC0)));
  UCSR0A |= 1<<TXC0;
  digitalWrite(DIR_PIN, DIR_RX);                // 6:   Change direction to RX
}

/* set_reg: Set register(s) provided by master module */
void set_reg(uint8_t addr, uint8_t len) {
  uint8_t i,j=0;
  for(i=0;i<len;i++) {
    while(Serial.available() <= 0);
    reg[i+addr] = Serial.read();
  }
}

//    digitalWrite(8, HIGH);
//    Serial.write(z);
//    delay(20);
//    digitalWrite(8, LOW);
//  }
//  
//}

