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

void SetupRTC(void) 
{
    // Use 1.024 kHz internal clock
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    
    // Set the period for 5 seconds (5 * 1024 = 5120)
    RTC.PER = 5120;
//    RTC.PER = 1024;
    // Enable the overflow interrupt
    //In the final module code, this interrupt code will be replaced with i2c intterrupt code. 
    RTC.INTCTRL = RTC_OVF_bm;
    // Clear any existing flags
    RTC.INTFLAGS = RTC_OVF_bm;
    // Enable the RTC
    RTC.CTRLA = RTC_RTCEN_bm;
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

