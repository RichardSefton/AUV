#ifndef I2C_HOST_H
#define I2C_HOST_H

#ifndef F_CPU
    #define F_CPU 3333333UL
#endif
#include <avr/io.h>

#define I2C_CHECK_START() (TWI_WIF_bm & TWI_CLKHOLD_bm) 
#define I2C_CHECK_WRITE() (TWI_WIF_bm)
#define I2C_CHECK_NACK() (TWI_WIF_bm & TWI_RXACK_bm)

typedef struct {
    // Add any necessary fields here, if needed
} I2C_Host;

void I2C_Host_InitPins(void);

void I2C_Host_InitI2C(void);

uint8_t I2C_Host_CalcBaud(void);

struct BAUD_TIMING_t{
    double T_LOW;
    double T_HIGH;
    double T_OF;
    double T_R;
};

double I2C_Host_CalcBaud_F_SCL(void);

double I2C_Host_CalcBaud_BAUD(double);

uint8_t I2C_Host_Start(uint8_t, uint8_t);

void I2C_Host_WriteData(uint8_t);

void I2C_Host_Stop(void);

#endif // I2C_HOST_H
