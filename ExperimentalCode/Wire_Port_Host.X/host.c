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

#define RED2 PIN0_bm
#define GREEN2 PIN1_bm
#define BLUE2 PIN2_bm

void setupPins(void);
void setupRTC(void);
void RGB(u8);
void Read_RGB(u8);
void send(u8, u8);
u8 read(u8);

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
    
    PORTC.DIR |= RED2 | GREEN2 | BLUE2;
    PORTC.OUTSET |= RED2 | GREEN2 | BLUE2;
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
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}

void Read_RGB(u8 colour) {
    PORTC.OUTSET = RED2 | GREEN2 | BLUE2;
    PORTC.OUTCLR = colour;
}

void send(u8 addr, u8 data) {
    TwoWire_beginTransmission(&twi0, addr);
    TwoWire_write(&twi0, data);
    TwoWire_endTransmission(&twi0, 1);
}

u8 read(u8 addr) {
    u8 data = 0;
    TwoWire_requestFrom(&twi0, addr, 1, 1);
    if (TwoWire_available(&twi0)) {
        data = TwoWire_read(&twi0);  // Read the received byte
    }
    TwoWire_endTransmission(&twi0, 1);
    return data;
}

ISR(RTC_CNT_vect) {
    RTC.INTFLAGS = RTC_OVF_bm;
    RGB(colours[ind]);
    Read_RGB(RED2);
    
    //WRITE OPERATIONS
    if (colours[ind] != GREEN) {
        send(0x30, colours[ind]);
    }
    if (colours[ind] == GREEN) {
        send(0x40, GREEN);
    } else {
        send(0x40, RED);
    }
    
    //READ OPERATIONS
    if (colours[ind] == GREEN) {
        u8 data = read(0x30);
        if (data == GREEN) {
            data = GREEN2;
        }
        Read_RGB(data);
    }
    if (colours[ind] == BLUE) {
        u8 data = read(0x40);
        if (data == BLUE) {
            data = BLUE2;
        }
        Read_RGB(data);
    }
    
    ind++;
    if (ind > 2) {
        ind = 0;
    }
}