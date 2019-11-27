/*
 * tunes.c
 *
 * Created: 14/03/2017 15:20:42
 *  Author: Henning
 */ 
 
#include "Arduino.h"
#include "pins_arduino.h"

#define BUZZER_PIN 29

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

// Epic Sax melody
uint16_t sax_melody[] = {
	NOTE_B4, 0, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_B4,
	NOTE_B4, 0, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_B4,
	NOTE_B4, 0, NOTE_D5, NOTE_B4, 0, NOTE_A4,
	NOTE_G4, 0, NOTE_E4, NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_E4
};
// Epic Sax tempo
uint8_t sax_tempo[] = {
	5, 5, 10, 20, 20, 20, 8,
	5, 5, 10, 20, 20, 20, 8,
	5, 10, 5, 10, 10, 5,
	10, 10, 10, 10, 10, 10, 10
};
// Epic Sax timing [ms]
uint16_t sax_timing[] = {
	0, 200, 300, 350, 400, 450, 575, 775, 975, 1075, 1125, 1175, 1225, 1350, 1550, 1650, 1850, 1950, 2050, 2250, 2350, 2450, 2550, 2650, 2750, 2850, 2950
};

/* FUNCTIONS */
#define LOOP_TIME 10	// [ms]

void tunes_loop(void);

IntervalTimer tunes_timer;
volatile int tunes_progress = 0;
static int song = 0;
static int tunes_length = 0;
static uint32_t start_note_time = 0;
int next_tune_loop = 0;

void tunes_setup(void) {
	pinMode(BUZZER_PIN, OUTPUT);
}

void sing(int s) {
	song = s;
	// TODO Choose song
	tunes_length = sizeof(sax_melody) / sizeof(uint16_t);
	Serial.println(tunes_length);
	tunes_progress = 0;
	tunes_timer.begin(tunes_loop, LOOP_TIME * 1000);  // run every 0.010 seconds
	tunes_timer.priority(255);	// lowest priority
}

bool tunes_is_ready(void) {
	return tunes_progress <= 0;
}

void tunes_loop(void) {
	Serial.print("Progress: ");
	Serial.println(tunes_progress);
	if(tunes_progress < 0) return;
	else if(tunes_progress >= tunes_length) {
		noTone(BUZZER_PIN);
		tunes_progress = -1;
		tunes_timer.end();
	}
	Serial.print("Loop: ");
	Serial.println(next_tune_loop);
	next_tune_loop--;
	if(next_tune_loop <= 0) {
		int noteDuration = 1000/sax_tempo[tunes_progress];
		noTone(BUZZER_PIN);
		tone(BUZZER_PIN, sax_melody[tunes_progress], noteDuration);
		tunes_progress++;
		next_tune_loop = noteDuration * 2.30 / LOOP_TIME;
		Serial.print("Loops to wait: ");
		Serial.println(next_tune_loop);
	}
}