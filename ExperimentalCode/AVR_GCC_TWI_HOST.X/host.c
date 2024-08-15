/* 
 * File:   host.c
 * Author: richa
 *
 * Created on August 12, 2024, 10:14 PM
 */

#define F_CPU 3333333UL
#include <stdio.h>
#include <stdlib.h>
#include <util/twi.h>
#include <util/delay.h>

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setupPins(void);
void RGB(uint8_t);
/*
 * 
 */
int main(int argc, char** argv) {

    while(1) {
        RGB(RED);
        _delay_ms(500);
        RGB(RED);
        _delay_ms(500);
        RGB(RED);
        _delay_ms(500);
    }
    
    return (EXIT_SUCCESS);
}

void TWI_init(void) {
    // Set the TWI bit rate register to configure the clock frequency
    // Assuming a CPU frequency of 8MHz and setting TWI frequency to 100kHz:
    TWSR = 0x00; // Prescaler = 1
    TWBR = ((F_CPU / 100000UL) - 16) / 2; // SCL frequency 100kHz
    
    // Enable the TWI module
    TWCR = (1 << TWEN);
}

void setupPins(void) {
    PORTB.OUT |= RED | GREEN | BLUE;
}

void RGB(uint8_t colour) {
    PORTB.OUTSET |= RED | GREEN | BLUE;
    PORTB.OUTCLR |= colour;
}
