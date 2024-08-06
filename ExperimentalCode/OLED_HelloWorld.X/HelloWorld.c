#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "OLED.h"
#include "TWI.h"

void mainClkCtrl(void);
void setup(void);
void setupRTC(void);
void setupPins(void);
void setupTCA(void);
void enableTCA(void);
void disableTCA(void);
void ping(void);
void setupTCB(void);
void enableTCB(void);
void disableTCB(void);

#define TRIGGER_PIN PIN0_bm
#define ECHO_PIN PIN1_bm

float distance = 0.0f;

int main(void) {
    setup();
    
//    TWI_Master_Init();
//
//    OLED_Init();
//    OLED_Clear();
//    OLED_SetCursor(0, 0);
//    OLED_PrintString("Loading...");
    
    sei();
    
    while (1) {
//        _delay_ms(500);
//        OLED_Clear();
//        OLED_SetCursor(0, 0);
//        OLED_PrintFloat(distance);  
    }

    return 0;
}

void mainClkCtrl(void) {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
}

void setupPins(void) {
    PORTC.DIR |= TRIGGER_PIN;
    PORTC.DIR &= ~(ECHO_PIN);
    
    PORTC.PIN1CTRL |= PORT_ISC_FALLING_gc | PORT_PULLUPEN_bm;
}

void setupRTC(void) {
    while (RTC.STATUS > 0) {} // Wait for all register to be synchronized
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc; // Using internal 32.768 kHz oscillator
    RTC.PER = 1024;
    RTC.INTCTRL |= RTC_OVF_bm;
    RTC.CTRLA |= RTC_RTCEN_bm;
}

void setupTCA(void) {
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.PER = 0xFFFF;
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

void setup(void) {
    mainClkCtrl();
    setupPins();
    setupRTC();
    setupTCA();
    setupTCB();
}

void calcDistance(float ticks) {
    float cpu = (float)F_CPU / 64.0f;
    float time = ticks / cpu;
    float sos = 34300.0f;
    distance = time * sos / 2.0f;
}

ISR(PORTC_PORT_vect) {
    if (PORTC.INTFLAGS & ECHO_PIN) {
        int ticks = TCA0.SINGLE.CNT;
        calcDistance((float) ticks);
        PORTC.INTFLAGS |= ECHO_PIN;
    }
}

ISR(TCB0_INT_vect) {
    disableTCB();
    PORTC.OUTCLR |= TRIGGER_PIN;        
    enableTCA();
    TCB0.INTFLAGS = TCB_CAPT_bm;
}

ISR(RTC_CNT_vect) {
    RTC.INTFLAGS = RTC_OVF_bm;
//    PORTC.OUTSET |= TRIGGER_PIN;
//    enableTCB();
}



