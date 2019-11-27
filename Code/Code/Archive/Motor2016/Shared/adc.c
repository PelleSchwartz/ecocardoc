/*
 * adc.c
 *
 * Created: 17/5/2015 11:30:44
 *  Author: Henning
 */ 

#include "global.h"

///////////////////////////////
//       ADC FUNCTIONS       //
///////////////////////////////

static uint16_t adc4 = 0;
static uint16_t adc8 = 0;

// Initialize the ADC
void adc_init(void) {
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));	// ADC prescaler to 128
	ADMUX |= (1<<REFS0);                // Voltage reference from Avcc (5V)
	ADCSRA |= (1<<ADEN);                // ADC Enable
	ADCSRA |= (1<<ADSC);                // ADC Start Conversion
}

// Perform a read on the ADC. For ADC 5, channel = 5.
uint16_t adc_read(uint8_t channel) {	
	if(channel < 8) {
		ADMUX &= 0xE0;					// Clear the older channel that was read
		ADMUX |= channel;				// Defines the new ADC channel to be read
		ADCSRB &= ~(1<<MUX5);			// Clear MUX5 bit if channel <= 7
		ADCSRA |= (1<<ADSC);			// Starts a new conversion
		while(ADCSRA & (1<<ADSC));		// Wait until the conversion is done
	}
	else {
		ADMUX &= 0xE0;					// Clear the older channel that was read
		ADMUX |= (channel-8);			// Defines the new ADC channel to be read
		ADCSRB |= (1<<MUX5);			// Set MUX5 bit if channel > 7
		ADCSRA |= (1<<ADSC);			// Starts a new conversion
		while(ADCSRA & (1<<ADSC));		// Wait until the conversion is done
	}
	uint16_t adc_value = ADCW;
	if(channel == 4) adc4 = adc_value;	// Save the value for future use if it is the battery voltage (DYNAMO)
	else if(channel == 8) adc8 = adc_value;	// Save the value for future use if it is the battery voltage (INNOVATOR)
	return adc_value;					// Returns the ADC value of the chosen channel
}

// Get the previous ADC4 (Battery voltage DYNAMO) value
uint16_t getPreviousAdc4Value(void) {
	return adc4;
}

// Get the previous ADC8 (Battery voltage INNOVATOR) value
uint16_t getPreviousAdc8Value(void) {
	return adc8;
}

///////////////////////////////
//   END OF ADC FUNCTIONS    //
///////////////////////////////