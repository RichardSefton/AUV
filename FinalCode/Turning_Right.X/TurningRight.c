#define F_CPU 3333333UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "CWire.h"
#include "Common.h"
#include "ShortTypes.h"
#include <util/delay.h>

#define ADDR 0x47

void setup(void);
void setupPins(void);
void startMotors(void);
void stopMotors(void);
void I2C_RX_Callback(u8);

#define MOTOR1_PIN1 PIN0_bm
#define MOTOR1_PIN2 PIN1_bm
#define MOTOR2_PIN1 PIN2_bm
#define MOTOR2_PIN2 PIN3_bm

enum MotorState {
    ON = 1,
    OFF = 2
};

TwoWire twi0;
int main(void) {
    setup();
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Slave_begin(&twi0, ADDR, 0, 0);
    
    TwoWire_onReceive(&twi0, I2C_RX_Callback);
    
    sei();
    
    RGB(RED);
    
    while(1);
    
    return 0;
}

void setup(void) {
    setupRGB();
    setupPins();
}

void setupPins(void) {
    PORTC.DIR |= MOTOR1_PIN1 | MOTOR1_PIN2 | MOTOR2_PIN1 | MOTOR2_PIN2;
    PORTC.OUTCLR = MOTOR1_PIN1 | MOTOR1_PIN2 | MOTOR2_PIN1 | MOTOR2_PIN2;
}

void startMotors(void) {
    PORTC.OUTSET = MOTOR1_PIN1 | MOTOR2_PIN1;
    PORTC.OUTCLR = MOTOR1_PIN2 | MOTOR2_PIN2;
}

void stopMotors(void) {
    PORTC.OUTCLR = MOTOR1_PIN1 | MOTOR1_PIN2 | MOTOR2_PIN1 | MOTOR2_PIN2;
}

void I2C_RX_Callback(u8 numBytes) {
    if (numBytes == 1) {
        while (TwoWire_available(&twi0)) {
            u8 data = TwoWire_read(&twi0);
            if (data == ON) {
                RGB(GREEN);
                startMotors();
            } else if (data == OFF) {
                RGB(RED);
                stopMotors();
            }
        }
    }
}
