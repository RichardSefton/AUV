#ifndef SONAR_H
#define	SONAR_H

/**
 Because the Sonar Module is infact multiple modules lets make it a library project 
 * and call it in when needed. 
 */

#ifndef F_CPU
#define F_CPU 3333333UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "Common.h"

//typedef uint8_t u8;
//typedef uint16_t u16;

extern enum {
    TRIGGER = PIN3_bm,
    ECHO = PIN2_bm
} S_Pins;

typedef enum {
    LAND = 0,
    WATER = 1,
} Mode;

typedef struct {
    u8 as_u8;
    u16 as_u16;
    float as_float;
} Distance;

typedef struct {
    Mode mode;
    u16 ticks;
    float sos_land;
    float sos_water;
    Distance distance;
} Sonar;

void Sonar_init(Sonar*, Mode);
void Sonar_setupSonar(void);
void Sonar_setupTCA(void);
void Sonar_enableTCA(void);
void Sonar_disableTCA(void);
void Sonar_trigger(Sonar*);
void Sonar_calculateDistance(Sonar*);
void Sonar_convertDistances(Sonar*);

#endif	/* SONAR_H */

