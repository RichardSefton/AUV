#include "I2C_Host.h"

void I2C_Host_InitI2C(void)
{
    // Set the Master Baud Rate (Master defines baud for clients)
    TWI0.MBAUD = 12; // This value should be calculated for the desired I2C speed

    // Set the bus state to IDLE
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    // Enable Smart Mode and acknowledge action set to ACK
    TWI0.MCTRLB = TWI_ACKACT_ACK_gc;

    // Enable the TWI
    TWI0.MCTRLA = TWI_SMEN_bm | TWI_ENABLE_bm;
}

// Start I2C communication with the given address and direction (0x00 for write, 0x01 for read)
uint8_t I2C_Host_Start(uint8_t addr, uint8_t dir)
{
    // Wait for the bus to be idle
    while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc);

    // Send the start condition and address
    TWI0.MADDR = (addr << 1) | dir;

    // Wait for the address to be acknowledged or an error to occur
    while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)));

    // Check for arbitration lost or bus error
    if (TWI0.MSTATUS & (TWI_ARBLOST_bm | TWI_BUSERR_bm))
    {
        // Reset TWI to clear error condition
        TWI0.MCTRLA &= ~TWI_ENABLE_bm;
        TWI0.MCTRLA |= TWI_ENABLE_bm;

        return 0x00; // Failed to start
    }

    // Check if the slave responded with a NACK
    if (TWI0.MSTATUS & TWI_RXACK_bm)
    {
        return 0x00; // No acknowledgment
    }

    return 0x01; // Success
}

// Write data to I2C
void I2C_Host_WriteData(uint8_t data)
{
    // Write data to the bus
    TWI0.MDATA = data;

    // Wait for the write to complete
    while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)));

    // Check for NACK or bus error
    if (TWI0.MSTATUS & (TWI_RXACK_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm))
    {
        I2C_Host_Stop();
    }
}

// Read data from I2C
uint8_t I2C_Host_ReadData(void)
{
    // Wait for data to be received
    while (!(TWI0.MSTATUS & (TWI_RIF_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)));

    // Check for bus error or arbitration lost
    if (TWI0.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
    {
        I2C_Host_Stop();
        return 0;
    }

    // Read the data
    uint8_t data = TWI0.MDATA;

    // Send ACK
    TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;

    return data;
}

// Send the stop condition
void I2C_Host_Stop(void)
{
    // Send the Stop command
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}
