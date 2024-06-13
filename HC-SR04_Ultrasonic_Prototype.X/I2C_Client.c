#include "I2C_Client.h"

// Global variable to store the receive callback function
static I2C_ReceiveCallback receive_callback = NULL;
static I2C_TransmitCallback transmit_callback = NULL;

// Initialize I2C in client mode
void I2C_Client_InitI2C(uint8_t address, I2C_ReceiveCallback rx_callback, I2C_TransmitCallback tx_callback)
{
    // Store the callback functions
    receive_callback = rx_callback;
    transmit_callback = tx_callback;

    // Set the slave address
    TWI0.SADDR = address << 1 | 0x00;

    // Enable the TWI and enable various interrupts
    TWI0.SCTRLA = TWI_ENABLE_bm | TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm;

    sei(); // Enable global interrupts
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
    // Write the data
    TWI0.SDATA = data;

    // Send acknowledge and wait for the data to be transmitted
    TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;
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
        // Check if data was requested (read operation)
        if (TWI0.SSTATUS & TWI_DIR_bm)
        {
            // Call the transmit callback to get data to send
            if (transmit_callback != NULL)
            {
                uint8_t data_to_send = transmit_callback();
                if (data_to_send > 0x00)
                {
                    I2C_Client_WriteData(data_to_send);
                }
                
            }
        }
        else
        {
            // Read the received data
            uint8_t received_data = TWI0.SDATA;

            // Call the receive callback if it's set
            if (receive_callback != NULL)
            {
                receive_callback(received_data);
            }
        }

        // Clear the data interrupt flag and prepare to respond
        TWI0.SSTATUS = TWI_DIF_bm;
        TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;
    }
}
