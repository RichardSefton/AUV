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

typedef struct {
    Sonar sonars [4];
    ForwardMotorDirection mainMotorDirection;
    SonarDirection mainMotorHeldBy;
    u8 sonarIndex;
} AUV;

void AUV_init(AUV*);
Sonar* AUV_loadSonar(AUV*, SonarModule);
void AUV_incrementSonarIndex(AUV*);
void AUV_setMotorDirection(AUV*, ForwardMotorDirection, SonarDirection);

#endif	/* MODULES_H */

