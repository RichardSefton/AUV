/**
 * We are running this module with a faster clock speed as the response time from the 
 * ultrasonic transducers will need to be faster than normal. Speed of sound is ~4x faster
 * underwater than OTA. 
 */
#define F_CPU 20000000UL
//#define F_CPU 3333333UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C_Client.h"

#define ADDR 0x6f

//Function declarations
void MainClkCtrl(void);
void SetupPins(void);
void SetupTCA(void);
void EnableWO(void);
void DisableWO(void);
void SetupRTC(void);
void SetupTCB0(void);
void SetupTCB1(void);
void EnableTCB1(void);
void DisableTCB1(void);

void I2CCallback(uint8_t);

//This is the fequency we want the PWM signal to be at. This will be with a 50% duty cycle. 
#define PWM_FREQUENCY 40000UL

uint8_t enableDistCalc = 0x00;

int main()
{
    MainClkCtrl();
    SetupPins();
    SetupTCA();
//    SetupRTC();
    SetupTCB0();
    
    I2C_Client_InitI2C(ADDR, I2CCallback);
    
    sei();
    
    while(1)
    {
        // Main loop does nothing, waiting for interrupts
    }
    
    return 0;
}

/**
 * This function is something I normally include even if I'm depending on the default chip 
 * setup which in this case would normally be 20mhz with a prescaler dividor of 6 taking the 
 * freq down to 3333333(3.33)mHz. Its good practice to be explicit on some things. 
 * 
 * In this instance though. As mentioned we want a faster clock speed so we are clearing the bits from the MCLKCTRLB 
 * register to specify no prescaler on the 20mhz clock. 
 */
void MainClkCtrl(void) 
{
    // Set the main clock to 20MHz without prescaler
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);  // No prescaler
}
//void MainClkCtrl(void) 
//{
//    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
//    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
//}

void SetupPins(void)
{
    //PA1 is going to be the pin to detect the u/s response. If it goes high, we will
    //stop the counters and calculate the sos based on this. 
    PORTA.DIR &= ~(PIN1_bm);
    //To detect the high, we want an interrupt on rising edge. 
    PORTA.PIN1CTRL |= PORT_ISC_FALLING_gc;
    
    //PWM output. TCA0 which will ewmit the PWM signal might take over them but lets just 
    //do this to be clear these pins will be used. 
    PORTB.DIR |= PIN3_bm | PIN4_bm; // PWM Output on PB3 and PB4
    // Invert WO1 (PB4) output
    // Originally, this was meant to drive the (two opposite oscillators to charge/discharge
    //the transducer which basically operates as a capacitot). But more power was required 
    //so this is now hooked up to an LED. The PWM signal is being funneled to a mosfet with a circuit to drive
    //this now.
//    PORTB.PIN4CTRL |= PORT_INVEN_bm;
    
    // PortMUX to set TCA0 PWM on alternate pins (PB3 and PB4)
    //Using the alternate pins as the defaults we need to keep clear 
    // for TWI/I2C
    PORTMUX.TCAROUTEA = PORTMUX_TCA00_ALT1_gc | PORTMUX_TCA01_ALT1_gc;
}

void SetupTCA(void)
{
    //Period for the PWM
    uint16_t period = (F_CPU / PWM_FREQUENCY) - 1;

    // Set period and duty cycle
    TCA0.SINGLE.PER = period;
    TCA0.SINGLE.CMP0 = period / 2; // 50% duty cycle for WO0
    TCA0.SINGLE.CMP1 = period / 2; // 50% duty cycle for WO1

    // Enable compare channels WO0 (PB3) and WO1 (PB4)
    //This will output the PWM to their CMP pins. WGMode is the Waveform Generator Mode
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

    // Enable TCA0 and set clock source to CLK_PER (no prescaler)
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
}

//Simple function to enable the PWM signal (by enabling the TCA0)
void EnableWO(void)
{
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

//Simple function to Disable the PWM signal by disabling TCA0;
void DisableWO(void)
{
    TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm);
}

//THIS IS NOT HOW THE FINAL MODULE WILL BEHAVE. IT WILL NOT PERFORM ON REGULAR INTERVALS 
//BUT VIA I2C COMMAND. 
void SetupRTC(void) 
{
    // Use 1.024 kHz internal clock
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    
    // Set the period for 5 seconds (5 * 1024 = 5120)
    RTC.PER = 5120;
//    RTC.PER = 1024;
    // Enable the overflow interrupt
    //In the final module code, this interrupt code will be replaced with i2c intterrupt code. 
    RTC.INTCTRL = RTC_OVF_bm;
    // Clear any existing flags
    RTC.INTFLAGS = RTC_OVF_bm;
    // Enable the RTC
    RTC.CTRLA = RTC_RTCEN_bm;
}

/**
 * TCB0 will be used to specify how long to ping the emitting transducer for. 
 * 
 * It will be enabled when TCA0 is enabled via the internal event system in the MCU.
 * 
 */
void SetupTCB0(void) 
{
//    TCB0.CTRLB |= TCB_ASYNC_bm | TCB_CNTMODE_SINGLE_gc; // Single-shot mode
    TCB0.CTRLB |= TCB_CNTMODE_SINGLE_gc;
    TCB0.INTCTRL = TCB_CAPT_bm; //Setup the Capture interrupt
    TCB0.EVCTRL |= TCB_CAPTEI_bm; //For the event control. 
    TCB0.CCMP = 0xFFFF; // 1/0xffff of a second
    TCB0.CTRLA = TCB_CLKSEL_DIV2_gc | TCB_ENABLE_bm; // Enable TCB with clock prescaler of 2 (10000000 == 1s rather than 20000000)
    
    //TCB0 should start via the event system. Attach the TCA0 to the channel and user channel should 
    EVSYS.CHANNEL0 = EVSYS_CHANNEL0_TCA0_OVF_LUNF_gc; // Use TCA0 overflow as event generator
    EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL0_gc; // Channel 0 routed to TCB0 Capture. 
}

//TCB1 will be used to count the ticks (from the main clock) from when the u/s ping starts to a response. 
void SetupTCB1(void)
{
    //Rewgular counting mode. 
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    //Explicitly set the CNT register to 0 (starting state)
    TCB1.CNT = 0;
    //Compare buffer will be maxed out. This will be 1/0xffff of a second. 
    TCB1.CCMP = 0xFFFF;
    
    //Enable the Capture interrupt. 
    TCB1.INTCTRL |= TCB_CAPT_bm;
    
    //Use a prescaler of 2 to give the clock more time. We're not enabling the TCB1 at this point. 
    TCB1.CTRLA |= TCB_CLKSEL_DIV1_gc;
}

//Function to enable TCB1
void EnableTCB1(void)
{
    enableDistCalc = 0x01;
    TCB1.CNT = 0;
    TCB1.CTRLA |= TCB_ENABLE_bm;
}

//Simple function to disable TCB1
void DisableTCB1(void)
{
    enableDistCalc = 0x00;
    TCB1.CTRLA &= ~(TCB_ENABLE_bm);
}

void I2CCallback(uint8_t data)
{
    if (data == 0xff)
    {
        EnableTCB1();
        EnableWO(); 
    }
}

// INTERRUPTS
//Pulse counter to allow us to pulse the transducer
uint8_t pulse_counter = 0;
ISR(TCB0_INT_vect) 
{
    //Clear the Interrupt flag
    TCB0.INTFLAGS = TCB_CAPT_bm;
    //Disable the PWM signal
    DisableWO();
    
//    //We want to pulse so...
//    if (pulse_counter < 8)
//    {
//        pulse_counter++;
//        //This is putting blocking code in an ISR routine which is considered bad,
//        //but its a matter of us so we should be fine. 
//        _delay_us(25);
//        //This should start this cycle again
////        EnableWO();
//    }
//    else if (pulse_counter >= 8)
//    {
//        //Clear the pulse counter ready for the next signal. 
//        DisableWO();
//        pulse_counter = 0;
//    }

}

float distance = 0;
ISR(PORTA_PORT_vect)
{
    if (enableDistCalc == 0x01)
    {
        DisableTCB1();
        uint16_t cnt = TCB1.CNT;
        if (cnt > 0)
        {
            DisableTCB1();
//            float count = (float)cnt / (float)(15.0);
            float count = (float)(cnt);
            float speedOfSound = 0.0343;
            distance = ((float)(count) * (float)(speedOfSound)) / 2;
            if (distance > 1.0)
            {
                 PORTA.INTFLAGS |= PIN1_bm;
            }
        }
    }
    
    PORTA.INTFLAGS |= PIN1_bm;
}

//This will be replaced with I2C code. 
////We want to enable the TCA0 to emit a PWM signal to the transducer. 
//ISR(RTC_CNT_vect) 
//{    
//    RTC.INTFLAGS = RTC_OVF_bm;
//    EnableTCB1();
//    EnableWO(); 
////    EnableTCB1();
//}
