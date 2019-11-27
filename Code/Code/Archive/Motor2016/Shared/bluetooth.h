/*
 * BlueTooth.h
 *
 * Created: 29/05/2016 13:40:54
 *  Author: Henning
 */ 

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

void blue_init(void);
void blue_putc(unsigned char data);
void blue_puts(char * s);
uint16_t blue_available();
void blue_flush_buffer();
unsigned char blue_read_data();
void blue_read_buffer(void * buff, uint16_t len);

#define ANDROID_START_STRING "START"
#define ANDROID_END_STRING "END"

#endif /* BLUETOOTH_H_ */