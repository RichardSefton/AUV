#ifndef WIRE_H
#define WIRE_H

#include <avr/io.h>

// Set SCL frequency to 100kHz assuming a 4MHz CPU clock
#define TWI_BAUD(F_SCL, T_RISE) (((((F_CPU / (F_SCL)) - 10) / 2) + ((F_CPU * T_RISE) / 1000000)) / 2)

#ifndef F_CPU
#define F_CPU 3333333UL
#endif

void Wire_begin(void);
void Wire_beginTransmission(uint8_t address);
uint8_t Wire_endTransmission(void);
uint8_t Wire_requestFrom(uint8_t address, uint8_t quantity);
void Wire_write(uint8_t data);
uint8_t Wire_read(void);

// Slave mode functions
void Wire_beginSlave(uint8_t address);
void Wire_onReceive(void (*function)(int));
void Wire_onRequest(void (*function)(void));

#endif
