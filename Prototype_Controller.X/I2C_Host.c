#include "I2C_Host.h"

void I2C_Host_InitI2C(void)
{
    // Set the Master Baud Rate (Master defines baud for clients)
    TWI0.MBAUD = 12; // This value should be calculated for the desired I2C speed

    // Set the bus state to IDLE
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

//    // Enable Smart Mode and acknowledge action set to ACK
//    TWI0.MCTRLB |= TWI_ACKACT_ACK_gc;

    // Enable the TWI
    TWI0.MCTRLA |= TWI_SMEN_bm | TWI_TIMEOUT_200US_gc | TWI_ENABLE_bm;
}

uint8_t I2C_Host_Start(uint8_t addr, uint8_t dir)
{
    return I2C_Host_Start_R(addr, dir, 0);
}
// Start I2C communication with the given address and direction (0x00 for write, 0x01 for read)
uint8_t I2C_Host_Start_R(uint8_t addr, uint8_t dir, uint8_t r)
{   
    //Escape clause incase we need recursion..
    if (r > 10)
    {
        return 0;
    }
    
    // Wait for the bus to be idle
    //Not required according to ds. If not idle the master will copy addr to MDATA and wait. 
//    while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc);

    // Send the start condition and address
    TWI0.MADDR = (addr << 1) | dir;
    
    //No slave response...
    if (TWI0.MSTATUS & I2C_CHECK_NACK())
    {
        return I2C_Host_Start_R(addr, dir, r++);
    }
    //Lost Arbitration
    if (TWI0.MSTATUS & I2C_CHECK_ARBLOST())
    {
        //We should wait till bus is idle then try again. 
        //NOTE: This is blocking code
        while(!(TWI0.MSTATUS & I2C_CHECK_IDLE()));
        return I2C_Host_Start_R(addr, dir, r++);
    }
    //Bus Error - Just return 0
    if (TWI0.MSTATUS & I2C_CHECK_BUSERR())
    {
        return 0;
    }
    
    if (dir == 0)
    {
        /**
         * Slave responds to the address packet:
         * 
         * ---MSTATUS---
         * WIF:         1
         * RXACK:       0
         * CLKHOLD:     1
         */
        if(TWI0.MSTATUS & I2C_CHECK_WRITE())
        {
            //We're now ready to transmit data. 
            return 1;
        }
    }
    
    if (dir == 1)
    {
        /**
         * Slave responds to the address packet:
         * 
         * ---MSTATUS---
         * RXACK:       0
         * 
         * Slave can start sending data. Client is ready to receive. 
         */
        while(!(TWI0.MSTATUS & I2C_CHECK_READ()));
        if (TWI0.MSTATUS & I2C_CHECK_READ())
        {
            //We're now ready to receive data. 
            return 1;
        }
    }
    
    return 0;

//    // Wait for the address to be acknowledged or an error to occur
//    while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)));
//
//    // Check for arbitration lost or bus error
//    if (TWI0.MSTATUS & (TWI_ARBLOST_bm | TWI_BUSERR_bm))
//    {
//        // Reset TWI to clear error condition
//        return 0x00; // Failed to start
//    }
//
//    // Check if the slave responded with a NACK
//    if (TWI0.MSTATUS & TWI_RXACK_bm)
//    {
//        return 0x00; // No acknowledgment
//    }
//
//    return 0x01; // Success
}
    
// Write data to I2C
void I2C_Host_WriteData(uint8_t data)
{
    // Write data to the bus. This will clear WIF flag. It will go back to 1 
    // when transfer is complete. 
    TWI0.MDATA = data;
    //Wait for flag to go high again..
    while(!(TWI0.MSTATUS & I2C_CHECK_WRITE_COMP()));
    
    //Error handle if needed... Lets just return if the bus is in an error or arblost
    //state..
    //Lost Arbitration
    if (TWI0.MSTATUS & I2C_CHECK_ARBLOST())
    {
        return;
    }
    //Bus Error - Just return 0
    if (TWI0.MSTATUS & I2C_CHECK_BUSERR())
    {
        return;
    }
    
    if (!(TWI0.MSTATUS) & I2C_CHECK_NACK())
    {
        //if ACK is 0 the slave is ready to receive more data packets..
        
        //In this library we're only sending 8 bits.. so...
        I2C_Host_Stop();
    }
    //If successful and a NACK received the client cant receive any more. So stop. 
    if (TWI0.MSTATUS & I2C_CHECK_NACK())
    {
        I2C_Host_Stop();
    }
    
//    // Check for NACK or bus error
//    if (TWI0.MSTATUS & (TWI_RXACK_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm))
//    {
//        I2C_Host_Stop();
//    }
}

// Read data from I2C
uint8_t I2C_Host_ReadData(void)
{   //Temporarily disable smart mode so we can send the NACK bit...
    TWI0.MCTRLA &= ~(TWI_SMEN_bm); 
    // Wait for data to be received or an error to occur
//    while (!(TWI0.MSTATUS & (TWI_RIF_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)));

    // Check for bus error or arbitration lost
    if (TWI0.MSTATUS & I2C_CHECK_ARBLOST())
    {
        return 0;
    }
    //Bus Error - Just return 0
    if (TWI0.MSTATUS & I2C_CHECK_BUSERR())
    {
        return 0;
    }

    while (!(TWI0.MSTATUS & I2C_CHECK_READ_ALLOWED()));
    // Read the data from the data register
    uint8_t data = TWI0.MDATA;

    // Send NACK to signal the end of the read operation
    //We're only really accepting 8 bits right now... We should issue NACK then stop. 
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc;
    I2C_Host_Stop();
    
    //Renable Smart mode
    TWI0.MCTRLA |= TWI_SMEN_bm;

    return data;
}

// Send the stop condition
void I2C_Host_Stop(void)
{
    // Send STOP condition
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
    // Wait for the bus to become idle
    uint8_t i = 0;
    while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc)
    {
        i++;
        if (i > 10) 
        {
            // Reset TWI to clear stuck condition
            if (TWI0.MSTATUS & TWI_RXACK_bm)
            {
                TWI0.MSTATUS &= ~(TWI_RXACK_bm);
            }
            TWI0.MCTRLB |= TWI_FLUSH_bm;
            i = 0;
        }
    }
}
