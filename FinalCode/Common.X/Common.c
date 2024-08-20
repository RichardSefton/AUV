#include "Common.h"

void setupRGB(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
    PORTB.OUT |= (RED | GREEN | BLUE);
}

void RGB(u8 colour) {
    PORTB.OUT |= RED | GREEN | BLUE;
    PORTB.OUT &= ~colour;
}