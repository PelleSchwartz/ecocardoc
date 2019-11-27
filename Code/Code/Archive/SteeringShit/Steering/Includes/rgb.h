#ifndef RGB_H_
#define RGB_H_

// Public function declarations
void send(unsigned char* values);
void Clear_LEDs(void);
void LED_RPM(unsigned short RPM);
void Set_LED_Color(unsigned char LED, unsigned char R, unsigned char G, unsigned char B);
void update_LED(void);

#endif /* RGB_H_ */