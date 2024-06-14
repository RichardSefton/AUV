#ifndef TWI_H
#define TWI_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h> 

/*
 * Macros used to help calculate the BAUD rate for the TWI/I2C Protocol. 
 * 
 * The formulas can be found on the datasheet.
 */
//only if not defined. 
#ifndef F_CPU
    #define F_CPU 3333333UL
#endif
#define TWI_FREQ 100000UL //1MHz
#define TWI_BAUD(F_SCL, T_RISE) ((((F_CPU / (F_SCL)) - 10) / 2) - (T_RISE / 2))

/*
 * Callback functions for the Slave initialisation of the protocol. This will be 
 * called by the ISR for Slave received requests so the code that implements this 
 * library can provide functionality without having to customise the library itself,
 * and call helper functions outside the scope of the library. 
 */
typedef void (*I2C_ReceiveCallback)(uint8_t data);
typedef uint8_t (*I2C_TransmitCallback)(void);

// TWI/I2C Master functions
/**
 * This function initialises the TWI/I2C Protocol on a device designated as the master
 * where it will be able to specify and send requests to the connected slave devices. 
 */
void TWI_Master_Init(void);

/**
 * This function will put the protocol in a state where it is able to either read or 
 * write data. It will target a slave, and specify if its a read or write operation. 
 * 
 * Read         0x01
 * Write        0x00
 * 
 * @param address       The Slave address to be contacted
 * @param read          The mode of operation (Read, Write)
 */
void TWI_Master_Start(uint8_t address, uint8_t read);

/**
 * This function will perform the Write operation. The function will write data to the
 * MDATA buffer which should transfer to the Slave. 
 * 
 * At the moment, this library will only really send a single uint8_t (one byte) of 
 * data. 
 * 
 * @param data          Data to be sent. 
 */
void TWI_Master_Write(uint8_t data);

/**
 * This function will read from the MDATA buffer for data sent from the Slave on Read
 * operations. The device will then send an ACK (0) to confirm it is ready for more 
 * data. 
 * 
 * @return              Data received from the Slave
 */
uint8_t TWI_Master_Read_ACK(void);

/**
 * This function will read from the MDATA buffer for data sent from the Slave on Read
 * operations. The device will then send an NACK (1) to tell the Slave it can't accept 
 * more data. 
 * 
 * @return              Data received from the Slave
 */
uint8_t TWI_Master_Read_NACK(void);

/**
 * This function issues a stop command to the protocol to cease transmissions. 
 * 
 * This should then put the bus in an IDLE state. 
 */
void TWI_Master_Stop(void);

// I2C Slave functions
/**
 * This function will initialise the I2C Protocol on Slave devices. It will set the 
 * address to the SADDR buffer, and set the callback functions so they can be called
 * by the ISR when an address is confirmed and the mode of operation is identified. 
 * 
 * In Read operations the data should be returned by the TX callback function so the ISR
 * can pick it up and continue. In the current state we can't just leave this function hanging. 
 * It MUST return a value that will be sent so we can't rely on interrupts to populate a value.
 * 
 * In Write operations the data from the Master will be sent to the RX callback function.  
 * @param address       The address of the Slave device. Requests from the Master will work on this device. 
 * @param               The RX Callback function
 * @param               The TX Callback function
 */
void TWI_Slave_Init(uint8_t address, I2C_ReceiveCallback, I2C_TransmitCallback);

/**
 * This function will send data to the Master device. It will write data to the SDATA
 * buffer which will be transmitted to the Master. 
 * 
 * @param data          The data to be sent
 */
void TWI_Slave_Write(uint8_t data);

/**
 * This function will read data from the Master device. It will get the data sent by 
 * reading from the SDATA buffer. 
 * 
 * @return              The received data
 */
uint8_t TWI_Slave_Read(void);

#endif // TWI_H