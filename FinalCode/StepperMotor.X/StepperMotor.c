#include "StepperMotor.h"

void StepperMotor_init(StepperMotor* self) {
    StepperMotor_setup();
    self->step = 0;
}

void StepperMotor_setup(void) {
    PORTC.DIR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
}

void StepperMotor_step(StepperMotor* self) {
    switch(self->step) {
        case 0:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3;
            PORTC.OUTSET |= STEP_PIN_4;
            break;     
        case 1:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2;
            PORTC.OUTSET |=  STEP_PIN_3 | STEP_PIN_4;
            break;    
        case 2:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_3;
            break;      
        case 3:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_2 | STEP_PIN_3;
            break;  
        case 4:
            PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_3 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_2;
            break;    
        case 5:
            PORTC.OUTCLR |= STEP_PIN_3 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_1 | STEP_PIN_2;
            break;
        case 6:
            PORTC.OUTCLR |= STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
            PORTC.OUTSET |= STEP_PIN_1;
            break;
        case 7:
            PORTC.OUTCLR |= STEP_PIN_2 | STEP_PIN_3;
            PORTC.OUTSET |= STEP_PIN_1 | STEP_PIN_4;
            break;
        default:
            break;
    }
}

void StepperMotor_increaseStep(StepperMotor* self) {
    self->step++;
    if (self->step > 7) {
        self->step = 0;
    }
}

void StepperMotor_decreaseStep(StepperMotor* self) {
    self->step--;
    if (self->step < 0) {
        self->step = 7;
    }
}

void StepperMotor_allStop(void) {
    PORTC.OUTCLR |= STEP_PIN_1 | STEP_PIN_2 | STEP_PIN_3 | STEP_PIN_4;
}