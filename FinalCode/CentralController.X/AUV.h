/* 
 * File:   Modules.h
 * Author: richa
 *
 * Created on 18 August 2024, 17:39
 */

#ifndef MODULES_H
#define	MODULES_H

#include "ShortTypes.h"

typedef enum {
    FMD_FORWARD = 1,
    FMD_STOP = 2,
    FMD_BACKWARD = 3
} ForwardMotorDirection;

typedef enum {
    SMD_NONE = 0,
    SMD_LEFT = 1,
    SMD_RIGHT = 2
} SideMotorDirection;

typedef enum {
    SMS_ON = 1,
    SMS_OFF = 2
} SideMotorState;

typedef enum {
    SD_NONE = 0,
    SD_LOWER = 1,
    SD_FORWARD = 2,
    SD_LEFT = 3,
    SD_RIGHT = 4
} SonarDirection;

typedef enum {
    LOWER = 0,
    FORWARD = 1,
    LEFT = 2,
    RIGHT = 3
} SonarModule;

typedef struct {
    u16 distance;
    SonarDirection direction;
} Sonar;

typedef enum {
    LAND = 1,
    WATER = 2,
} SonarMode;

typedef struct {
    Sonar sonars [4];
    ForwardMotorDirection mainMotorDirection;
    SonarDirection mainMotorHeldBy;
    SideMotorDirection sideMotorDirection;
    SonarDirection sideMotorHeldBy;
    u8 sonarIndex;
    SonarMode sonarMode;
    u8 secondsSubmerged;
    u8 submergeTimer;
    u8 secondsSurfaced;
    u8 surfaceTimer;
} AUV;

void AUV_init(AUV*);
Sonar* AUV_loadSonar(AUV*, SonarModule);
void AUV_incrementSonarIndex(AUV*);
void AUV_setMotorDirection(AUV*, ForwardMotorDirection, SonarDirection);
void AUV_setTurnDirection(AUV*, SideMotorState, SonarDirection);
void AUV_setSonarMode(AUV*);
void AUV_increaseSubmergeTimer(AUV*);
u8 AUV_shouldSurface(AUV*);
void AUV_increaseSurfaceTimer(AUV*);
u8 AUV_shouldDive(AUV*);

#endif	/* MODULES_H */

