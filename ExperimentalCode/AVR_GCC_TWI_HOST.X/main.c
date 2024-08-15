#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include "TWI.h"

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setupPins(void);
void RGB(uint8_t);

int main() {
    setupPins();
    
    TWI_Master_Init();
    
    while(1) {
        RGB(RED);
        TWI_Master_Start(0x30, 0);
        TWI_Master_Write(RED);
        TWI_Master_Stop();
        _delay_ms(500);
        RGB(GREEN);        
        _delay_ms(500);
        RGB(BLUE);
        TWI_Master_Start(0x30, 0);
        TWI_Master_Write(BLUE);
        TWI_Master_Stop();
        _delay_ms(500);
    }
    
    return 0;
}

void setupPins(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
}

void RGB(uint8_t colour) {
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}

