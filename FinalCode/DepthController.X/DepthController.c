/**
 * v2 based on the Psuedocode. 
 */

#define F_CPU 3333333UL
#include <avr/io.h>
// #include <avr/iotn1627.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "TWI.h"

volatile uint8_t plungerPos = 255;
volatile uint8_t commandedPos = 0;

#define THREAD_OUT 0x01
#define THREAD_IN 0x02
#define STOP 0x00
#define GO 0x01

volatile uint8_t dir = THREAD_IN;
volatile uint8_t run = GO;
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

//Need an LED Status indicator. 
#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

//Functions we need to define. 
//This isn't in pseudocode. I like to include it to be explicit
void mainClkCtrl(void);
void stepper(uint8_t);
void allStop(void);
void setup(void);
void setupPins(void);
void setupRTC(void);
void rgb(uint8_t);

//My TWI Library requires callbacks. 
void I2C_RX_Callback(uint8_t);
uint8_t I2C_TX_Callback(void);
//Also need an address we can bind this module to. 
#define ADDR 0x4C

int main(void) {
    setup();
    TWI_Slave_Init(ADDR, I2C_RX_Callback, I2C_TX_Callback);
    
    int step = 0;
    while(1) {
        if (plungerPos != commandedPos) {
            run = GO;
            rgb(BLUE);
            if ((dir == THREAD_OUT && plungerPos != 254) && plungerPos != commandedPos){
                stepper(step);
                step--;
                 if (step < 0) {
                    step = 7;
                }
            } else if ((dir == THREAD_IN && plungerPos != 1) && plungerPos != commandedPos) {
                stepper(step);
                step++;
                if (step > 7) {
                    step = 0;
                }
            }
            _delay_us(750);
        } else {
            run = STOP;
            rgb(GREEN);
            allStop();
        }
    }
    
    return 0;
}

void stepper(uint8_t step) {
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
        default:
            break;
    }
}

void allStop(void) {
    PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
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
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    while(RTC.STATUS);
    RTC.CTRLA |= RTC_PRESCALER_DIV1_gc;
    /**
     * End to end takes 2min 39s/159s
     * 
     * 255/159 = 1.6035 and change. So one tick of the pos is worth ~1.6s
     */
    RTC.PER = 1624;
    while (RTC.STATUS);
    RTC.INTFLAGS |= RTC_OVF_bm;
    RTC.INTCTRL |= RTC_OVF_bm;
    while (RTC.STATUS);
    RTC.CTRLA |= RTC_RTCEN_bm;
    while (RTC.STATUS);
}

void setupPins(void) {
    //Motor
    PORTC.DIR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
    
    //Buffers
    PORTA.DIR &= ~(BUFFER_OUT_PIN);
    PORTA.DIR &= ~(BUFFER_IN_PIN);
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm | PORT_ISC_RISING_gc; 
    
    PORTB.DIR |= RED | GREEN | BLUE;
}

void I2C_RX_Callback(uint8_t com) {
    if (com > plungerPos) {
        dir = THREAD_OUT;
    } else {
        dir = THREAD_IN;
    }

    commandedPos = com;
}

uint8_t I2C_TX_Callback(void) {
    //This is used in read requests where the controller is expecting the depth.
    
    //without getting the current depth we would risk bottoming out particularly 
    //in the first call to dive which sends to max depth. 
    rgb(RED | GREEN);
    return plungerPos;
}

void rgb(uint8_t colour) {
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
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