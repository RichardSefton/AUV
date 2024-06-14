#ifndef I2C_HOST_H
#define I2C_HOST_H

#ifndef F_CPU
    #define F_CPU 3333333UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define I2C_CHECK_WRITE() (TWI_WIF_bm & TWI_CLKHOLD_bm)
#define I2C_CHECK_READ() (TWI_RXACK_bm)
#define I2C_CHECK_NACK() (TWI_WIF_bm & TWI_RXACK_bm)
#define I2C_CHECK_ARBLOST() (TWI_WIF_bm & TWI_ARBLOST_bm)
#define I2C_CHECK_BUSERR() (TWI_BUSERR_bm)
#define I2C_CHECK_IDLE() (TWI_BUSSTATE_IDLE_gc)
#define I2C_CHECK_WRITE_COMP() (TWI_WIF_bm)
#define I2C_CHECK_READ_ALLOWED() (TWI_RIF_bm & TWI_CLKHOLD_bm)

typedef struct {
    // Add any necessary fields here, if needed
} I2C_Host;

// Type definition for the receive callback function
//typedef void (*I2C_ReceiveCallback)(uint8_t data);

void I2C_Host_InitI2C(void);

uint8_t I2C_Host_Start(uint8_t, uint8_t);
uint8_t I2C_Host_Start_R(uint8_t, uint8_t, uint8_t);

void I2C_Host_WriteData(uint8_t);

uint8_t I2C_Host_ReadData(void);

void I2C_Host_Stop(void);

#endif // I2C_HOST_H
