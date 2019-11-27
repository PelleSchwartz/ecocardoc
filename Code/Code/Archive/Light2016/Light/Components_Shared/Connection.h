/*
 * Connection.h
 *
 * Created: 2016-05-14 20:15:53
 *  Author: kry
 */ 


#ifndef CONNECTION_H_
#define CONNECTION_H_

//these needs to be initiated outside connection.
void connection_initiate(uint8_t inDataSize, uint8_t outDataSize, uint8_t unitID);
uint8_t connection_GetInData(int byteNr);
void connection_SetOutData(int byteNr, uint8_t value);
void connection_SetOutDataBit(int byteNr, int bitNr, uint8_t highLow);
uint8_t connection_AnyChanges(void);

#endif /* CONNECTION_H_ */