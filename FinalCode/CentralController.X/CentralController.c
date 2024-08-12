/**
    Based on the v1 pseudocode
 */

#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 //#include <avr/iotn1627.h> 

#define ULTRASONIC_BOTTOM 0x3C

#define DEPTH_CONTROLLER 0x4C

volatile uint8_t depth = 0;

#define LOWER 1

//Need an LED Status indicator. 
#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setup(void);
void mainClkCtrl(void);
void setupRTC(void);
void setupPins(void);
int ping(void);
void handleDistanceResponse(uint8_t, uint8_t);
uint8_t getDepth();
void dive(uint8_t);
void raise(uint8_t);
void rgb(uint8_t);

#define TRUE 0x01
#define FALSE 0x00

uint8_t initComplete = FALSE; 

int main() {
    setup();
    
    TWI_Master_Init(); 
//
//    //DIVE, DIVE, DIVE!
    dive(0xFF);
//    
    //initComplete = TRUE;
    
    while(1) {
        rgb(RED);
        _delay_ms(500);
        rgb(BLUE);
        _delay_ms(500);
        rgb(GREEN);
        _delay_ms(500);
    }

    return 0;
}

void setup(void) {
    mainClkCtrl();
    //setupRTC();
    setupPins();
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

void setupPins(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
}

int ping(void) {
    if (initComplete == TRUE) {
        TWI_Master_Start(ULTRASONIC_BOTTOM, 0x01); //Start TWI to sonar module 
        int distance = TWI_Master_Read_NACK();
        TWI_Master_Stop();
        return distance;
    } else {
        return 0;
    }
}

void handleDistanceResponse(uint8_t distance, uint8_t direction) {
    switch (direction) {
        case LOWER: {
            if (depth == 0) {
                //Handle later
            } else {
                depth = getDepth();
                depth -= 10;
                raise(depth);
            }
        }

        default: { 
            return;
        }
    }
}

uint8_t getDepth() {
    rgb(BLUE);
    TWI_Master_Start(DEPTH_CONTROLLER, 0x01); //Start TWI to sonar module 
    uint8_t d = TWI_Master_Read_NACK();
    TWI_Master_Stop();
    return d;
    rgb(GREEN);
}

void dive(uint8_t d) {
    rgb(RED | BLUE);
    TWI_Master_Start((uint8_t)DEPTH_CONTROLLER, 0x00); //Write Command..
    TWI_Master_Write(d);
    TWI_Master_Stop();
    rgb(GREEN);
}
void raise(uint8_t d) {
    dive(GREEN | BLUE);
    TWI_Master_Start((uint8_t)DEPTH_CONTROLLER, 0x00);
    TWI_Master_Write(d);
    TWI_Master_Stop();
    rgb(GREEN);
}

void rgb(uint8_t colour) {
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}

ISR(RTC_CNT_vect) {
    if (initComplete == TRUE) {
        rgb(RED | GREEN | BLUE);
        RTC.INTFLAGS = RTC_OVF_bm;
        uint8_t distance = ping();
        if (distance < 100 && distance > 0) {
            handleDistanceResponse(distance, LOWER);
        }    
    } else {
        RTC.INTFLAGS = RTC_OVF_bm;
    }
}