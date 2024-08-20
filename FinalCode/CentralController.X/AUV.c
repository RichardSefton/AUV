
#include "AUV.h"

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
