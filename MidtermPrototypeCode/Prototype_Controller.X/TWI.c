#include "TWI.h"

/* Initialize TWI/I2C master - TWI is AVR version that supports I2C since I2C is a 
 * protocol made by Phillips and supported by many MCUs.
 * 
 * This library is developed using the datasheet and the Make: AVR Programming book
 * with some reference to libraries such as https://github.com/technoblogy/tiny-i2c
 * 
 * Finding code examples online is a challenge and they need to be adapted because
 * the ATTiny1627 is not widely used as say anything from the ATMega line. Typically
 * users of the Tiny series use chips such as the ATTiny45 or similar that comes in a 
 * DIP (breadboard friendly) packaging. 
 * 
 * Also alot of the existing libraries are focused towards Arduino which I'm trying 
 * to move away from.
 */

void TWI_Master_Init(void) 
{
    /* Set the TWI baud rate
     * This calculation should for the ATTiny1627 on standard speed come to ~12
     * but because the way C handles float conversion to uint8, the fractional is dropped 
     * and the result rounded down. So we're adding one at the end to combat this.
     */ 
    TWI0.MBAUD = (uint8_t)TWI_BAUD(TWI_FREQ, 0) + 1; // Adding 1 as it is rounding down
    
    /* Enable TWI master and Smart Mode
     * Smart mode will automatically raise the DIF, RIF and WIF interrupt flags when
     * reading from and or writing to the MDATA, SDATA and MADDR buffers. This takes
     * a fair bit of the work out of our hands, but may raise some issues when we come 
     * to ending the Read operation as the Master should send a NACK to RXACK with a 
     * stop condition to end the operation. 
     */
    TWI0.MCTRLA |= TWI_ENABLE_bm | TWI_SMEN_bm;
    
    /**
     * Writing to the Flush bit on the MCTRLB register will flush any errors or bus busy states
     * try to force the protocol into IDLE. Since in this implementation we only have 
     * one Master, this should work but I've never writen a protocol from scratch before
     * so I guess we'll find out..
     */
    TWI0.MCTRLB |= TWI_FLUSH_bm; // Flush TWI data
    
    // Explicitly set the bus state to IDLE.
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
    
    // Enable the Master Write Interrupt and Master Read Interrupt
    TWI0.MCTRLA |= TWI_WIEN_bm | TWI_RIEN_bm;
    
    /**
     * Because some implementations of this library may not need Interrupts (Motor 
     * Controller is a key suspect here), we should enable Interrupts so code in this
     * library will still work. 
     */
    sei();
}

void TWI_Master_Reset(void)
{
    // Disable TWI by writing a zero to the enable bit
    TWI0.MCTRLA &= ~TWI_ENABLE_bm;

    // Clear status registers to reset the state
    TWI0.MSTATUS |= TWI_BUSERR_bm | TWI_ARBLOST_bm | TWI_RXACK_bm | TWI_COLL_bm | TWI_BUSSTATE_IDLE_gc;
//    TWI0.MCTRLB |= TWI_FLUSH_bm; // Flush TWI

    // Re-enable TWI
    TWI0.MCTRLA |= TWI_ENABLE_bm;
}

void TWI_Master_Start(uint8_t address, uint8_t read)
{
    TWI0.MADDR = (address << 1) | read;
    //Wait for the address transmission to complete. When the address is confirmed 
    //by the slave, the WIF or RIF flags will be raised. 
    while (!(TWI0.MSTATUS & (TWI_RIF_bm | TWI_WIF_bm)));
}

void TWI_Master_Write(uint8_t data) 
{
    TWI0.MDATA = data;
    
    //Wait for the WIF to become high. Writing data will set it low (Smart mode),
    //When the Slave receives it will be set back to high. 
    while (!(TWI0.MSTATUS & TWI_WIF_bm));
    
    // Check for arbitration lost or bus error
    if (TWI0.MSTATUS & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {
        // Handle error
        TWI0.MCTRLB = TWI_FLUSH_bm;
        TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
    }
}

uint8_t TWI_Master_Read_ACK(void) 
{
//    //Temporarily Disable SM
//    TWI0.MCTRLA &= ~TWI_SMEN_bm;
    //Prime the MCTRLB with the ACK bit
    TWI0.MCTRLB = TWI_ACKACT_ACK_gc;
    
    //Wait till the device has received data
    while (!(TWI0.MSTATUS & TWI_RIF_bm));
    
    //return the data from the buffer. Smart mode should then send the ACK bit and 
    //clear the relevant flags.
    return TWI0.MDATA;
}

uint8_t TWI_Master_Read_NACK(void) 
{
    //Temporarily Disable SM
//    TWI0.MCTRLA &= ~TWI_SMEN_bm;
    
    //Prime the MCTRLB with the NACK bit
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc;
    
    //Wait till the device has received the data
    while (!(TWI0.MSTATUS & TWI_RIF_bm));
    
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
    //Return the data from the buffer. Smart mode should then send the NACK bit 
    //and clear the relevant flags
    uint8_t data = TWI0.MDATA;
    
//    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
    
    //Renable SM
//    TWI0.MCTRLA |= TWI_SMEN_bm;
    
//    return TWI0.MDATA;
    return data;
}

/**
 * DO NOT CALL ARBITRARILY. THIS COULD PUT THE BUS IN AN ERROR STATE.
 */
void TWI_Master_Stop(void) {
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
//    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; 
}

// Global variable to store the receive callback function
static I2C_ReceiveCallback receive_callback = NULL;
static I2C_TransmitCallback transmit_callback = NULL;

void TWI_Slave_Init(uint8_t address, I2C_ReceiveCallback rx, I2C_TransmitCallback tx) 
{
    //Assign the RX and TX callback functions so they can be accessed by this script (raise their scope). 
    receive_callback = rx;
    transmit_callback = tx;
    
    // Set the slave address. << 1 as the first bit is for enabling broadcast mode on 0x00
    TWI0.SADDR = address << 1;
    
    // Enable TWI slave and various interrupts. Also Smart Mode. 
    TWI0.SCTRLA |= TWI_ENABLE_bm | TWI_SMEN_bm | TWI_DIEN_bm | TWI_APIEN_bm | TWI_PIEN_bm;
    
    sei();
}

void TWI_Slave_Write(uint8_t data) {
    //Write the data to the buffer to send to the Master. This should clear the DIF interrupt. 
    TWI0.SDATA = data;
    
    //When the Master has received the data the DIF interrupt should be high again. 
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
    
    if (TWI0.SSTATUS & TWI_RXACK_bm)
    {
        //Master send dobby an NACK. END TRANSMISSION
        TWI0.SCTRLB |= TWI_SCMD_COMPTRANS_gc;
    }
    
    // Clear the DIF
//    TWI0.SSTATUS |= TWI_DIF_bm;
    
}

uint8_t TWI_Slave_Read(void) {
    //Wait for the data to be ready. 
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
    
    //Read the data from the buffer
    uint8_t data = TWI0.SDATA;
    
    // Clear the data interrupt flag
    //THIS SHOULDNT BE REQUIRED BECAUSE SMART MODE. 
//    TWI0.SSTATUS |= TWI_DIF_bm;
    
    
    return data;
}

uint8_t TWI_Slave_Read_ACK(void) {
    // Wait for the data to be ready
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
    
    // Read the data from the buffer
    uint8_t data = TWI0.SDATA;
    
    // Send ACK after receiving data
    TWI0.SCTRLB = TWI_ACKACT_ACK_gc;
    
    return data;
}

uint8_t TWI_Slave_Read_NACK(void) {
    // Wait for the data to be ready
    while (!(TWI0.SSTATUS & TWI_DIF_bm));
    
//     Send NACK after receiving data
    TWI0.SCTRLB = TWI_ACKACT_NACK_gc;
    
    return TWI0.SDATA;
}

/**
 * Interrupt Service Routines
 * 
 * This is how the Slave will begin responding to the requests. 
 * 
 * First it will check the address. AP is an address match. Clearing the flag will 
 * trigger the Master to begin its operation. 
 * 
 * DIF is raised when data is received or Master is ready to receive. the DIR register bit
 * will identify which mode of operation it is. 
 */
ISR(TWI0_TWIS_vect) 
{
    // Check if Address/Stop interrupt
//    if (TWI0.SSTATUS & TWI_APIF_bm) {
//        // Clear the interrupt flag
//        TWI0.SSTATUS |= TWI_APIF_bm;
//        // Check if the address match
//        if (TWI0.SSTATUS & TWI_AP_bm) {
//            // Clear the address match flag
//            TWI0.SSTATUS |= TWI_AP_bm;
//        }
//    }
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
//        else
//        {
//            // If not an address match, it must be a stop condition
//            TWI0.SCTRLB = TWI_SCMD_COMPTRANS_gc;
//        }
    }

    // Check if Data interrupt
    if (TWI0.SSTATUS & TWI_DIF_bm) {
        // Check if data was requested (read operation)
        if (TWI0.SSTATUS & TWI_DIR_bm) {
            //Get the data from the callback function passed in in initialisation. 
            //Should handle cases where this is NULL. but eh. Not for this this implementation. 
            uint8_t data_to_send = transmit_callback();
            TWI_Slave_Write(data_to_send);  // Example data
        } else {
            //Read the received data. 
            uint8_t received_data = TWI_Slave_Read_NACK();
            //Send the data to the callback function
            receive_callback(received_data);
        }
        
        TWI0.SSTATUS |= TWI_DIF_bm;
    }
}