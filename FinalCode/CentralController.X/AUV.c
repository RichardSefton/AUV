
#include "AUV.h"
#include <avr/io.h>

void AUV_init(AUV* self) {
    Sonar s1, s2, s3, s4;
    s1.direction = SD_LOWER;
    s2.direction = SD_FORWARD;
    s3.direction = SD_LEFT;
    s4.direction = SD_RIGHT;
    self->sonars[0] = s1;
    self->sonars[1] = s2;
    self->sonars[2] = s3;
    self->sonars[3] = s4;
    self->sonarIndex = 0;
    self->mainMotorDirection = FMD_STOP;
    self->mainMotorHeldBy = SD_NONE;
    self->sideMotorDirection = SMD_NONE;
    self->sideMotorHeldBy = SD_NONE;
    self->submergeTimer = 120;
    self->surfaceTimer = 30;
    
    //Using pins for mode detection. Votage will mean we're in water mode. 
    //PA5(Pin 6) Will be the voltage (OUTPUT)
    PORTA.DIR |= PIN5_bm;
    PORTA.OUT |= PIN5_bm;
    //PA7(Pin 8) will be the detection pin. If high, we're in Water mode. 
    PORTA.DIR &= ~(PIN7_bm);
}

Sonar* AUV_loadSonar(AUV* self, SonarModule module) {
    return &self->sonars[module];
}

void AUV_incrementSonarIndex(AUV* self) {
    self->sonarIndex++;
    if (self->sonarIndex > 3) {
        self->sonarIndex = 0;
    }
}

void AUV_setMotorDirection(AUV* self, ForwardMotorDirection dir, SonarDirection hold) {
    self->mainMotorDirection = dir;
    self->mainMotorHeldBy = hold;
}

void AUV_setTurnDirection(AUV* self, SideMotorState dir, SonarDirection hold) {
    self->sideMotorDirection = dir;
    self->sideMotorHeldBy = hold;
}

void AUV_setSonarMode(AUV* self) {
    //Set to land mode. 
    self->sonarMode = LAND;
    //If theres voltage on the detection pin, we're in water mode. 
    if (PORTA.IN & PIN7_bm) {
        self->sonarMode = WATER;
    }
}

void AUV_increaseSubmergeTimer(AUV* self) {
    self->secondsSubmerged++;
}

u8 AUV_shouldSurface(AUV* self) {
    if (self->secondsSubmerged >= self->submergeTimer) {
        self->secondsSubmerged = 0;
        return 1;
    }
    return 0;
}

void AUV_increaseSurfaceTimer(AUV* self) {
    self->secondsSurfaced++;
}

u8 AUV_shouldDive(AUV* self) {
    if (self->secondsSurfaced >= self->surfaceTimer) {
        self->secondsSurfaced = 0;
        return 1;
    }
    return 0;
}