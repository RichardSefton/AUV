/**
 * v2 based on the Psuedocode. 
 */

#define F_CPU 3333333UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "CWire.h"
#include "ShortTypes.h"
#include "Common.h"
#include "StepperMotor.h"

u8 plungerPos = 255;
u8 commandedPos = 0;

#define THREAD_OUT 0x01
#define THREAD_IN 0x02
#define STOP 0x00
#define GO 0x01

u8 dir = THREAD_IN;
u8 run = GO;

//PortA
#define BUFFER_OUT_PIN PIN5_bm
#define BUFFER_IN_PIN PIN7_bm

//Functions we need to define. 
//This isn't in pseudocode. I like to include it to be explicit
void mainClkCtrl(void);
void setup(void);
void setupPins(void);
void setupRTC(void);

//My TWI Library requires callbacks. 
void I2C_RX_Callback(u8);
void I2C_TX_Callback(void);
//Also need an address we can bind this module to. 

TwoWire twi0;
StepperMotor stepper;

#define ADDR 0x40

int main(void) {
    setup();
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Slave_begin(&twi0, ADDR, 0, 0);
    
    TwoWire_onReceive(&twi0, I2C_RX_Callback);
    TwoWire_onRequest(&twi0, I2C_TX_Callback);
    
    sei();
    
    while(1) {
        if (plungerPos != commandedPos) {
            run = GO;
            RGB(BLUE);
            if ((dir == THREAD_OUT && plungerPos != 254) && plungerPos != commandedPos){
                StepperMotor_step(&stepper);
                StepperMotor_decreaseStep(&stepper);
            } else if ((dir == THREAD_IN && plungerPos != 1) && plungerPos != commandedPos) {
                StepperMotor_step(&stepper);
                StepperMotor_increaseStep(&stepper);
            }
            _delay_us(750);
        } else {
            run = STOP;
            RGB(GREEN);
            StepperMotor_allStop();
        }
    }
    
    return 0;
}

void mainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
}

void setup(void) {
    mainClkCtrl();
    setupRTC();
    setupPins();
    StepperMotor_init(&stepper);
    setupRGB();
}

void setupRTC(void) {
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    while(RTC.STATUS);
    RTC.CTRLA |= RTC_PRESCALER_DIV1_gc;
    /**
     * End to end takes 2min 25s/145s
     * 
     * Going to knock off 5s so we should be clear of the buffer. 
     * 
     * 145/255 = 0.57 and change. So one tick of the pos is worth ~0.57s 
     */
    RTC.PER = 582; //Needed to add 12 as 1k is 1024 kHz
    while (RTC.STATUS);
    RTC.INTFLAGS |= RTC_OVF_bm;
    RTC.INTCTRL |= RTC_OVF_bm;
    while (RTC.STATUS);
    RTC.CTRLA |= RTC_RTCEN_bm;
    while (RTC.STATUS);
}

void setupPins(void) {
    //Buffers
    PORTA.DIR &= ~(BUFFER_OUT_PIN);
    PORTA.DIR &= ~(BUFFER_IN_PIN);
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc; 
}

void I2C_RX_Callback(u8 nBytes) {
    u8 data = 0;
    while (TwoWire_available(&twi0)) {
        data = TwoWire_read(&twi0);
    }
    
    if (data > plungerPos) {
        dir = THREAD_OUT;
    } else {
        dir = THREAD_IN;
    }

    commandedPos = data;
}

void I2C_TX_Callback(void) {
    //This is used in read requests where the controller is expecting the depth.
    
    //without getting the current depth we would risk bottoming out particularly 
    //in the first call to dive which sends to max depth. 
    TwoWire_write(&twi0, plungerPos);
}

//ISRS
ISR(RTC_CNT_vect) {
    RTC.INTFLAGS = RTC_OVF_bm;
    if (run == GO) {
        if (dir == THREAD_OUT) {
            plungerPos += 1;
        } else if (dir == THREAD_IN) {
            plungerPos -= 1;
        }
    } 
    //Incase we naturally get there without using the buffer. We don't want to sit at 0
//    if (plungerPos == 1) {
//        commandedPos = 1;
//        dir = THREAD_IN;
//    } else if (plungerPos == 254 && commandedPos > 0) {
//        commandedPos = 254;
//        dir = THREAD_OUT;
//    }
}

ISR(PORTA_PORT_vect) {
    if (PORTA.INTFLAGS & BUFFER_OUT_PIN) {
        if (!(PORTA.IN & BUFFER_OUT_PIN)) {
            RTC.CNT = 0;
            plungerPos = 255;
            commandedPos = (254);
            dir = THREAD_IN;
        }
        PORTA.INTFLAGS |= BUFFER_OUT_PIN;
    }
    
    if (PORTA.INTFLAGS & BUFFER_IN_PIN) {
        if (!(PORTA.IN & BUFFER_IN_PIN)) {
            RTC.CNT = 0;
            plungerPos = 0;
            commandedPos = 1;
            dir = THREAD_OUT; 
        }
        PORTA.INTFLAGS |= BUFFER_IN_PIN;
    }
}