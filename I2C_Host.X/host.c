#include <avr/io.h>
#include <avr/interrupt.h>

#include "I2C_Host.h"

#define SLAVE_ADDR 0x6f

void InitRTC(void);

uint8_t cmd[3] = { 0xff, 0xee, 0xdd };
uint8_t index = 0;

int main(void)
{
//    I2C_Host_InitPins();
    I2C_Host_InitI2C();

    InitRTC();

    sei();

    while(1)
    {

    }

    return 0;
}

void InitRTC(void)
{
    // Configure the RTC clock source to the 32.768 kHz external oscillator
    // Assuming an external 32.768 kHz crystal is connected to the TOSC1 and TOSC2 pins
    CLKCTRL.OSC32KCTRLA = 0;

    // Wait for the external oscillator to stabilize
    while (RTC.STATUS & RTC_CTRLABUSY_bm);

    // Select the 32.768 kHz clock as the RTC clock source
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;

    // Configure the RTC period for 2 seconds
    // Assuming a 32.768 kHz clock, 2 seconds = 65536 counts
    RTC.PER = 0xFFFF;

    // Enable the RTC overflow interrupt
    RTC.INTCTRL = RTC_OVF_bm;

    // Enable the RTC
    RTC.CTRLA = RTC_RTCEN_bm | RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc;
}

// RTC Overflow Interrupt Service Routine (ISR)
ISR(RTC_CNT_vect)
{
    // Clear the interrupt flag
    RTC.INTFLAGS = RTC_OVF_bm;
    uint8_t addr = (uint8_t)(SLAVE_ADDR);
    //0x01 should be write mode. 
    if (I2C_Host_Start(addr, 0x00) == 0x01)
    {
        I2C_Host_WriteData(cmd[index]);
        index++;
        if (index > 2)
        {
            index = 0;
        }
    }

    I2C_Host_Stop();
}

