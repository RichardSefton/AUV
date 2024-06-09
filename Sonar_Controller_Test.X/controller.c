#include <avr/io.h>
#include <avr/interrupt.h>
#include "I2C_Host.h"

#define CLIENT_ADDR 0x6f

#define PING 0xff

/**
 * This will be the default setup but its good to be explicit..
 */
void MainClkCtrl(void);
/**
 * Real time clock will count to a period and trigger an interrupt. The interrupt 
 * will send a request to the sonar module via I2C.
 */
void SetupRTC(void);

int main(void)
{
    MainClkCtrl();
    SetupRTC();
    I2C_Host_InitI2C();
    
    sei();
    
    while(1)
    {
        
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    // Set the main clock to 20MHz without prescaler
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm); //Prescaler of 6
    //F_CPU with this configuration will be 3.33MHz
}

void SetupRTC(void) 
{
    // Use 1.024 kHz internal clock
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    
    // Set the period for 5 seconds (5 * 1024 = 5120)
    RTC.PER = 5120*6;
//    RTC.PER = 1024;
    // Enable the overflow interrupt
    //In the final module code, this interrupt code will be replaced with i2c intterrupt code. 
    RTC.INTCTRL = RTC_OVF_bm;
    // Clear any existing flags
    RTC.INTFLAGS = RTC_OVF_bm;
    // Enable the RTC
    RTC.CTRLA = RTC_RTCEN_bm;
}

ISR(RTC_CNT_vect)
{
    //Clear the interrupt flag
    RTC.INTFLAGS |= RTC_OVF_bm;
    
    uint8_t client = (uint8_t)(CLIENT_ADDR);
    
    if (I2C_Host_Start(client, 0x00) == 0x01)
    {
        I2C_Host_WriteData(PING);
    }
    
    I2C_Host_Stop();
}