#include "stdint.h"

#define BAUD        250000

#define LED1        9
#define LED2        10
#define LED_PWM     11
#define LED4        12
#define LED5        13
#define LED6        3
#define LED7        5
#define LED8        6
#define DIR_PIN     8
#define DIR_RX      0
#define DIR_TX      1

//For test
unsigned int lights[8] = {LED1,LED2,LED_PWM,LED4,LED5,LED6,LED7,LED8};

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
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED_PWM, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
}

void loop() {
  for(unsigned int i=0;i<8;i++)
  {
    //digitalWrite(lights[i],1);
  }
  //delay(1000);
    for(unsigned int i=0;i<8;i++)
  {
    //digitalWrite(lights[i],0);
  }
  //delay(1000);
  bus_update();
  //digitalWrite(LED1, reg[LED1_STATE]);
  //digitalWrite(LED2, reg[LED2_STATE]);
  
  if(1){
  //if(reg[FRONT_LIGHT_STATE]==1){
    digitalWrite(LED1, 1);
    digitalWrite(LED2, 1);
    digitalWrite(LED4, 1);
    analogWrite(LED_PWM, 20);
  }else{
    digitalWrite(LED1, 0);
    digitalWrite(LED2, 0);
    digitalWrite(LED4, 0);
    analogWrite(LED_PWM, 0);
  }
  

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




//void bus_update() {
//  
//  if(Serial.available()) {
//    digitalWrite(LED1, !digitalRead(LED1));
//    unsigned char i=0, p[6];
//    
//    while(Serial.peek() != 0xA5) Serial.read();
//    
//    for(i=0;i<4;i++) {
//      while(!Serial.available());  // Implement timeout!
//      p[i] = Serial.read();
//    }
//    
//    
//    
//    if((p[0]==0xA5) && (((p[1]>>1) & 0x3F) == 0x01)) {  // Is it talking to us?
//      
//      if((p[1] & 0x01) == 0) {  // MRSW (Master Read Operation)
//        reg[LED1_STATE] = !reg[LED1_STATE]; // Update reg each time. To be removed
//        delay(20);
//        digitalWrite(DIR_PIN, DIR_TX);
//        while(Serial.available() > 0) Serial.read();
//        for(i=0;i<p[3];i++) Serial.write(reg[p[2]+i]);
//        Serial.write(0x01);
//        Serial.write(0x01);
//        Serial.flush();  // Wait for TX to complete
//        digitalWrite(DIR_PIN, DIR_RX);
//        
//      } else {                  // MWSR (Master Write Operation)
//        for(i=0;i<p[3];i++) {
//          while(!Serial.available());  // Implement timeout!
//          reg[p[2]+i] = Serial.read();
//        }
//      }
//    }
//  }
//}






//#include "pins_arduino.h"
//
//void setup() {
//  Serial.begin(9600);
//  
//  pinMode(9, OUTPUT);
//  
//  pinMode(8, OUTPUT);
//  digitalWrite(8, LOW);
//}
//
//void loop() {
//  char z;
//  if(Serial.available() > 0 && (z = Serial.read()) && (z == 0x21)) {
//    
//    digitalWrite(9, !digitalRead(9));
//    delay(20);
//    digitalWrite(8, HIGH);
//    Serial.write(z);
//    delay(20);
//    digitalWrite(8, LOW);
//  }
//  
//}

