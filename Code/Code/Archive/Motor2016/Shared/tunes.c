/*
Arduino Mario Bros Tunes
With Piezo Buzzer and PWM

Connect the positive side of the Buzzer to pin 3,
then the negative side to a 1k ohm resistor. Connect
the other side of the 1 k ohm resistor to
ground(GND) pin on the Arduino.

by: Dipto Pratyaksa
last updated: 31/3/13
*/

#include "global.h"
#include "tunes.h"
#include "LEDs.h"
#include "delay.h"

/*************************************************
* Public Constants
*************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  205
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 2061
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define melodyPin 3
// Startup sound
int warning_melody[] = {
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4
};
int warning_tempo[] = {
	12, 12, 12, 12,
	12, 12
};
// Startup melody "Super Mario"
int startup_melody[] = {
	NOTE_E7, NOTE_E7, 0, NOTE_E7,
	0, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0
};
int startup_tempo[] = {
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12
};
// Startup melody2 "Imperial March"
int startup_melody2[] = {
	NOTE_G5, NOTE_G5, NOTE_G5,
	NOTE_DS5, NOTE_AS5, NOTE_G5, NOTE_DS5,
	NOTE_AS5, NOTE_G5
};
int startup_tempo2[] = {
	6, 6, 6,
	9, 12, 6, 9,
	12, 3
};
// Startup melody3 "Der er et yndigt land"
int startup_melody3[] = {
	NOTE_D7, NOTE_A7, NOTE_A7, NOTE_F7,
	NOTE_D7, NOTE_B7
};
int startup_tempo3[] = {
	8, 6, 8, 6,
	6, 3
};
// Mario main theme melody
int melody[] = {
	NOTE_E7, NOTE_E7, 0, NOTE_E7,
	0, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0,
	
	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,
	
	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0,
	
	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,
	
	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0
};
// Mario main them tempo
int tempo[] = {
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
};
// Underworld melody
int underworld_melody[] = {
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0, NOTE_DS4, NOTE_CS4, NOTE_D4,
	NOTE_CS4, NOTE_DS4,
	NOTE_DS4, NOTE_GS3,
	NOTE_G3, NOTE_CS4,
	NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
	NOTE_GS4, NOTE_DS4, NOTE_B3,
	NOTE_AS3, NOTE_A3, NOTE_GS3,
	0, 0, 0
};
// Underworld tempo
int underworld_tempo[] = {
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	6, 18, 18, 18,
	6, 6,
	6, 6,
	6, 6,
	18, 18, 18, 18, 18, 18,
	10, 10, 10,
	10, 10, 10,
	3, 3, 3
};
// Cantina melody
int cantina_melody[] = {
	NOTE_A4, NOTE_D5, NOTE_A4, NOTE_D5, NOTE_A4, NOTE_D5,
	NOTE_A4, NOTE_GS4, NOTE_A4,
	NOTE_A4, NOTE_GS4, NOTE_A4, NOTE_G4, 0,
	NOTE_FS4, NOTE_G4, NOTE_FS4,
	NOTE_F4, NOTE_D4, 0	
};
// Cantina tempo
int cantina_tempo[] = {
	12, 12, 12, 12, 12, 12, 
	24, 24, 12, 
	24, 24, 24, 24, 24, 
	24, 24, 24, 
	6, 12, 12
};
// River melody
int river_melody[] = {
	NOTE_A4, NOTE_GS4, NOTE_A4, NOTE_GS4,
	NOTE_A4, NOTE_E4, NOTE_A4, NOTE_D4,
	NOTE_A3, NOTE_CS3
};
// River tempo
int river_tempo[] = {
	6, 6, 6, 6,
	6, 6, 6, 3,
	12, 12
};
// Epic Sax melody
int sax_melody[] = {
	NOTE_B4, 0, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_B4,
	NOTE_B4, 0, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_B4,
	NOTE_B4, 0, NOTE_D5, NOTE_B4, 0, NOTE_A4, 
	NOTE_G4, 0, NOTE_E4, NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_E4
};
// Epic Sax tempo
int sax_tempo[] = {
	5, 5, 10, 20, 20, 20, 8,
	5, 5, 10, 20, 20, 20, 8,
	5, 10, 5, 10, 10, 5,
	10, 10, 10, 10, 10, 10, 10
};

/* FUNCTIONS */

static int song = 0;

void tunes_init(void) {
	DDRJ |= (1 << DDJ6);
}

void sing(int s) {
	// iterate over the notes of the melody:
	song = s;
	if(song == STARTUP_MELODY_ID) {
		int size = sizeof(startup_melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / startup_tempo[thisNote];
			
			buzz(melodyPin, startup_melody[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
		}
	}
	else if(song == STARTUP_MELODY2_ID) {
		int size = sizeof(startup_melody2) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / startup_tempo2[thisNote];
			
			buzz(melodyPin, startup_melody2[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
		}
	}
	else if(song == WARNING_MELODY_ID) {
		int size = sizeof(warning_melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / warning_tempo[thisNote];
			
			buzz(melodyPin, warning_melody[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
		}
	}
	else if(song == UNDERWORLD_MELODY_ID) {
		int size = sizeof(underworld_melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / underworld_tempo[thisNote];
			
			buzz(melodyPin, underworld_melody[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
		}
	}
	else if(song == MARIO_MAIN_MELODY) {
		int size = sizeof(melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / tempo[thisNote];
			
			buzz(melodyPin, melody[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
			
		}
	} 
	else if(song == CANTINA_MELODY_ID) {
		int size = sizeof(cantina_melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / cantina_tempo[thisNote];
		
			buzz(melodyPin, cantina_melody[thisNote], noteDuration);
		
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
		
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
		
		}
	}
	else if(song == RIVER_MELODY_ID) {
		int size = sizeof(river_melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / river_tempo[thisNote];
			
			buzz(melodyPin, river_melody[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
			
		}
	}
	else if(song == SAX_MELODY_ID) {
		int size = sizeof(sax_melody) / sizeof(int);
		for (int thisNote = 0; thisNote < size; thisNote++) {
			// to calculate the note duration, take one second
			// divided by the note type.
			//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
			int noteDuration = 1000 / sax_tempo[thisNote];
			
			buzz(melodyPin, sax_melody[thisNote], noteDuration);
			
			// to distinguish the notes, set a minimum time between them.
			// the note's duration + 30% seems to work well:
			int pauseBetweenNotes = noteDuration * 1.30;
			delay_ms(pauseBetweenNotes);
			
			// stop the tone playing:
			buzz(melodyPin, 0, noteDuration);
			
		}
	}
}

void buzz(int targetPin, long frequency, long length) {
	LED1On();
	long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
	//// 1 second's worth of microseconds, divided by the frequency, then split in half since
	//// there are two phases to each cycle
	long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
	//// multiply frequency, which is really cycles per second, by the number of seconds to
	//// get the total number of cycles to produce
	for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
		PORTJ |= (1 << PINJ6); // write the buzzer pin high to push out the diaphram
		delay_us(delayValue); // wait for the calculated delay value
		PORTJ &= ~(1 << PINJ6); // write the buzzer pin low to pull back the diaphram
		delay_us(delayValue); // wait again for the calculated delay value
	}
	LED1Off();
}
