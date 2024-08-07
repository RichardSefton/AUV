/**
    Based on the v1 pseudocode
 */

#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 //#include <avr/iotn1627.h> 

#define ULTRASONIC_BOTTOM 0x08

#define DEPTH_CONTROLLER 0x4C

volatile int depth = 0;

#define LOWER 1

void setup(void);
void mainClkCtrl(void);
void setupRTC(void);
int ping(void);
void handleDistanceResponse(int, int);
void dive(uint8_t);
void raise(uint8_t);

#define BLOCK 1
#define NOBLOCK 0
uint8_t block = BLOCK;

int main() {
    TWI_Master_Init();
    
    _delay_ms(5000);

    //DIVE, DIVE, DIVE!
    dive(255);
    
    _delay_ms(60000);

//    cli();
//    setup();
//    sei();
    
    while(1) {

    }

    return 0;
}

void setup(void) {
    mainClkCtrl();
    setupRTC();
}

void mainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
    // F_CPU with this configuration will be 3.33MHz
}

void setupRTC(void) {
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc; // Using internal 32.768 kHz oscillator
    RTC.PER = 1024;
    RTC.INTCTRL |= RTC_OVF_bm;
    RTC.CTRLA |= RTC_RTCEN_bm;
}

int ping(void) {
    TWI_Master_Start(ULTRASONIC_BOTTOM, 0x01); //Start TWI to sonar module 
    int distance = TWI_Master_Read_NACK();
    TWI_Master_Stop();
    return distance;
}

void handleDistanceResponse(int distance, int direction) {
    switch (direction) {
        case LOWER: {
            if (depth == 0) {
                //Handle later
            } else {
                depth -= 10;
                raise(depth);
            }
        }

        default: { 
            return;
        }
    }
}

void dive(uint8_t d) {
    TWI_Master_Start((uint8_t)DEPTH_CONTROLLER, 0x00); //Write Command..
    TWI_Master_Write(d);
    TWI_Master_Stop();
}
void raise(uint8_t d) {
    TWI_Master_Start((uint8_t)DEPTH_CONTROLLER, 0x00);
    TWI_Master_Write(d);
    TWI_Master_Stop();
}

ISR(RTC_CNT_vect) {
    if (block == NOBLOCK) {
        int distance = ping();
        handleDistanceResponse(distance, LOWER);
    }
    
    RTC.INTFLAGS = RTC_OVF_bm;
}