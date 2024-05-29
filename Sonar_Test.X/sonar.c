#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 20000000UL  // Define your CPU frequency here

void MainClkCtrl(void);
void SetupPins(void);
void SetupTCA(void);
void EnableWO(void);
void DisableWO(void);

#define PWM_FREQUENCY 40000UL  // Desired PWM frequency (40kHz)

int main()
{
    MainClkCtrl();
    SetupPins();
    SetupTCA();
    EnableWO();
    
    sei();
    
    while(1)
    {
        // Main loop does nothing, waiting for interrupts
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    // Set the main clock to 20MHz without prescaler
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x00);  // No prescaler
}

void SetupPins(void)
{
    PORTB.DIR |= PIN3_bm | PIN4_bm; // PWM Output on PB3 and PB4
        
    // Invert WO1 (PB4) output
    PORTB.PIN4CTRL = PORT_INVEN_bm;
    
    // PortMUX to set TCA0 PWM on alternate pins (PB3 and PB4)
    PORTMUX.TCAROUTEA = PORTMUX_TCA00_ALT1_gc | PORTMUX_TCA01_ALT1_gc;
}

void SetupTCA(void)
{
    // Calculate the period value for the desired frequency
    uint16_t period = (F_CPU / PWM_FREQUENCY) - 1;

    // Set period and duty cycle
    TCA0.SINGLE.PER = period;
    TCA0.SINGLE.CMP0 = period / 2; // 50% duty cycle for WO0
    TCA0.SINGLE.CMP1 = period / 2;

    // Enable compare channels WO0 (PB3) and WO1 (PB4)
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;

    // Enable TCA0 and set clock source to CLK_PER (no prescaler)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;

}

void EnableWO(void)
{
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void DisableWO(void)
{
    TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
}

// TCA0 Compare channel 0 interrupt
ISR(TCA0_CMP0_vect)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm; // Clear the interrupt flag
}

// TCA0 Overflow interrupt
ISR(TCA0_OVF_vect)
{
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Clear the interrupt flag
}
