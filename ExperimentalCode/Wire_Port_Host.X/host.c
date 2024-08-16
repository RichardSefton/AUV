#define F_CPU 3333333UL

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ShortTypes.h"
#include "CWire.h"

#define MEGATINYCORE

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setupPins(void);
void setupRTC(void);
void RGB(u8);
void send(u8, u8);

TwoWire twi0;

u8 colours[3] = { RED, GREEN, BLUE };
u8 ind = 0;

int main(int argc, char** argv) {
    setupPins();
    setupRTC();
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Master_begin(&twi0);
    
    sei();
    
    while(1) {

    }
    
    return (EXIT_SUCCESS);
}

void setupPins(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
    PORTB.OUTSET |= RED | GREEN | BLUE;
}

void setupRTC(void) {
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    while(RTC.STATUS);
    RTC.CTRLA |= RTC_PRESCALER_DIV1_gc;
    RTC.PER = 1024;
    while (RTC.STATUS);
    RTC.INTFLAGS |= RTC_OVF_bm;
    RTC.INTCTRL |= RTC_OVF_bm;
    while (RTC.STATUS);
    RTC.CTRLA |= RTC_RTCEN_bm;
    while (RTC.STATUS);
}

void RGB(u8 colour) {
    PORTB.OUTSET |= RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}

void send(u8 addr, u8 data) {
    TwoWire_beginTransmission(&twi0, addr);
    TwoWire_write(&twi0, data);
    TwoWire_endTransmission(&twi0, 1);
}

ISR(RTC_CNT_vect) {
    RTC.INTFLAGS = RTC_OVF_bm;
    RGB(colours[ind]);
    
    if (colours[ind] != GREEN) {
        send(0x30, colours[ind]);
    }
    
    ind++;
    if (ind > 2) {
        ind = 0;
    }
}