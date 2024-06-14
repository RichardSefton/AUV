#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "TWI.h"

#define ADDR 0x08

#define COM_SCAN 0xFF

void MainClkCtrl(void);
void SetupPins(void);
void SetupRTC(void);
//void SetupTCB0(void);
void I2C_RX_Callback(uint8_t);
uint8_t I2C_TX_Callback(void);

uint8_t waitingForEcho = 0;
uint16_t startTime = 0.0;
uint16_t endTime = 0;
float distance = 0.0;
float speedOfSound = 0.0343; // cm per microsecond
float ticks = 0.0;

int main()
{
    MainClkCtrl();
    SetupPins();
    SetupRTC();
//    SetupTCB0();
    
    TWI_Slave_Init(ADDR, I2C_RX_Callback, I2C_TX_Callback);
    
    sei();
    
    while(1)
    {
        // Main loop does nothing, waiting for interrupts
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm); // Prescaler of 6
    // F_CPU with this configuration will be 3.33MHz
}

void SetupPins(void)
{
    // Trigger
    PORTA.DIR |= PIN2_bm;
    
    // Echo
    PORTA.DIR &= ~(PIN3_bm);
//    PORTA.PIN3CTRL = PORT_ISC_FALLING_gc;
}

void SetupRTC(void) {
    // Select the 32.768 kHz internal oscillator as the RTC clock source
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    
    // Wait for synchronization
    while (RTC.STATUS & RTC_CTRLABUSY_bm);
    
    // Set the period to maximum
    RTC.PER = 0xFFFF;
    
    // Enable RTC overflow interrupt
//    RTC.INTCTRL = RTC_OVF_bm;
    
    // Enable the RTC, run in standby mode, no prescaler
    RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
}
//
//void SetupTCB0(void)
//{
//    TCB0.CCMP = 17;
//    TCB0.CNT = 0xFFFF;
//    TCB0.CTRLA |= TCB_CLKSEL_DIV2_gc; 
//    TCB0.CTRLB |= TCB_CNTMODE_INT_gc;
//
//    TCB0.INTCTRL |= TCB_CAPT_bm;
//}

void I2C_RX_Callback(uint8_t com)
{
    // Handle received I2C commands here
}

uint8_t I2C_TX_Callback(void)
{
    // Set trigger pin to high
    PORTA.OUTSET = PIN2_bm;
    _delay_us(10);  // Wait at least 10us
    PORTA.OUTCLR = PIN2_bm;  // Disable trigger
    startTime = RTC.CNT;
    
    // Wait for echo pin to go high
    while (!(PORTA.IN & PIN3_bm));
    
    // Wait for echo pin to go low
    while (PORTA.IN & PIN3_bm);
    
    // Read RTC counter value
    endTime = RTC.CNT;
    
    // Calculate distance
    ticks = (float)(endTime - startTime) * 30.5176; // Convert to microseconds
    distance = (ticks * speedOfSound) / 2;
    
    // Return the calculated distance as a byte
    return (uint8_t)distance;
}