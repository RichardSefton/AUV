#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>

#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN4_bm

void setup(void);
void RGB(uint8_t);

int main() {
    setup();
    
    while(1) {
        RGB(RED);
        _delay_ms(500);
        RGB(GREEN);
        _delay_ms(500);
        RGB(BLUE);
        _delay_ms(500);
    }
    
    return 0;
}

void setup(void) {
    PORTB.OUT |= RED | GREEN | BLUE;
}

void RGB(uint8_t colour) {
    PORTB.OUTSET |= RED | GREEN | BLUE;
    PORTB.OUTCLR |= colour;
}