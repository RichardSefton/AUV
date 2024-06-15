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
//uint8_t ping = 0;
volatile uint8_t sendMotorInstruction = 0;

int main()
{
    MainClkCtrl();
    
    // Allow Slaves to init
    _delay_ms(2000);
    
    SetupRTC();
    SetupTCA();
    
    TWI_Master_Init();
        
    SetupPins();
    
    sei();
    
    while(1)
    {
        // Main loop can be used for other tasks
        ChangeColour(GREEN);
        
//        if (sendMotorInstruction == 1)
//        {
//            TWI_Master_Stop();
//            TWI_Master_Start((uint8_t)MOTOR_DRIVER_ADDR, 0x00);
//            TWI_Master_Write(COM_START);
//            sendMotorInstruction = 0;
//        }
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm); // Prescaler of 6
    // F_CPU with this configuration will be 3.33MHz
}

void SetupRTC(void) 
{
    // Use 1.024 kHz internal clock
    RTC.CLKSEL |= RTC_CLKSEL_INT1K_gc;
    
    // Set the period for 5 seconds (5 * 1024 = 5120)
    RTC.PER = 1024;
    
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
    
    // Disable the TCA initially
//    TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
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
    distance = 0;
    TWI_Master_Start(SONAR_ADDR, 0x01);
    distance = TWI_Master_Read_NACK();
//    TWI_Master_Stop();
    TWI_Master_Start((uint8_t)MOTOR_DRIVER_ADDR, 0x00);
    if (distance < 55)
    {
        TWI_Master_Write(COM_STOP);
    }
    else
    {
        TWI_Master_Write(COM_START);
    }
    TWI_Master_Stop();
//    sendMotorInstruction = 0;
//    TWI_Master_Stop();
    
    ChangeColour(PURPLE);

    // Enable TCA
//    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

ISR(TCA0_OVF_vect)
{    
    // Clear the TCA overflow interrupt flag
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    // Disable TCA after operation is complete
    TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
    sendMotorInstruction = 1;
    // Perform TWI write operation
    ChangeColour(BLUE);
//    TWI_Master_Start(MOTOR_DRIVER_ADDR, 0x00);
//    TWI_Master_Write(COM_START);
//    TWI_Master_Stop();
//    
//    if (distance < 55)
//    {
//        TWI_Master_Write(COM_STOP);
//    }
//    else
//    {
//        TWI_Master_Write(COM_START);
//    }
    
//    TWI_Master_Stop();
}
