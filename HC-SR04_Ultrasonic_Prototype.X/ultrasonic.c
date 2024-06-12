#include <avr/io.h>
#include <avr/interrupt.h>

#include "I2C_Client.h"

#define ADDR 0xB1

#define COM_SCAN 0xFF

void MainClkCtrl(void);
void SetupPins(void);
void SetupRTC(void);
void SetupTCB0(void);
void I2CCallback(uint8_t);

uint8_t waitingForEcho = 0;
uint8_t endTime = 0;
float distance = 0.0;
float speedOfSound = 0.0343;

int main()
{
    MainClkCtrl();
    SetupPins();
    SetupRTC();
    SetupTCB0();
    
    I2C_Client_InitI2C(ADDR, I2CCallback);
    
    while(1)
    {
        
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
    //Trigger
    PORTA.DIR |= PIN2_bm;
    
    //Echo
    PORTA.DIR &= ~(PIN3_bm);
    PORTA.PIN3CTRL = PORT_ISC_FALLING_gc;
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
    TCB0.CCMP = 17;
    TCB0.CNT = 0xFFFF;
    TCB0.CTRLA |= TCB_CLKSEL_DIV2_gc; 
    TCB0.CTRLB |= TCB_CNTMODE_INT_gc;

    TCB0.INTCTRL |= TCB_CAPT_bm;
}

void I2CCallback(uint8_t com)
{
    if (com == COM_SCAN)
    {
        TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm;
        //Turn on the Trigger pin.
        PORTA.OUT |= PIN2_bm;
        //Turn on TCB so we can count to 10us
        TCB0.CTRLA |= TCB_ENABLE_bm;
    }
}

ISR(TCB0_INT_vect)
{
    if(TCB0.INTFLAGS & TCB_CAPT_bm)
    {
        TCB0.INTFLAGS = TCB_CAPT_bm;    

        if (!waitingForEcho)
        {
            PORTA.OUTCLR |= PIN1_bm;
            TCB0.CTRLA &= ~(1 << TCB_ENABLE_bp);
            waitingForEcho = 1;
            //Init the RTC
            RTC.CTRLA |= RTC_RTCEN_bm;
        }
    }
}

ISR(PORTA_PORT_vect)
{
    if (PORTA.INTFLAGS & PORT_INT_2_bm)
    {
        if (waitingForEcho && !(PORTA.IN & (1 << PIN2_bp)))
        {
            // Pin is low, record the end time and calculate distance
            endTime = RTC.CNT; // Capture the time elapsed since the trigger was stopped
            //convert the endtime from cycles to ?ms?
            
            endTime = endTime/32.768;
            if (endTime)
            {
                // Calculate distance in millimeters (distance = speed * time / 2)
                distance = ((float)(speedOfSound) * endTime) / 2.0;
                
                if (distance < (float)(2.5)) // Distance threshold in millimeters
                {

                } 


                //Disable the RTC
                RTC.CTRLA &= ~(1 << RTC_RTCEN_bm);
                RTC.CNT = 0;

            }   

            // Reset waitingForEcho for the next cycle
            waitingForEcho = 0;

        }
        // Clear the interrupt flag
        PORTA.INTFLAGS = PORT_INT_2_bm;
    }
}

/**
 * RTC Interrupt to reenable the ability to trigger ultrasonic sensor
 */
ISR(RTC_CNT_vect)
{
    RTC.INTFLAGS = RTC_INTFLAGS; //Should reset the flags
//    MoveServo(312);
    waitingForEcho = 0; //Reenable the ability to trigger the ultrasonic sensor
    //Disable the RTC
    RTC.CTRLA &= ~(1 << RTC_RTCEN_bm);
    RTC.CNT = 0;
}