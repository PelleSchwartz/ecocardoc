/*
 * adc.h
 *
 * Created: 17/5/2015 11:52:28
 *  Author: Henning
 */ 

#ifndef ADC_H_
#define ADC_H_

// Initialize the ADC
void adc_init(void);

// Perform a read on the ADC. For ADC 5, channel = 5.
uint16_t adc_read(uint8_t channel);

// Get the previous ADC4 (Battery voltage) value
uint16_t getPreviousAdc4Value(void);

// Get the previous ADC8 (Battery voltage INNOVATOR) value
uint16_t getPreviousAdc8Value(void);

#endif /* ADC_H_ */