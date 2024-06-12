#include <avr/io.h>
#include "I2C_Host.h"

#define MOTOR_DRIVER_ADDR 0xA1
#define SONAR_ADDR 0xB1

void MainClkCtrl(void);

int main()
{
    MainClkCtrl();
    I2C_Host_InitI2C();
    
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