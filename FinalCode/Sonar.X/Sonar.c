#include "Sonar.h"

void Sonar_init(Sonar* self, Mode mode) {
    Sonar_setupSonar();
    Sonar_setupTCA();
    self->sos_land = 34300.0f;
    self->sos_water = 148000.0f;
    self->mode = mode;
}

void Sonar_setupSonar(void) {
    PORTC.DIR |= TRIGGER;
    PORTC.DIR &= ~ECHO;
    
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
}

void Sonar_setupTCA(void) {
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.PER = 0xFFFF;
}

void Sonar_enableTCA(void) {
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void Sonar_disableTCA(void) {
    TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm);
}

void Sonar_trigger(Sonar* self) {
    PORTC.OUTSET |= TRIGGER;
    _delay_us(10);
    PORTC.OUTCLR |= TRIGGER;
    //wait for echo to go high
    while((!(PORTC.IN & ECHO)));
    Sonar_enableTCA();
    //Wait for echo to go High
    while(PORTC.IN & ECHO);
    Sonar_disableTCA();
    self->ticks = TCA0.SINGLE.CNT;
    Sonar_calculateDistance(self);
}

void Sonar_calculateDistance(Sonar* self) {
    float cpu = (float)F_CPU / 64.0f; //Think this was using a prescaler of 64?
    float time = (float)self->ticks / cpu;
    float sos = 0.0f;
    if (self->mode == LAND) {
        sos = self->sos_land;
    } else {
        sos = self->sos_water;
    }
    self->distance.as_float = time * sos / 2.0f;
    Sonar_convertDistances(self);
}

void Sonar_convertDistances(Sonar* self) {
    //Should take the whole number of the float
    float fdist = self->distance.as_float;
    u16 u16dist = (u16)fdist;
    u8 u8dist = (u8)(u16dist / 100);
    
    self->distance.as_u16 = u16dist;
    self->distance.as_u8 = u8dist;
}
