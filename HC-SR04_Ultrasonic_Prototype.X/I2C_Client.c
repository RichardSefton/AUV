#include "I2C_Client.h"

// Global variable to store the receive callback function
static I2C_ReceiveCallback receive_callback = NULL;

// Initialize I2C pins
void I2C_Client_InitPins(void)
{
    // Set PB0 (SCL) and PB1 (SDA) as input
    PORTB.DIR &= ~(PIN0_bm | PIN1_bm);
}

// Initialize I2C in client mode
void I2C_Client_InitI2C(uint8_t address, I2C_ReceiveCallback callback)
{
    // Store the callback function
    receive_callback = callback;

    // Set the slave address
    TWI0.SADDR = address << 1 | 0x00; //| 0x01 respond to all addr. 

    // Enable the TWI and Smart Mode, clear collision and bus error flags
    TWI0.SCTRLA = TWI_ENABLE_bm | TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm;

    // Ensure the slave interface is enabled
//    TWI0.SCTRLB = 0;
    sei();
}

// Read data from I2C
uint8_t I2C_Client_ReadData(void)
{
    // Wait for data to be received
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
    
    // Check if data was received
    if (TWI0.SSTATUS & TWI_DIF_bm)
    {
        // Read and return the data
        return TWI0.SDATA;
    }

    // Return 0 if no data was received
    return 0;
}

// Write data to I2C
void I2C_Client_WriteData(uint8_t data)
{
    // Wait for the data register to be empty
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
    
    // Write the data
    TWI0.SDATA = data;
    
    // Wait for the data to be transmitted
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
}

// Interrupt Service Routine for I2C
ISR(TWI0_TWIS_vect)
{
    // Check if Address/Stop interrupt
    if (TWI0.SSTATUS & TWI_APIF_bm)
    {
        // Clear the interrupt flag
        TWI0.SSTATUS = TWI_APIF_bm;
        
        // Check if the address match
        if (TWI0.SSTATUS & TWI_AP_bm)
        {
            // Clear the address match flag
            TWI0.SSTATUS = TWI_AP_bm;
            TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;
        }
        else
        {
            // If not an address match, it must be a stop condition
            TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
        }
    }

    // Check if Data interrupt
    if (TWI0.SSTATUS & TWI_DIF_bm)
    {
        // Read the received data
        uint8_t received_data = TWI0.SDATA;

        //Call the receive callback if it's set
        if (receive_callback != NULL)
        {
            receive_callback(received_data);
        }

        // Clear the data interrupt flag
        TWI0.SSTATUS = TWI_DIF_bm;
    }
}


