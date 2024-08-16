#define F_CPU 3333333UL

#include <avr/io.h>
#include <stdlib.h>
#include "CWire.h"
#include "ShortTypes.h"
#include <avr/interrupt.h>

#define MEGATINYCORE

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setupPins(void);
void RGB(u8);
void onReceive(u8);
void onRequest(void);

TwoWire twi0;

int main(int argc, char** argv) {
    setupPins();
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Slave_begin(&twi0, 0x40, 0, 0);
    
    TwoWire_onReceive(&twi0, onReceive);
    TwoWire_onRequest(&twi0, onRequest);
    
    sei();
    
    while(1) {

    }
    
    return (EXIT_SUCCESS);
}

void setupPins(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
    PORTB.OUTSET |= RED | GREEN | BLUE;
}

void RGB(u8 colour) {
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}

void onReceive(u8 numBytes) {
    while (TwoWire_available(&twi0)) {
        u8 data = TwoWire_read(&twi0);
        RGB(data);
    }
}

void onRequest(void) {
    TwoWire_write(&twi0, BLUE);
}