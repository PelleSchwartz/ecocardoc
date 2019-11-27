#include "stdint.h"

#define BAUD        250000

#define BLINK_H      9
#define STARTER_V   10 
#define LED3        11
#define LED4        12
#define BACK        13
#define STARTER_H   3      
#define BLINK_V     5  
#define BACKPWM     6  
#define DIR_PIN     8
#define DIR_RX      0
#define DIR_TX      1



enum {
	BACK_LIGHT_STATE,
	BACK_LIGHT_PWM,
	STOP_LIGHT_STATE,
	STOP_LIGHT_PWM,
	STARTER_LIGHT_STATE,
	STARTER_LIGHT_PWM,
	TURNSIG_RIGHT_STATE,
	TURNSIG_RIGHT_PWM,
	TURNSIG_LEFT_STATE,
	TURNSIG_LEFT_PWM,

	// Leave this last entry untouched
	REG_SIZE
};

uint8_t reg[REG_SIZE] = {0x00};

void bus_update();
void get_reg(uint8_t addr, uint8_t dir);
void set_reg(uint8_t addr, uint8_t dir);

void setup() {
	Serial.begin(BAUD);
	pinMode(DIR_PIN, OUTPUT);
	digitalWrite(DIR_PIN, DIR_RX);
	pinMode(BLINK_H, OUTPUT);
	pinMode(STARTER_V, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	pinMode(BACK, OUTPUT);
	pinMode(STARTER_H, OUTPUT);
	pinMode(BLINK_V, OUTPUT);
	pinMode(BACKPWM, OUTPUT);
}

void loop() {

	bus_update();

	digitalWrite(BLINK_H,reg[TURNSIG_RIGHT_STATE]);
        digitalWrite(BLINK_V,reg[TURNSIG_LEFT_STATE]);
        
	digitalWrite(STARTER_V,reg[STARTER_LIGHT_STATE]);
        digitalWrite(STARTER_H,reg[STARTER_LIGHT_STATE]);

	digitalWrite(BACK,reg[STARTER_LIGHT_STATE]);
	



	//digitalWrite(LED3,1);
	//digitalWrite(BACK,1);
	//analogWrite(LED3, 20); 

	if(reg[STOP_LIGHT_STATE]==1){
		analogWrite(LED3, 255);
	}else {
		if(reg[BACK_LIGHT_STATE]==1){
			analogWrite(LED3, 20);
		}else{
			analogWrite(LED3, 0); 
		}

	}  

	//digitalWrite(STOP,reg[STOP_LIGHT_STATE]);

}

void bus_update() {
	uint8_t op, addr, len, crc;

	if(Serial.available() > 0) {
		if(Serial.read() == 0xA5) {              // New frame received
			while(Serial.available() <= 0);
			if((Serial.peek()>>1) == 0x03) {       // Is it talking to us?
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
