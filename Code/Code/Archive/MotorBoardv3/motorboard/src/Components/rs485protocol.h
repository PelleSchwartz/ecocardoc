/*
 * rs485.h
 *
 * Created: 23-03-2015 10:44:20
 *  Author: Kim
 *	Note: Mostly copied and modified from RS232.c from old master board.
 */ 


#ifndef RS485PROTOCOL_H_
#define RS485PROTOCOL_H_




void initiateRS485(void);

void rs485_send_packet(uint8_t id, uint8_t operation, uint8_t local, uint8_t dest, uint8_t len);
void rs485_send_packet2(uint8_t id, uint8_t operation, uint8_t dest, uint8_t len);


#endif /* RS485PROTOCOL_H_ */