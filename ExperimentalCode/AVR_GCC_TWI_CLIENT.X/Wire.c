#include "Wire.h"

static uint8_t txAddress;
static uint8_t rxBuffer[32];
static uint8_t rxBufferIndex = 0;
static uint8_t rxBufferLength = 0;

// Function pointers for callbacks
static void (*onReceiveCallback)(int) = 0;
static void (*onRequestCallback)(void) = 0;

void Wire_begin(void) {
    // Set the baud rate for 100kHz SCL frequency
//    TWI0.MBAUD = TWI_BAUD(100000UL, 0);
    TWI0.MBAUD = 12;
    // Enable TWI, enable smart mode, and enable master mode
    TWI0.MCTRLA = TWI_ENABLE_bm | TWI_WIEN_bm | TWI_RIEN_bm;
}

void Wire_beginTransmission(uint8_t address) {
    txAddress = address << 1;
    TWI0.MADDR = txAddress;
    while (!(TWI0.MSTATUS & TWI_WIF_bm)); // Wait for write to complete
    if (TWI0.MSTATUS & TWI_ARBLOST_bm) {
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    }
}

uint8_t Wire_endTransmission(void) {
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    return 0; // Assume success
}

uint8_t Wire_requestFrom(uint8_t address, uint8_t quantity) {
    uint8_t i;

    rxBufferIndex = 0;
    rxBufferLength = quantity;
    TWI0.MADDR = (address << 1) | 0x01; // Read operation

    for (i = 0; i < quantity; i++) {
        while (!(TWI0.MSTATUS & TWI_RIF_bm)); // Wait for data reception
        rxBuffer[i] = TWI0.MDATA;
        if (i < quantity - 1) {
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc; // Send ACK
        } else {
            TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc; // Send NACK and stop
        }
    }

    return quantity;
}

void Wire_write(uint8_t data) {
    TWI0.MDATA = data;
    while (!(TWI0.MSTATUS & TWI_WIF_bm)); // Wait for data to be sent
    if (TWI0.MSTATUS & TWI_RXACK_bm) {
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    }
}

uint8_t Wire_read(void) {
    uint8_t value = 0xFF;
    if (rxBufferIndex < rxBufferLength) {
        value = rxBuffer[rxBufferIndex++];
    }
    return value;
}

// Slave Mode

void Wire_beginSlave(uint8_t address) {
    TWI0.SADDR = address << 1; // Set slave address
    TWI0.SCTRLA = TWI_ENABLE_bm | TWI_DIF_bm | TWI_APIEN_bm | TWI_PIEN_bm; // Enable TWI and interrupts
}

void Wire_onReceive(void (*function)(int)) {
    onReceiveCallback = function;
}

void Wire_onRequest(void (*function)(void)) {
    onRequestCallback = function;
}

ISR(TWI0_TWIS_vect) {
    if (TWI0.SSTATUS & TWI_DIF_bm) { // Data interrupt
        if (TWI0.SSTATUS & TWI_DIR_bm) { // Master is requesting data
            if (onRequestCallback) {
                onRequestCallback();
            }
            TWI0.SDATA = rxBuffer[rxBufferIndex++]; // Send data
        } else { // Master is sending data
            rxBuffer[rxBufferIndex++] = TWI0.SDATA; // Receive data
            if (onReceiveCallback) {
                onReceiveCallback(rxBufferIndex);
            }
        }
        TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc; // Send ACK and wait for more data
    }
    if (TWI0.SSTATUS & TWI_APIF_bm) { // Address match
        TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc; // Prepare for data reception
    }
}

