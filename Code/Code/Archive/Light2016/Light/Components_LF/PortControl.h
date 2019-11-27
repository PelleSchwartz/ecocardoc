/*
 * LightControl.h
 *
 * Created: 2016-05-16 12:33:19
 *  Author: kry
 */ 


#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_

void portControl_Initiate(void);
void portControl_Wiper(uint8_t onOff);
void portControl_Fan(uint8_t onOff);
void portControl_Horn(uint8_t onOff);

uint8_t portControl_ReadBrake(void);

void portControl_NormalLight(uint8_t onOff);
void portControl_LeftSignal(uint8_t onOff);
void portControl_RightSignal(uint8_t onOff);

#endif /* LIGHTCONTROL_H_ */