#include <avr/io.h>

#include "I2C_Client.h"

#define SLAVE_ADDR 0x6f

void I2CCallback(uint8_t);
void InitPins(void);

int main(void)
{
//     I2C_Client_InitPins();
    InitPins();
    I2C_Client_InitI2C(SLAVE_ADDR, I2CCallback);
    
    while(1)
    {

    }

    return 0;
}

void I2CCallback(uint8_t data)
{
    if (data == 0xff)
    {
        PORTA.DIRCLR |= PIN4_bm | PIN5_bm | PIN6_bm;
        PORTA.DIR |= PIN4_bm;
    }
    if (data == 0xee)
    {
        PORTA.DIRCLR |= PIN4_bm | PIN5_bm | PIN6_bm;
        PORTA.DIR |= PIN5_bm;
    }
    if (data == 0xdd)
    {
        PORTA.DIRCLR |= PIN4_bm | PIN5_bm | PIN6_bm;
        PORTA.DIR |= PIN6_bm;
    }
}

void InitPins(void)
{
    PORTA.DIR |= PIN4_bm | PIN5_bm | PIN6_bm;
}