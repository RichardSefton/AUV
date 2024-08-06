#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "TWI.h"

#define MOTOR_DRIVER_ADDR 0x49
#define SONAR_ADDR 0x08

#define COM_STOP 0xFF
#define COM_START 0xAA

// LED For debugging
#define RED PIN6_bm
#define GREEN PIN5_bm
#define BLUE PIN7_bm
#define YELLOW (PIN6_bm | PIN5_bm)
#define PURPLE (PIN6_bm | PIN7_bm)
#define CYAN (PIN5_bm | PIN7_bm)
#define WHITE (PIN6_bm | PIN5_bm | PIN7_bm)

void MainClkCtrl(void);
void SetupRTC(void);
void SetupTCA(void);

// For LED Debugging
void SetupPins(void);
void ChangeColour(uint8_t);

volatile uint8_t distance = 0;
volatile uint8_t sendMotorInstruction = 0;

int main()
{
    MainClkCtrl();
    
    // Allow Slaves to init first
    _delay_ms(2000);
    
    SetupRTC();
    SetupTCA();
    
    TWI_Master_Init();
        
    SetupPins();
    
    sei();
    
    while(1)
    {
        ChangeColour(GREEN);
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
    // F_CPU with this configuration will be 3.33MHz
}

void SetupRTC(void) 
{
    RTC.CLKSEL |= RTC_CLKSEL_INT1K_gc;
    RTC.PER = 1024; //provides a 1 second interval timer
    // Clear any existing flags
    RTC.INTFLAGS |= RTC_OVF_bm;   
    // Enable the overflow interrupt
    RTC.INTCTRL |= RTC_OVF_bm;
    // Enable the RTC
    RTC.CTRLA |= RTC_RTCEN_bm;
}

void SetupTCA(void)
{
    // Set up TCA to overflow at a desired interval
    TCA0.SINGLE.PER = 0xFFFF; // Maximum period
    // Enable TCA overflow interrupt
    TCA0.SINGLE.INTCTRL |= TCA_SINGLE_OVF_bm;
    // Set TCA to Normal mode
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_NORMAL_gc;
    // Use the system clock and set prescaler
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
}

void SetupPins(void)
{
    PORTB.DIR |= RED | GREEN | BLUE;
}

void ChangeColour(uint8_t c)
{
    PORTB.OUTSET = RED | GREEN | BLUE;
    PORTB.OUTCLR = c;
}

ISR(RTC_CNT_vect)
{
    // Clear the interrupt flag
    RTC.INTFLAGS = RTC_OVF_bm; 
    
    ChangeColour(RED);
    
    // Start TWI operation
    distance = 0; //reset the distance
    TWI_Master_Start(SONAR_ADDR, 0x01); //Start TWI to sonar module 
    distance = TWI_Master_Read_NACK(); //Get the distance
    TWI_Master_Start((uint8_t)MOTOR_DRIVER_ADDR, 0x00); //Start TWI to Motor
    if (distance < 55)
    {
        TWI_Master_Write(COM_STOP); //Send motor a stop command
    }
    else
    {
        TWI_Master_Write(COM_START); //Send motor a start command. 
    }
    TWI_Master_Stop();
    
    ChangeColour(PURPLE);
}
