
#include "I2C_Host.h"

void I2C_Host_InitPins(void)
{
    /**
     * Data sheet claims 1627 pins are the following:
     * 
     * PB0: SCL
     * PB1: SDA
    */

    PORTB.DIR |= PIN0_bm | PIN1_bm;

    PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
}

void I2C_Host_InitI2C(void)
{
    //Set the Master Baud Rate (Master defines baud for clients)
//    TWI0.MBAUD = I2C_Host_CalcBaud();
    TWI0.MBAUD = 12;

    //Set the bus state to IDLE
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    //Enable Smart Mode. We need to set MCTRLB ACKACT set to ACK
    //Then MCTRLA SMEN (Smart Mode Enable) set to 1
    TWI0.MCTRLB = TWI_ACKACT_ACK_gc;

    //Enable the TWI
    TWI0.MCTRLA = TWI_SMEN_bm | TWI_ENABLE_bm;
}

uint8_t I2C_Host_CalcBaud(void)
{
    double fSCL = I2C_Host_CalcBaud_F_SCL();
    double baud = I2C_Host_CalcBaud_BAUD(fSCL);
    return (uint8_t)(baud);
}


struct BAUD_TIMING_t BAUD_TIMING = {
    .T_LOW = 5.0e-6,
    .T_HIGH = 5.0e-6,
    .T_OF = 0,
    .T_R = 0
}; //These values should produce time of 100000

double I2C_Host_CalcBaud_F_SCL(void)
{
    //f(SCL) = 1 / t_LOW + t_HIGH + t_OF + t_R
    /**
     * From the timing requirements we can infer that:
     * 
     * t_LOW:               MIN VALUE                   MAX VALUE
     *      @ <= 100kHZ     4.7us
     *      @ <= 400kHZ     1.3us
     *      @ <= 1MHz       0.5us
     * 
     * t_HIGH
     *      @ <= 100kHZ     4.0us
     *      @ <= 400kHZ     0.6us
     *      @ <= 1MHz       0.26us
     * 
     * t_OF
     *      @ <= 100kHZ     -                           250ns
     *      @ <= 400kHZ     20*(Vdd / 5.5)ns            250ns
     *      @ <= 1MHz       20*(Vdd / 5.5)ns            120ns
     * 
     * t_R
     *      @ <= 100kHZ     -                           1000ns
     *      @ <= 400kHZ     20ns                        300ns
     *      @ <= 1MHz       -                           120ns
    */
    //init values in seconds
   
   double total = 5.0e-6 + 5.0e-6 + 0 + 0;
   double fSCL = 1.0 / total;
   return fSCL;
}

double I2C_Host_CalcBaud_BAUD(double fScl)
{
    double baud = (F_CPU / 2*(fScl)) - (5 + ((F_CPU * BAUD_TIMING.T_R) / 2));
    return baud;
}

uint8_t I2C_Host_Start(uint8_t addr, uint8_t dir)
{
    while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc);
    //Write the address shifting one to the left. dir bit determines 
    //if Read(0x00) or write(0x01) operation
    TWI0.MADDR |= (addr << 1) | dir;

    //We only want to check the flags are high on write mode
    if (dir == 0x00)
    {
        //Check Interrupt Flags are high
//        while(!(TWI0.MSTATUS & I2C_CHECK_START()));
        while (!(TWI0.MSTATUS & TWI_WIF_bm) || !(TWI0.MSTATUS & TWI_CLKHOLD_bm));
    }

    // all is good, return truthy
    return 0x01;
}

void I2C_Host_WriteData(uint8_t data)
{
    //Writing to MData clears the WIF (Write Interrupt Flag)
    TWI0.MDATA = data;

    //Check for WIF
    while(!(TWI0.MSTATUS & I2C_CHECK_WRITE()))
    {
        if (TWI0.MSTATUS & I2C_CHECK_NACK())
        {
            //Error Handle. For now call stop?
            I2C_Host_Stop();
            return;
        }
    }
}

void I2C_Host_Stop(void)
{
    //Send the Stop Command on MCTRLB
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}
