#define F_CPU 3333333UL

#include <stdio.h>
#include <stdlib.h>
#include "CWire.h"
#include "ShortTypes.h"
#include <util/delay.h>

#define MEGATINYCORE

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setupPins(void);
void RGB(u8);
void send(u8, u8);

TwoWire twi0;

int main(int argc, char** argv) {
    setupPins();
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Master_begin(&twi0);
    
    while(1) {
        RGB(RED);
        send(0x30, RED);
        _delay_ms(500);
        RGB(GREEN);
        _delay_ms(500);
        RGB(BLUE);
        send(0x30, BLUE);
        _delay_ms(500);
    }
    
    return (EXIT_SUCCESS);
}

void setupPins(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
    PORTB.OUTSET |= RED | GREEN | BLUE;
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