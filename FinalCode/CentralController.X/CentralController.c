/**
    Based on the v1 pseudocode
 */

#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "CWire.h"
#include "ShortTypes.h"
#include "AddressBook.h"
#include "Modules.h"
#include "Common.h"

u8 depth = 0;

void setup(void);
void mainClkCtrl(void);
void setupRTC(void);
u8 ping(void);
void handleDistanceResponse(u8);
u8 getDepth(void);
void dive(u8);
void raise(u8);

TwoWire twi0;
Distances distances;

int main() {
    setup();
    
    //Give other modules time to init
    RGB(RED);
    _delay_ms(2000);
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Master_begin(&twi0);
    
    //Wait for depth controller to enter home position.
    while(getDepth() != 1) {
        RGB(RED);
        _delay_ms(1000);
    }
    
    _delay_ms(2000);
    
    RGB(GREEN); 
    
    //DIVE, DIVE, DIVE!
    dive(0xFF);
   
    sei();
    
    while(1) {
        
    }

    return 0;
}

void setup(void) {
    mainClkCtrl();
    setupRTC();
    setupRGB();
}

void mainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
    // F_CPU with this configuration will be 3.33MHz
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

u8 ping(void) {
    //initiate sonar
    TwoWire_beginTransmission(&twi0, US_BOTTOM);
    TwoWire_write(&twi0, 0x50); //The arb value to initiate an actual ping
    TwoWire_endTransmission(&twi0, 1);
    _delay_ms(250); //Wait before responding
    
    //get the results
    u8 data = 0;
    TwoWire_requestFrom(&twi0, US_BOTTOM, 1, 1);
    if (TwoWire_available(&twi0)) {
        data = TwoWire_read(&twi0);  // Read the received byte
    }
    
    return data;
}

void handleDistanceResponse(u8 dir) {
    switch (dir) {
        case LOWER: {
            if (distances.lower < 50) {
                depth = getDepth();
                if (depth == 0) {
                //Handle later
                } else {
                    /**
                     * I think this is having the same ghandi bug from the civ games. 
                     * if depth is less than 10 and we take off 10 it does to 255 - depth - 10
                     * 
                     * If its less than 10 we just need to raise to 0. 
                     */
                    if (depth > 10) {
                        depth -= 10;
                        raise(depth);
                    } else if(depth == 1) {
                        raise(1); //not doing anything in reality. 
                    } else {
                        raise(0); //raise to 0 and it should 1 itself. 
                    }                    
                }
            }
        }

        default: { 
            return;
        }
    }
}

u8 getDepth(void) {
    RGB(BLUE);
    _delay_ms(100); //So we can see the xmission
    u8 data = 0;
    TwoWire_requestFrom(&twi0, DEPTH_CONTROLLER, 1, 1);
    if (TwoWire_available(&twi0)) {
        data = TwoWire_read(&twi0);  // Read the received byte
    }
    return data;
}

void dive(u8 d) {
    RGB(BLUE);
    TwoWire_beginTransmission(&twi0, DEPTH_CONTROLLER);
    TwoWire_write(&twi0, d);
    TwoWire_endTransmission(&twi0, 1);
    RGB(GREEN);
}
void raise(u8 d) {
    dive(d);
}

ISR(RTC_CNT_vect) {
    RGB(BLUE);
    RTC.INTFLAGS = RTC_OVF_bm;
    distances.lower = ping();
    RGB(GREEN);
    handleDistanceResponse(LOWER);
    RTC.INTFLAGS = RTC_OVF_bm;
}