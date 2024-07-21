/**
 * OK. So since we're having SO MUCH TROUBLE WITH MY OWN SONAR MODULE!
 * 
 * I have hacked an SR04 module by replacing the standard transducers with my alumininium 
 * ones (since they're waterproof. 
 * 
 * I am NOT expecting this to work and if it does I'm not expecting it to be efficient. 
 * The two different transducers (standard and mine) will likely have very different 
 * power requirements. But I am expecting to see hopefully something. 
 * 
 */


/**
 * These modules are very simple. Pull the trigger pin high for > 10uS. 
 * 
 * Count the time for the echo pin to become low. 
 * 
 * Right now we only care about pulling it high. I can verify on an oscilloscope. 
 * 
 * Also not doing this with interrupt. Quick and dirty in the main loop will suffice. 
 * 
 */

/**
 * UPDATE
 * 
 * It may have worked. At first glance on the scope, channel 2 was showing no signs of 
 * activity. but when adjusting the trigger on this channel, the waveform shows it 
 * increasing to 5v. 
 * 
 * There is hope. 
 * 
 * However, this should be a fallback plan incase my sonar module is a complete failure
 * (I think I'm close now I understand opamps a little better). We will definately have
 * an uncontrollable blindspot on this configuration. 
 */

#include <avr/io.h>
#define F_CPU 3333333UL
#include <avr/delay.h>
#include <avr/interrupt.h>

void SetupPins(void);
void TriggerOn(void);
void TriggerOff(void);

/**
 * Lets add a Timer Counter to measure the response time. 
 * 
 */
void SetupTCA(void);

float t = 0.0;

int main() 
{
    SetupPins();
    SetupTCA();

    //Enable the interrupts
    sei();
    
    while(1) {
        TCA0.SINGLE.CNT = 0; //Reset the counter
        TriggerOn();
        _delay_us(10);
        TriggerOff();
        
        //wait for the next loop
        _delay_ms(1000);
    }
    
    return (0);
}

void SetupPins(void) 
{
    //PA6 (Pin 7 on the chip) will be the trigger
    PORTA.DIR |= PIN6_bm;
    //PA7 (Pin 8 on the chip) will be the echo
    PORTA.DIR &= ~(PIN7_bm);
    //Attach an interrupt to the echo pin
    PORTA.PIN7CTRL |= PORT_ISC_FALLING_gc; //We want the falling edge. 
}

void TriggerOn(void)
{
    PORTA.OUTSET |= PIN6_bm; 
}

void TriggerOff(void)
{
    PORTA.OUTCLR |= PIN6_bm;
}

void SetupTCA(void)
{   
    TCA0.SINGLE.PER = 0xFFFF; //MAX Period
     
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
}

//Interrupt for when the interrupt falls
ISR(PORTA_PORT_vect)
{
    //Stop the count
    uint16_t count = TCA0.SINGLE.CNT;
    
    //Just so something with the count so the debugger has it in scope
    float SoS = 0.0343; //cm/ms
    float time = (count) /(3333333); 
    float dist = (SoS * time) / 2;
    
    float this = dist + 1.0;
    
    
    
    //Clear the Interrupt flag so the ISR can be triggered again. 
    //Doing it last so clearing it doesn't mess with our variables next time the interrupt should be called. 
    //We can then run the calculation with Speed of Sound scaled for whichever unit we need (uS most likely)
    PORTA.INTFLAGS |= PIN7_bm;
}
