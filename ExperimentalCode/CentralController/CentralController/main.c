/**
    Based on the v1 pseudocode
 */

 #include <avr/io.h>
 //#include <avr/iotn1627.h>

 #include "TWI.h"

 #define ULTRASONIC_BOTTOM 0x08

 #define DEPTH_CONTROLLER 0x09

 int depth = 0;

 #define LOWER 1

void setup(void);
void mainClkCtrl(void);
void setupRTC(void);
int ping(void);
void handleDistanceResponse(int, int);
void dive(int);
void raise(int);

int main() {
    setup();

    TWI_Master_Init();

    sei();

    //DIVE, DIVE, DIVE!
    // dive(255);
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
    return distance;
    TWI_Master_Stop();
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

void dive(int d) {
    TWI_Master_Start(DEPTH_CONTROLLER, 0x00); //Write Command..
    TWI_Master_Write(d);
    TWI_Master_Stop();
}
void raise(int d) {
    TWI_Master_Start(DEPTH_CONTROLLER, 0x00);
    TWI_Master_Write(d);
    TWI_Master_Stop();
}

ISR(RTC_CNT_vect) {
    // int distance = ping();
    // handleDistanceResponse(distance, LOWER);
    RTC.INTFLAGS = RTC_OVF_bm;
}