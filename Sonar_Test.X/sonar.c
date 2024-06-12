#define F_CPU 20000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C_Client.h"

#define ADDR 0x6f
#define PWM_FREQUENCY 40000UL
#define PULSE_DURATION_US 200  // Duration of the pulse in microseconds

void MainClkCtrl(void);
void SetupPins(void);
void SetupTCA(void);
void EnableWO(void);
void DisableWO(void);
void SetupRTC(void);
void SetupTCB0(void);

void I2CCallback(uint8_t);

uint8_t enableDistCalc = 0x00;
volatile uint16_t start_time = 0;
volatile uint16_t end_time = 0;
volatile float distance = 0;

int main()
{
    MainClkCtrl();
    SetupPins();
    SetupTCA();
//    EnableWO();
    SetupRTC();
    SetupTCB0();
    
    PORTA.OUTSET |= PIN2_bm;
    
    I2C_Client_InitI2C(ADDR, I2CCallback);
    
    sei();
    
    while(1)
    {
        // Main loop does nothing, waiting for interrupts
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);  // No prescaler
}

void SetupPins(void)
{
    PORTA.DIR &= ~(PIN1_bm);
    PORTA.PIN1CTRL |= PORT_ISC_RISING_gc;

    PORTB.DIR |= PIN3_bm | PIN4_bm; // PWM Output on PB3 and PB4
    
    PORTMUX.TCAROUTEA = PORTMUX_TCA00_ALT1_gc | PORTMUX_TCA01_ALT1_gc;
    
    PORTA.DIR |= PIN2_bm;
}

void SetupTCA(void)
{
    uint16_t period = (F_CPU / PWM_FREQUENCY) - 1;

    TCA0.SINGLE.PER = period;
    TCA0.SINGLE.CMP0 = period / 2; // 50% duty cycle for WO0
    TCA0.SINGLE.CMP1 = period / 2; // 50% duty cycle for WO1

    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
}

void EnableWO(void)
{
    enableDistCalc = 0x00;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void DisableWO(void)
{
    enableDistCalc = 0x01;
    TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm);
}

void SetupRTC(void) 
{
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    while(RTC.STATUS & RTC_PERBUSY_bm);
    RTC.PER = 32768;

    RTC.INTCTRL = RTC_OVF_bm;

    while(RTC.CTRLA & RTC_PERBUSY_bm);
    RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc;
}

void SetupTCB0(void) 
{
    TCB0.CTRLB |= TCB_CNTMODE_SINGLE_gc; // Single-shot mode
    TCB0.INTCTRL = TCB_CAPT_bm; // Enable capture interrupt
    TCB0.CCMP = 0xFFFF; // Pulse duration in ticks (100ns per tick at 20MHz with DIV2 prescaler)
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm; // Set clock prescaler

    // Reintroduce event system to tie TCB0 to TCA
    EVSYS.CHANNEL0 = EVSYS_CHANNEL0_TCA0_OVF_LUNF_gc; // Use TCA0 overflow as event generator
    EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL0_gc; // Route event channel to TCB0 capture
}

void I2CCallback(uint8_t data)
{
    if (data == 0xff)
    {
        start_time = RTC.CNT;
        RTC.CTRLA |= RTC_RTCEN_bm;
        EnableWO();
        TCB0.CNT = 0; // Reset TCB0 counter
//        TCB0.CTRLA |= TCB_ENABLE_bm; // Start TCB0
    }
}

ISR(TCB0_INT_vect) 
{
    TCB0.INTFLAGS = TCB_CAPT_bm;
    DisableWO();
}

ISR(PORTA_PORT_vect)
{
    if (enableDistCalc == 0x01)
    {
        end_time = RTC.CNT;

        uint16_t elapsed_ticks;
        if (end_time >= start_time)
        {
            elapsed_ticks = end_time - start_time;
        }
        else
        {
            elapsed_ticks = (0xFFFF - start_time) + end_time + 1; // Handle overflow
        }

//        float elapsed_time = (float)elapsed_ticks / 32.768; // Convert to seconds
        float elapsed_time = (float)(end_time) / 32.768;
        float speed_of_sound = 0.0343; // Speed of sound in cm/us
        distance = (elapsed_time * speed_of_sound) / 2.0; // Divide by 2 for round trip

        if (distance > 1.0)
        {
            PORTA.INTFLAGS |= PIN1_bm;
        }
        enableDistCalc = 0x00;
    }
    
    PORTA.INTFLAGS |= PIN1_bm;
}

ISR(RTC_CNT_vect)
{
    RTC.INTFLAGS = RTC_INTFLAGS; //Should reset the flags
    RTC.CTRLA &= ~(1 << RTC_RTCEN_bm);
    RTC.CNT = 0;
}