/*
 * tunes.h
 *
 * Created: 11/06/2016 17:35:27
 *  Author: Henning
 */ 

#ifndef TUNES_H_
#define TUNES_H_

#define STARTUP_MELODY_ID 4
#define STARTUP_MELODY2_ID 6
#define UNDERWORLD_MELODY_ID 2
#define MARIO_MAIN_MELODY 1
#define WARNING_MELODY_ID 5
#define CANTINA_MELODY_ID 7
#define RIVER_MELODY_ID 8
#define SAX_MELODY_ID 9

void tunes_init(void);
void sing(int s);
void buzz(int targetPin, long frequency, long length);

#endif /* TUNES_H_ */