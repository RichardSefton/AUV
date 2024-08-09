/**
 * v2 based on the Psuedocode. 
 */

#define F_CPU 3333333UL
#include <avr/io.h>
#include <avr/iotn1627.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "TWI.h"

int plungerPos = 125;
int commandedPos = 0;

#define OUT 1
#define IN 2
int dir = IN;

/*
 * Lets apply the pins to some defines
 * 
 * 
 * 
 * StepperMotor
 * 17 - PC0
 * 18 - PC1
 * 19 - PC2
 * 20 - PC3
 * 
 * Going to use 6(PA5), 7(PA6). These need to be inputs to detect voltage
 * Will also need interrupts on these and pullup enabled because they're floating
 * 
 * Button we'll put on PB2
 * Also we'll add the pullup because while theres no voltage its also floating. 
*/
//PortC
#define STEP_PIN_1 PIN0_bm
#define STEP_PIN_2 PIN1_bm
#define STEP_PIN_3 PIN2_bm
#define STEP_PIN_4 PIN3_bm

//PortA
#define BUFFER_OUT_PIN PIN5_bm
#define BUFFER_IN_PIN PIN7_bm

//Functions we need to define. 
//This isn't in pseudocode. I like to include it to be explicit
void mainClkCtrl(void);
void stepper(int);
void setup(void);
void setupPins(void);
void setupRTC(void);
void enableRTC(void);
void disableRTC(void);

//My TWI Library requires callbacks. 
void I2C_RX_Callback(uint8_t);
uint8_t I2C_TX_Callback(void);
//Also need an address we can bind this module to. 
#define ADDR 0x08

int main(void) {
    setup();
    
    TWI_Slave_Init(ADDR, I2C_RX_Callback, I2C_TX_Callback);

    sei();
    
    int step = 0;
    while(1) {
        if (plungerPos != commandedPos) {
            if (dir == OUT && plungerPos != 255) {
                stepper(step);
                step--;
                 if (step < 0) {
                    step = 7;
                }
            } else if (dir == IN && plungerPos != 0) {
                stepper(step);
                step++;
                if (step > 7) {
                    step = 0;
                }
            }
            _delay_us(750);
        }
    }
    
    return 0;
}

void stepper(int step) {
    switch(step) {
        case 0:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3;
            PORTC.OUTSET |= STEP_PIN_4;
            break;     
        case 1:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2;
            PORTC.OUTSET |=  STEP_PIN_3 | STEP_PIN_4;
            break;    
        case 2:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_3;
            break;      
        case 3:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_2 | STEP_PIN_3;
            break;  
        case 4:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_3 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_2;
            break;    
        case 5:
            PORTC.OUTCLR |= STEP_PIN_3 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_1 | STEP_PIN_2;
            break;
        case 6:
            PORTC.OUTCLR |= STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_1;
            break;
        case 7:
            PORTC.OUTCLR |= STEP_PIN_2 | STEP_PIN_3;
            PORTC.OUTSET |= STEP_PIN_1 | STEP_PIN_4;
            break;
    }
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
}

void setupRTC(void) {
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; // Using internal 32.768 kHz oscillator
    RTC.CTRLA = RTC_PRESCALER_DIV64_gc;
    RTC.PER = 2560;
    RTC.INTCTRL |= RTC_OVF_bm;
}

void enableRTC(void) {
    RTC.CNT = 0;
    RTC.CTRLA |= RTC_RTCEN_bm;
}
void disableRTC(void) {
    RTC.CTRLA &= ~(RTC_RTCEN_bm);
}

void setupPins(void) {
    //Motor
    PORTC.DIR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
    
    //Buffers
    PORTA.DIR &= ~(BUFFER_OUT_PIN);
    PORTA.DIR &= ~(BUFFER_IN_PIN);
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc; 
}

void I2C_RX_Callback(uint8_t com) {
    // We're not actually using this. Just need the function for the SlaveInit function
    if (com > plungerPos) {
        dir = OUT;
    } else {
        dir = IN;
    }

    commandedPos = com;
}

uint8_t I2C_TX_Callback(void) {
    // We're not actually using this. Just need the function for the SlaveInit function
}

//ISRS
ISR(RTC_CNT_vect) {
    RTC.INTFLAGS = RTC_OVF_bm;
    if (dir == OUT) {
        plungerPos += 5;
    } else if (dir == IN) {
        plungerPos -= 5;
    }
}

ISR(PORTA_PORT_vect) {
    if (PORTA.INTFLAGS & BUFFER_OUT_PIN) {
        plungerPos = 255;
        dir = IN;
        PORTA.INTFLAGS |= BUFFER_OUT_PIN;
    }
    
    if (PORTA.INTFLAGS & BUFFER_IN_PIN) {
        plungerPos = 0;
        dir = OUT;
        PORTA.INTFLAGS |= BUFFER_IN_PIN;
    }
}