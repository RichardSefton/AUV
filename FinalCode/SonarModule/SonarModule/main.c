/**
 * Based on the v2 pseudocode
 */
#include <stdint.h>
#define F_CPU 3333333UL

#include <avr/io.h>
// #include <avr/iotn1627.h>

#include <util/delay.h>
#include <avr/interrupt.h>

// #include "TWI.h"

float distance = 0.0;

#define TRIGGER_PIN PIN0_bm
#define ECHO_PIN PIN1_bm

void setup(void);
void mainClkCtrl(void);
void setupPins(void);
void setupTCA(void);
void enableTCA(void);
void disableTCA(void);
void triggerSonar(void);
void calcDistance(float);

//TWI Library requires callbacks. 
void I2C_RX_Callback(uint8_t);
uint8_t I2C_TX_Callback(void);
//Also need an address 
#define ADDR 0x09

int main(void) {
    setup();

    TWI_Slave_Init(ADDR, I2C_RX_Callback, I2C_TX_Callback);

    sei();
    
    while(1) {
        //Do nothing. 
    }
}

void setup(void) {
    mainClkCtrl();
    setupPins();
    setupTCA();
}

void mainClkCtrl(void) {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
}

void setupPins(void) {
    PORTC.DIR |= TRIGGER_PIN;
    PORTC.DIR &= ~(ECHO_PIN);
    
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;
    
    PORTC.PIN3CTRL |= PORT_ISC_RISING_gc | PORT_PULLUPEN_bm;
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

void triggerSonar(void) {
    PORTC.OUTSET |= TRIGGER_PIN;
    _delay_us(10);
    PORTC.OUTCLR |= TRIGGER_PIN;
}

void calcDistance(float ticks) {
    float cpu = (float)F_CPU / 64.0f;
    float time = ticks / cpu;
    float sos = 34300.0f;
    distance = time * sos / 2.0f;
}

void I2C_RX_Callback(uint8_t val) {
    //This one doesn't do anything. We receive a command and send a distance. 
    //There is no need to use any data passed to the request
}
//The response requires blocking code as we can't just sit around and wait for the calculations 
//to drop. We need to sequentially do everything in this function
uint8_t I2C_TX_Callback(void) {
    triggerSonar();
    //wait for echo to go high
    while((!(PORTC.IN & ECHO_PIN)));
    enableTCA();
    //Wait for echo to go High
    while(PORTC.IN & ECHO_PIN);
    disableTCA();
    uint16_t ticks = TCA0.SINGLE.CNT;
    calcDistance((float)ticks);
    return (uint8_t)distance;
}