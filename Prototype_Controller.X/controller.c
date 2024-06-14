#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"

#define MOTOR_DRIVER_ADDR 0x09
#define SONAR_ADDR 0x08

#define COM_STOP 0xFF
#define COM_START 0xAA

void MainClkCtrl(void);
void SetupRTC(void);

uint8_t distance = 0;

int main()
{
    MainClkCtrl();
    SetupRTC();
    
    TWI_Master_Init();
    
    sei();
    
    while(1)
    {
        
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm); // Prescaler of 6
    // F_CPU with this configuration will be 3.33MHz
}

void SetupRTC(void) 
{
    // Use 1.024 kHz internal clock
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    
    // Set the period for 5 seconds (5 * 1024 = 5120)
//    RTC.PER = 5120*6;
//    RTC.PER = 5120 * 2;
    RTC.PER = 1024;
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
    distance = 0;
    
    TWI_Master_Start(SONAR_ADDR, 0x01);
    distance = TWI_Master_Read_NACK();
    TWI_Master_Stop();
    
    TWI_Master_Start(MOTOR_DRIVER_ADDR, 0x00);
    
  
//    if (I2C_Host_Start(MOTOR_DRIVER_ADDR, 0x00) == 0x01)
//    {
//        if (distance < 50)
//        {
//            I2C_Host_WriteData(COM_STOP);
//        }
//        else 
//        {
//            I2C_Host_WriteData(COM_START);
//        }
//        I2C_Host_Stop();      
//    }
    
    
}