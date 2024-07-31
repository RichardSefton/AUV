/**
 * Based on the v1 pseudocode
 */
#define F_CPU 3333333UL

#include <avr/io.h>
// #include <avr/iotn1627.h>

#include <util/delay.h>
#include <avr/interrupt.h>

float distance = 0.0;

#define TRIGGER_PIN PIN0_bm
#define ECHO_PIN_RISING PIN1_bm
#define ECHO_PIN_FALLING PIN2_bm
#define BUTTON_PIN PIN3_bm

void setup(void);
void mainClkCtrl(void);
void setupPins(void);
void setupTCA(void);
void enableTCA(void);
void disableTCA(void);
void setupTCB(void);
void enableTCB(void);
void disableTCB(void);
void triggerSonar(void);
void calcDistance(float);


int main(void) {
    setup();
    
    sei();
    
    while(1) {
        //Do nothing. 
    }
}

void setup(void) {
    mainClkCtrl();
    setupPins();
    setupTCA();
    setupTCB();
}

void mainClkCtrl(void) {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
}

void setupPins(void) {
    PORTC.DIR |= TRIGGER_PIN;
    PORTC.DIR &= ~(ECHO_PIN_RISING | ECHO_PIN_FALLING | BUTTON_PIN);
    
    PORTC.PIN1CTRL |= PORT_ISC_RISING_gc | PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
    
    PORTC.PIN3CTRL |= PORT_ISC_RISING_gc | PORT_PULLUPEN_bm;
}

void setupTCA(void) {
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.PER = 0xFFFF;
    TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
}
void enableTCA(void) {
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}
void disableTCA(void) {
    TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm);
}

void setupTCB(void) {
    TCB0.CTRLA |= TCB_CLKSEL_DIV2_gc;  // Clock / 2
    uint16_t period = (F_CPU / 2) * 10 / 1000000;
    TCB0.CCMP = period;
    TCB0.CTRLB |= TCB_CNTMODE_INT_gc;
    TCB0.INTCTRL |= TCB_CAPT_bm;
}
void enableTCB(void) {
    TCB0.CNT = 0;
    TCB0.CTRLA |= TCB_ENABLE_bm;
}
void disableTCB(void) {
    TCB0.CTRLA &= ~(TCB_ENABLE_bm);
}

void triggerSonar(void) {
    PORTC.OUTSET |= TRIGGER_PIN;
    enableTCB();
}

void calcDistance(float ticks) {
    float cpu = (float)F_CPU / 64.0f;
    float time = ticks / cpu;
    float sos = 34300.0f;
    distance = time * sos / 2.0f;
}

ISR(PORTC_PORT_vect) {
    if (PORTC.INTFLAGS & ECHO_PIN_RISING) {
        enableTCA();
        PORTC.INTFLAGS |= ECHO_PIN_RISING;
    }
    
    if (PORTC.INTFLAGS & ECHO_PIN_FALLING) {
        disableTCA();
        uint16_t ticks = TCA0.SINGLE.CNT;
        calcDistance((float)ticks);
        PORTC.INTFLAGS |= ECHO_PIN_FALLING;
    }
    
    if (PORTC.INTFLAGS & BUTTON_PIN) {
        triggerSonar();
        PORTC.INTFLAGS = BUTTON_PIN;
    }
}

ISR(TCB0_INT_vect) {
    disableTCB();
    PORTC.OUTCLR |= TRIGGER_PIN;
    TCB0.INTFLAGS = TCB_CAPT_bm;
}