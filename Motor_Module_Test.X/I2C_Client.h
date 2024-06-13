#ifndef I2C_CLIENT_H
#define I2C_CLIENT_H

#include <avr/io.h>
#include <stddef.h>  // Include stddef.h for NULL
#include <avr/interrupt.h>

// Type definition for the receive callback function
typedef void (*I2C_ReceiveCallback)(uint8_t data);
typedef uint8_t (*I2C_TransmitCallback)(void);

// Function declarations
void I2C_Client_InitI2C(uint8_t, I2C_ReceiveCallback, I2C_TransmitCallback);
uint8_t I2C_Client_ReadData(void);
void I2C_Client_WriteData(uint8_t data);

#endif // I2C_CLIENT_H