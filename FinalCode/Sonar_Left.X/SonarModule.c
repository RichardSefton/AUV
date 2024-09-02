/**
 * Based on the v2 pseudocode
 */
#include <stdint.h>
#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ShortTypes.h"
#include "CWire.h"
#include "Common.h"
#include "Sonar.h"

void setup(void);
void mainClkCtrl(void);

//TWI Library requires callbacks. 
void I2C_RX_Callback(u8);
void I2C_TX_Callback(void);
//Also need an address 
#define ADDR 0x32

TwoWire twi0;
Sonar sonar;

int main(void) {
    setup();
    RGB(RED);
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Slave_begin(&twi0, ADDR, 0, 0);
    
    TwoWire_onReceive(&twi0, I2C_RX_Callback);
    TwoWire_onRequest(&twi0, I2C_TX_Callback);
    
    sei();
    
    RGB(GREEN);
    
    while(1) {
        //Do nothing. 
    }
}

void setup(void) {
    mainClkCtrl();
    Sonar_init(&sonar, LAND);
    setupRGB();
}

void mainClkCtrl(void) {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
}

void I2C_RX_Callback(u8 val) {
    /**
     * So the i2c rail isn't waiting for a response and runs a risk of timing out:
     * 
     * We are going to trigger the ping from the master with a write command. 
     * Once triggered the master will wait (a time in ms, maybe 200ms) then 
     * perform a read request to get the results. 
     */
    if (val == 0x01) { //We're only expecting 1 byte
        while (TwoWire_available(&twi0)) {
            u8 data = TwoWire_read(&twi0);
            //arbitrary value just to prevent against accidental triggers from noise on the line
            if ((data == LAND) || (data == WATER)) { 
                if (data == LAND) {
                    sonar.mode = LAND;
                } else if (data == WATER) {
                    sonar.mode == WATER;
                }
                RGB(BLUE);
                Sonar_trigger(&sonar);
            }
        }
    }
}

//By the point this is called the distance calculations should have occurred.
void I2C_TX_Callback(void) {
    //sending it back as the u16 value. Should really do this in the library but lets just check it works first. 
    u8 high = sonar.distance.as_u16 >> 8;
    u8 low = sonar.distance.as_u16;
    u8 dataToSend [2] = { low, high };
    TwoWire_writeBytes(&twi0, dataToSend, 2);
    RGB(GREEN);
}