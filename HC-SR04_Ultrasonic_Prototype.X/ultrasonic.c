#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "I2C_Client.h"

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
    
    I2C_Client_InitI2C(ADDR, I2C_RX_Callback, I2C_TX_Callback);
    
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
    
    
    // Turn on TCB so we can count to 10us
//    distance = 0.0;
//    
//    TCB0.CTRLA |= TCB_ENABLE_bm;
//    
//    // Turn on the Trigger pin
//    PORTA.OUTSET |= PIN2_bm;
//
//    return 0x00; // Placeholder return value
}

//ISR(TCB0_INT_vect)
//{
//    if (TCB0.INTFLAGS & TCB_CAPT_bm)
//    {   
//        if (waitingForEcho == 0)
//        {
//            // Disable TCB0
//            TCB0.CTRLA &= ~TCB_ENABLE_bm;
//            
//            // Turn off the Trigger pin
//            PORTA.OUTCLR |= PIN2_bm;
//
//            // Enable the RTC to start timing
//            RTC.CNT = 0; // Reset the counter
//            RTC.CTRLA |= RTC_RTCEN_bm;
//            
//            waitingForEcho = 1;
//        }
//        
//        TCB0.INTFLAGS = TCB_CAPT_bm; 
//    }
//}

//ISR(PORTA_PORT_vect)
//{
//    if (PORTA.INTFLAGS & PIN3_bm)
//    {
//        if (waitingForEcho == 1)
//        {
//            // Reset waitingForEcho for the next cycle
//            waitingForEcho = 0;
//            
//            // Pin is low, record the end time and calculate distance
//            endTime = RTC.CNT; // Capture the time elapsed since the trigger was stopped 
//            
//            //time in microseconds
//            ticks = (float)endTime * 30.5176;
//
//            // Calculate the distance
//            distance = ((float)(ticks) * speedOfSound) / 2.0;
//
//            I2C_Client_WriteData((uint8_t)(distance)); // Send distance as a single byte
//
//            // Disable the RTC
//            RTC.CTRLA &= ~RTC_RTCEN_bm;
//            RTC.CNT = 0;
//        }
//
//        // Clear the interrupt flag
//        PORTA.INTFLAGS |= PIN3_bm;
//    }
//}

/**
 * RTC Interrupt to reenable the ability to trigger ultrasonic sensor
 */
ISR(RTC_CNT_vect)
{
    RTC.INTFLAGS = RTC_OVF_bm; // Clear the overflow flag
    waitingForEcho = 0; // Re-enable the ability to trigger the ultrasonic sensor

    // Disable the RTC
    RTC.CTRLA &= ~RTC_RTCEN_bm;
}
