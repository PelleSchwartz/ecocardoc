/*
 * pwm.h
 *
 * Created: 05/05/2016 12:57:33
 *  Author: Kim
 */ 


#ifndef PWM_H_
#define PWM_H_

void oc1_init(void);
void oc1a_activate(void);//port B1, dvs. LED 14
void oc1b_activate(void);//port B2, dvs. LED 10

//With current setting, from 0 to 2500.
void oc1a_set_pwm(uint16_t duty);
void oc1b_set_pwm(uint16_t duty);

#endif /* PWM_H_ */