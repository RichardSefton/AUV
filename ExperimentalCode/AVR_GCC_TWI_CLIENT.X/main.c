#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Wire.h"

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setupPins(void);
void RGB(uint8_t);
void receiveEvent(int howMany);

int main() {
    setupPins();
    
    Wire_beginSlave(0x30);
    Wire_onReceive(receiveEvent);
    
    sei();
    
    while(1) {
        
    }
    
    return 0;
}

void setupPins(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
}

void RGB(uint8_t colour) {
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}

void receiveEvent(int howMany) {
    // Handle data received from master
    while (howMany--) {
        uint8_t received = Wire_read();
        RGB(received);
    }
}

