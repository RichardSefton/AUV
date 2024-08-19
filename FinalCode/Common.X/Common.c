#include "Common.h"

void setupRGB(void) {
    PORTB.DIR |= RED | GREEN | BLUE;
    PORTB.OUTSET |= RED | GREEN | BLUE;
}

void RGB(u8 colour) {
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = colour;
}