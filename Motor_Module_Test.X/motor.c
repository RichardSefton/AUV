#define F_CPU 3333333UL
#include <avr/io.h>
#include <util/delay.h>

void MainClkCtrl(void);
void SetupPins(void);
void digitalWrite(uint8_t, uint8_t);

#define HIGH 0x01
#define LOW 0x00

void stepperMotorStep(uint8_t step);

int main()
{
    MainClkCtrl();
    SetupPins();
    
    uint8_t step = 0x00;
    uint8_t dir = 0x01;
    
    while(1)
    {
        stepperMotorStep(step);
        
        if(dir == 0x01)
        {
            step++;
        }
        else
        {
            step--;
        }
        
        if(step > 0x07)
        {
            step = 0x00;
        }
        
        if(step < 0x00)
        {
            step = 0x07;
        }
        
        _delay_us(750); // Adjust delay as needed
    }
    
    return 0;
}

void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm); // Prescaler of 6
    // F_CPU with this configuration will be 3.33MHz
}

void SetupPins()
{
    PORTA.DIR |= PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    if (value == LOW)
    {
        PORTA.OUTCLR = pin;
    }
    else
    {
        PORTA.OUTSET = pin;
    }
}

void stepperMotorStep(uint8_t step)
{
    switch (step)
    {
        case 0:
            digitalWrite(PIN4_bm, LOW);
            digitalWrite(PIN5_bm, LOW);
            digitalWrite(PIN6_bm, LOW);
            digitalWrite(PIN7_bm, HIGH);
            break;
        case 1:
            digitalWrite(PIN4_bm, LOW);
            digitalWrite(PIN5_bm, LOW);
            digitalWrite(PIN6_bm, HIGH);
            digitalWrite(PIN7_bm, HIGH);
            break;
        case 2:
            digitalWrite(PIN4_bm, LOW);
            digitalWrite(PIN5_bm, LOW);
            digitalWrite(PIN6_bm, HIGH);
            digitalWrite(PIN7_bm, LOW);
            break;
        case 3:
            digitalWrite(PIN4_bm, LOW);
            digitalWrite(PIN5_bm, HIGH);
            digitalWrite(PIN6_bm, HIGH);
            digitalWrite(PIN7_bm, LOW);
            break;
        case 4:
            digitalWrite(PIN4_bm, LOW);
            digitalWrite(PIN5_bm, HIGH);
            digitalWrite(PIN6_bm, LOW);
            digitalWrite(PIN7_bm, LOW);
            break;
        case 5:
            digitalWrite(PIN4_bm, HIGH);
            digitalWrite(PIN5_bm, HIGH);
            digitalWrite(PIN6_bm, LOW);
            digitalWrite(PIN7_bm, LOW);
            break;
        case 6:
            digitalWrite(PIN4_bm, HIGH);
            digitalWrite(PIN5_bm, LOW);
            digitalWrite(PIN6_bm, LOW);
            digitalWrite(PIN7_bm, LOW);
            break;
        case 7:
            digitalWrite(PIN4_bm, HIGH);
            digitalWrite(PIN5_bm, LOW);
            digitalWrite(PIN6_bm, LOW);
            digitalWrite(PIN7_bm, HIGH);
            break;
    }
}
