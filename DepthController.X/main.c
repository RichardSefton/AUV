/**
 * This is the depth controller for the submarine. 
 * 
 * This is a stepper motor connected to a convertor of my own design to convert
 * linear to rotary force. Its basically a screw connected to a large nut with a housing for the syringe 
 * plunger. 
 * 
 * So basically this controller needs to control the stepper motor in two directions (forward
 * and reverse), and also make sure the motor does not turn beyond a certain point in either direction. 
 * 
 * Basically we need a way to track the position of the nut. 
 */

//Lets get the basics out the way. 
#define F_CPU 3333333UL
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

void MainClkCtrl(void);

/**
 * Need a way to track the position. I'm thinking an array. As the motor turns, 
 * every second or so we can either increment or decrement the array based on direction. 
 * 
 * If the array is at the start of end, we can't progress. It would be neat. As long 
 * as theres no reset of the device. 
 * 
 * Option 2 would be a wire and a contact plate so we know where at the start or the
 * end of the rail.
 * 
 * I think both. So in cases where the device has no power at non-start or stop position its 
 * reference isn't completely lost. 
 * 
 * To traverse the array we'll have a Timer Counter that will increment the index after a 
 * set period
 */
volatile int pos[1024];
void PosSetup(void);
void BufferSetup(void);
void RTCSetup(void);
void PosDec(void);
void PosInc(void);

/*
 * Motor driver. We can steal this from the midterm prototype. which in itself was 
 * stolen from some forum describing how to use stepper motors with arduino. I think it 
 * was the original stepper motor datasheet but this was a while ago. - I just read its
 * comments. It was from the datasheet
 * 
 * So we need a digital write function
 * 
 * We also need to rejig the pins its using. 
 * 
 * And have a setup function to setup the pins 
 */
void MotorSetup(void);
void StepperMotorStep(uint8_t);
void digitalWrite(uint8_t, uint8_t);
#define HIGH 0x01
#define LOW 0x00
//Contols the step
volatile int8_t step = 0x00; //needs to be signed so it can slip into negative values

/**
 * Need a way to control the direction. We'll use some constants and a direction variable
 * 
 * There are 3 possible directions. UP, DOWN and NONE
 */
#define UP 0
#define NONE 1
#define DOWN 2
volatile uint8_t dir = 1;

int main(void)
{
    MainClkCtrl();
    PosSetup(); 
    BufferSetup();
    
    while(1)
    {
        //1 is the marker number. If the element is 1, it means the device thinks the 
        //nut is in that position. so we won't go any further. 
        if (dir == UP && pos[1023] != 1)
        {
            stepperMotorStep(step);
            step--;
        }
        else if (dir == DOWN && pos[0] != 1)
        {
            stepperMotorStep(step);
            step++;
        }
        
        //Keep the step in bounds
        if(step > 0x07)
        {
            step = 0x00;
        }

        if(step < 0x00)
        {
            step = 0x07;
        }
        
        _delay_us(750); //controls the speed. Adjust as needed. 
    }
    
    return 0;
}

/**
 * I think I've mentioned this in a previous part. This MCU has a default clock speed
 * of 3.33MHz. (20MHz with a default prescaler of 6). These settings are fine so we're 
 * just going to make it explicit. 
 */
void MainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc | CLKCTRL_CLKOUT_bm);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
}

/*
 * Basically we're going to 0 the array and make the midpoint (512) 1. 
 * It will sort itself out in travel with the buffer
 */
void PosSetup(void)
{
    for (int i = 0; i < 1024; i++)
    {
        pos[i] = 0;
    }
    pos[512] = 1;
}

/**
 Buffer setup is the setup function for the start/stop plate we can use to reset the array to
 * make sure we don't go out of bounds.
 */
void BufferSetup(void)
{
    //Pins to avoid are 15(PB1) and 16(PB0) as these are SDA and SCL respectively. 
    //also 23 (PA0) is the UPDI pin. 
    
    //Going to use 6(PA5), 7(PA6). These need to be inputs to detect voltage
    PORTA.DIR &= ~(PIN5_bm | PIN6_bm);
    
    //Gonna need the interrupts on these
    PORTA.PIN5CTRL |= PORT_ISC_RISING_gc;
    PORTA.PIN6CTRL |= PORT_ISC_RISING_gc;
}

/**
 * Setup the RTC to count periods of time to control the motor position
 */
void RTCSetup(void)
{
    
}

/**
 * Pos mover. 
 */
void PosDec(void)
{
    int index = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (pos[i] == 1)
        {
            index = i;
        }
        pos[i] = 0;
    }
    pos[index - 1] = 1;
}
void PosInc(void)
{
    int index = 0;
    for (int i = 0; i < 1024; i++)
    {
        if (pos[i] == 1)
        {
            index = i;
        }
        pos[i] = 0;
    }
    pos[index + 1] = 1;
}

/**
 * Need to setup the pins for the stepper motor. Because of the digital write function 
 * we need to keep them on the same port. 
 * 
 * We'll use port c. 
 * 17 - PC0
 * 18 - PC1
 * 19 - PC2
 * 20 - PC3
 */
void MotorSetup(void)
{
    PORTC.DIR |= PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
}

/*
 * This function will drive the stepper motor. The sequance depends on the step
 */
void stepperMotorStep(int8_t step)
{
    //This is basically from the datasheet http://eeshop.unl.edu/pdf/Stepper+Driver.pdf
    //Made the digitalWrite function to keep it inline. 
    switch (step)
    {
        case 0:
            digitalWrite(PIN0_bm, LOW);
            digitalWrite(PIN1_bm, LOW);
            digitalWrite(PIN2_bm, LOW);
            digitalWrite(PIN3_bm, HIGH);
            break;
        case 1:
            digitalWrite(PIN0_bm, LOW);
            digitalWrite(PIN1_bm, LOW);
            digitalWrite(PIN2_bm, HIGH);
            digitalWrite(PIN3_bm, HIGH);
            break;
        case 2:
            digitalWrite(PIN0_bm, LOW);
            digitalWrite(PIN1_bm, LOW);
            digitalWrite(PIN2_bm, HIGH);
            digitalWrite(PIN3_bm, LOW);
            break;
        case 3:
            digitalWrite(PIN0_bm, LOW);
            digitalWrite(PIN1_bm, HIGH);
            digitalWrite(PIN2_bm, HIGH);
            digitalWrite(PIN3_bm, LOW);
            break;
        case 4:
            digitalWrite(PIN0_bm, LOW);
            digitalWrite(PIN1_bm, HIGH);
            digitalWrite(PIN2_bm, LOW);
            digitalWrite(PIN3_bm, LOW);
            break;
        case 5:
            digitalWrite(PIN0_bm, HIGH);
            digitalWrite(PIN1_bm, HIGH);
            digitalWrite(PIN2_bm, LOW);
            digitalWrite(PIN3_bm, LOW);
            break;
        case 6:
            digitalWrite(PIN0_bm, HIGH);
            digitalWrite(PIN1_bm, LOW);
            digitalWrite(PIN2_bm, LOW);
            digitalWrite(PIN3_bm, LOW);
            break;
        case 7:
            digitalWrite(PIN0_bm, HIGH);
            digitalWrite(PIN1_bm, LOW);
            digitalWrite(PIN2_bm, LOW);
            digitalWrite(PIN3_bm, HIGH);
            break;
    }
}
void digitalWrite(uint8_t pin, uint8_t value)
{
    if (value == LOW)
    {
        PORTC.OUTCLR = pin;
    }
    else
    {
        PORTC.OUTSET = pin;
    }
}

ISR(PORTA_PORT_vect)
{
    //Two pins that could trigger this. Need to handle each one differently
    //5 We'll have as the near end
    if (PORTA.INTFLAGS & PIN5_bm) {
        //I know normally we would use the size of the array but in c we need to calculate this. 
        //Its sizeof the array / sizeof the first element. We know the size and its not going to change. 
        //so we'll just use that..
        for (int i = 0; i < 1024; i++)
        {
            pos[i] = 0;
        }
        pos[1023] = 1;
        
        //Clear the flag so it can be raised in the future
        PORTA.INTFLAGS = PIN5_bm;
    }

    if (PORTA.INTFLAGS & PIN6_bm) {
        for (int i = 0; i < 1024; i++)
        {
            pos[i] = 0;
        }
        pos[0] = 1;
        
        //Clear the flag so it can be raised in the future
        PORTA.INTFLAGS = PIN6_bm;
    }
}