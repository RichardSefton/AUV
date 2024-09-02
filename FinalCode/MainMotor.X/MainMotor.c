#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Common.h"
#include "StepperMotor.h"
#include "CWire.h"
#include "ShortTypes.h"

#define ADDR 0x45

void setup(void);
//TWI Library requires callbacks. 
void I2C_RX_Callback(u8);

typedef enum {
    FORWARD = 1,
    STOP = 2,
    BACKWARD = 3
} Directions;

StepperMotor stepper;
Directions dir;
TwoWire twi0;

int main() {
    setup();
    
    RGB(RED);
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Slave_begin(&twi0, ADDR, 0, 0);
    
    TwoWire_onReceive(&twi0, I2C_RX_Callback);
    
    RGB(GREEN);
    dir = STOP;
    
    sei();
    
    while(1) {
        if (dir == FORWARD) {
            RGB(GREEN);
            StepperMotor_step(&stepper);
            StepperMotor_increaseStep(&stepper);
        } else if (dir == BACKWARD) {
            RGB(BLUE);
            StepperMotor_step(&stepper);
            StepperMotor_decreaseStep(&stepper);
        } else {
            RGB(RED);
            StepperMotor_allStop();
        }
        _delay_us(750);
    }
    
    return 0;
}

void setup(void) {
    setupRGB();
    StepperMotor_init(&stepper);
}

//TWI Library requires callbacks. 
void I2C_RX_Callback(u8 numOfBytes) {
    if (numOfBytes == 1) {
        while (TwoWire_available(&twi0)) {
            u8 data = TwoWire_read(&twi0);
            if (data == FORWARD) {
                dir = FORWARD;
            } 
            if (data == BACKWARD) {
                dir = BACKWARD;
            }
            if (data == STOP) {
                dir = STOP;
            }
        }
    }
}