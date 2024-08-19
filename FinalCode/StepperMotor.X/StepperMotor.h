#ifndef STEPPERMOTOR_H
#define	STEPPERMOTOR_H

#ifndef F_CPU
#define F_CPU 3333333UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "Common.h"

/*
 * Lets apply the pins to some defines
 * 
 * 
 * 
 * StepperMotor
 * 17 - PC0
 * 18 - PC1
 * 19 - PC2
 * 20 - PC3
 * 
 * Going to use 6(PA5), 7(PA6). These need to be inputs to detect voltage
 * Will also need interrupts on these and pullup enabled because they're floating
 * 
 * Button we'll put on PB2
 * Also we'll add the pullup because while theres no voltage its also floating. 
*/
extern enum {
    STEP_PIN_1 = PIN0_bm,
    STEP_PIN_2 = PIN1_bm,
    STEP_PIN_3 = PIN2_bm,
    STEP_PIN_4 = PIN3_bm
} StepperPins;

typedef struct {
    i8 step;
} StepperMotor;

void StepperMotor_init(StepperMotor*);
void StepperMotor_setup(void);
void StepperMotor_step(StepperMotor*);
void StepperMotor_increaseStep(StepperMotor*);
void StepperMotor_decreaseStep(StepperMotor*);
void StepperMotor_allStop(void);

#endif	/* STEPPERMOTOR_H */

