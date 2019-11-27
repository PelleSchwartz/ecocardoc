//#include "header/init.h"
#include <avr/io.h>
#include <header/init.h>

/**
 * \brief Function to setup IO for terminal mode.
 *
 * Configure all I/O as input pull-up enabled to make sure all I/Os have
 * a defined level except LEDs (PB3:0) which is configured as outputs.
 * Light sensor and NTC sensor (PA6 and PA7): input pull-up disable.
 */
void IO_Init(void)
{
	// A0 = output, rest = input
	DDRA = 0b00000001;
	// B0-3 + B6 = input, rest = output
	DDRB = 0b10110000;
	// C all = input. REMEMBER TO DISABLE JTAG!!! STUPID F*CK :-)
	DDRC = 0b00000000;
	// D2 = input, rest = output
	DDRD = 0b11111011;
	
	//Pull ups
	PORTB |= (1 << PORTB5) | (1 << PORTB7);
	PORTD |= (1 << PORTD0) | (1 << PORTD1);
	
	
	// Enable pinchange interrupt on PCINT2-11 PCINT16-23
// 	PCICR = (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);
// 	PCMSK0 = (1 << PCINT7) | (1 << PCINT6) | (1 << PCINT5) | (1 << PCINT4) | (1 << PCINT3) | (1 << PCINT2);
// 	PCMSK1 = (1 << PCINT11) | (1 << PCINT10) | (1 << PCINT9) | (1 << PCINT8);
// 	PCMSK2 = (1 << PCINT23) | (1 << PCINT22) | (1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19) | (1 << PCINT18) | (1 << PCINT17) | (1 << PCINT16);
}


void SPI_MasterInit(void)
{
	//Make sure Power Save didn't turn SPI off
	//PRR0 &= ~(1 << PRSPI);
	/* Set MOSI and SCK output, all others input */
	//DDRB = (1<<DDB5)|(1<<DDB7);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);
}

void timer_init(void){
// 	// this code sets up timer1 for a 1ms  @ 16Mhz Clock (mode 4)
// 	OCR0A = 0x7C;// 	TCCR0B |= (1 << WGM02);// 	// Mode 4, CTC on OCR0A// 	TIMSK0 |= (1 << OCIE0A);// 	//Set interrupt on compare match// 	TCCR0B |= (1 << CS01) | (1 << CS00);// 	// set prescaler to 8 and start the timer
	// this code sets up timer1 for a 1ms  @ 16Mhz Clock (mode 4)
	OCR1A = 0x07CF;
	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR0A
	TIMSK1 |= (1 << OCIE1A);
	//Set interrupt on compare match
	TCCR1B |= (1 << CS11);
	// set prescaler to 8 and start the timer
}