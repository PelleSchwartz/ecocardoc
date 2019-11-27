/*
 * LightControl.h
 *
 * Created: 2016-05-06 17:45:52
 *  Author: kry
 */ 


#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_

void lightControl_Initiate(void);
void lightControl_LightsOn(uint8_t onOff);

void lightControl_StarterLight(uint8_t);
void lightControl_BrakeLight(uint8_t);
void lightControl_NormalLight(uint8_t);
void lightControl_LeftSignal(uint8_t);
void lightControl_RightSignal(uint8_t);
void update_blink_state(void);

#endif /* LIGHTCONTROL_H_ */