/**
    Based on the v1 pseudocode
 */

#define F_CPU 3333333UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "CWire.h"
#include "ShortTypes.h"
#include "AddressBook.h"
#include "AUV.h"
#include "Common.h"

u8 depth = 0;

void setup(void);
void mainClkCtrl(void);
void setupRTC(void);
u16 ping(SonarModule);
void handleDistanceResponse(SonarDirection);
u8 getDepth(void);
void dive(u8);
void raise(u8);
void commandForwardMotor(ForwardMotorDirection);
void commandSideMotors(SideMotorDirection);

TwoWire twi0;
AUV auv;

int main() {
    setup();
    
    //Give other modules time to init
    RGB(RED);
    _delay_ms(2000);
    
    TwoWire_init(&twi0, &TWI0);
    TwoWire_Master_begin(&twi0);
    
    //Wait for depth controller to enter home position.
    while(getDepth() != 1) {
        RGB(RED);
        _delay_ms(1000);
    }
    
    _delay_ms(2000);
    
    RGB(GREEN); 
    
    //DIVE, DIVE, DIVE!
    dive(0xFF);
   
    sei();
    
    while(1) {
        
    }

    return 0;
}

void setup(void) {
    mainClkCtrl();
    setupRTC();
    setupRGB();
    AUV_init(&auv);
}

void mainClkCtrl(void) 
{
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm);
    // F_CPU with this configuration will be 3.33MHz
}

void setupRTC(void) {
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    while(RTC.STATUS);
    RTC.CTRLA |= RTC_PRESCALER_DIV1_gc;
    RTC.PER = 1024;
    while (RTC.STATUS);
    RTC.INTFLAGS |= RTC_OVF_bm;
    RTC.INTCTRL |= RTC_OVF_bm;
    while (RTC.STATUS);
    RTC.CTRLA |= RTC_RTCEN_bm;
    while (RTC.STATUS);
}

u16 ping(SonarModule module) {
    AUV_setSonarMode(&auv);
    //initiate sonar
    u8 addr = 0x00;
    switch(module) {
        case LOWER: {
            addr = US_BOTTOM;
            break;
        }
        case FORWARD: {
            addr = US_FORWARD;
            break;
        }
        case LEFT: {
            addr = US_LEFT;
            break;
        }
        case RIGHT: {
            addr = US_RIGHT;
            break;
        }
        default: break;
    }
    cli();
    TwoWire_beginTransmission(&twi0, addr);
    TwoWire_write(&twi0, auv.sonarMode); //The value is the mode of operation
    TwoWire_endTransmission(&twi0, 1);
    _delay_ms(250); //Wait before responding
    
    //get the results
    u16 dist = 0;
    TwoWire_requestFrom(&twi0, addr, 2, 1);
    if (TwoWire_available(&twi0) == 2) {
        u8 dataLow = TwoWire_read(&twi0);  
        u8 dataHigh = TwoWire_read(&twi0);
        dist = dataLow;
        dist |= (dataHigh << 8);
    }
    sei();
    
    //Becuase the sensor is mounted on the bottom, we can't actively test this now its
    //assembled. It was tested as working previously but now we need to override the 
    //returned value. 
    if ((addr == US_BOTTOM) && (auv.sonarMode == LAND)) {
        return 0xFFFF; //Return max. 
    }
    return dist;
}

void handleDistanceResponse(SonarDirection dir) {
    switch (dir) {
        case SD_LOWER: {
            Sonar* s = AUV_loadSonar(&auv, LOWER);
            if (s->distance > 0 && s->distance < 1400) {
                depth = getDepth();
                if (depth == 0) {
                //Handle later
                } else {
                    /**
                     * I think this is having the same ghandi bug from the civ games. 
                     * if depth is less than 10 and we take off 10 it does to 255 - depth - 10
                     * 
                     * If its less than 10 we just need to raise to 0. 
                     */
                    if (depth > 10) {
                        depth -= 10;
                        raise(depth);
                    } else if(depth == 1) {
                        raise(1); //not doing anything in reality. 
                    } else {
                        raise(0); //raise to 0 and it should 1 itself. 
                    }                    
                }
                AUV_setMotorDirection(&auv, FMD_STOP, SD_LOWER);
            } else if (s->distance > 0 && s->distance > 3000) {
//                if (depth < 245) {
//                    depth += 10;
//                    dive(depth);
//                } else if (depth == 254) {
//                    dive(254);
//                } else {
//                    dive(255);
//                }
                if (auv.mainMotorHeldBy == SD_LOWER || auv.mainMotorHeldBy == SD_NONE) {
                    AUV_setMotorDirection(&auv, FMD_FORWARD, SD_NONE);
                }
            }
        }

        case SD_FORWARD: {
            Sonar* s = AUV_loadSonar(&auv, FORWARD);
            if (s->distance > 0 && s->distance < 1400) {
                if(s->distance < 800) {
                    AUV_setMotorDirection(&auv,FMD_BACKWARD, SD_FORWARD);
                } else {
                    AUV_setMotorDirection(&auv, FMD_STOP, SD_FORWARD);
                }
            } else if(s->distance > 0 && s->distance > 1400) {
                if (auv.mainMotorHeldBy == SD_FORWARD || auv.mainMotorHeldBy == SD_NONE) {
                    AUV_setMotorDirection(&auv, FMD_FORWARD, SD_NONE);
                }
            }
        }
        
        case SD_LEFT: {
            Sonar* s = AUV_loadSonar(&auv, LEFT);
            if (s->distance < 1400) {
                AUV_setMotorDirection(&auv,FMD_STOP, SD_LEFT);
                AUV_setTurnDirection(&auv, SMD_RIGHT, SD_LEFT);
            } else {
                if (auv.mainMotorHeldBy == SD_LEFT || auv.mainMotorHeldBy == SD_NONE) {
                    AUV_setMotorDirection(&auv, FMD_FORWARD, SD_NONE);
                }
                if (auv.sideMotorHeldBy == SD_LEFT || auv.sideMotorHeldBy == SD_NONE) {
                    AUV_setTurnDirection(&auv, SMD_NONE, SD_NONE);
                }
            }
        }
        
        case SD_RIGHT: {
            Sonar* s = AUV_loadSonar(&auv, LEFT);
            if (s->distance < 800) {
                AUV_setMotorDirection(&auv,FMD_STOP, SD_RIGHT);
                AUV_setTurnDirection(&auv, SMD_LEFT, SD_RIGHT);
            } else {
                if (auv.mainMotorHeldBy == SD_RIGHT || auv.mainMotorHeldBy == SD_NONE) {
                    AUV_setMotorDirection(&auv, FMD_FORWARD, SD_NONE);
                }
                if (auv.sideMotorHeldBy == SD_RIGHT || auv.sideMotorHeldBy == SD_NONE) {
                    AUV_setTurnDirection(&auv, SMD_NONE, SD_NONE);
                }
            }
        }
        
        default: { 
            break;
        }
    }
}

u8 getDepth(void) {
    RGB(BLUE);
    _delay_ms(100); //So we can see the xmission
    u8 data = 0;
    TwoWire_requestFrom(&twi0, DEPTH_CONTROLLER, 1, 1);
    if (TwoWire_available(&twi0)) {
        data = TwoWire_read(&twi0);  // Read the received byte
    }
    return data;
}

void dive(u8 d) {
    RGB(BLUE);
    TwoWire_beginTransmission(&twi0, DEPTH_CONTROLLER);
    TwoWire_write(&twi0, d);
    TwoWire_endTransmission(&twi0, 1);
    RGB(GREEN);
}
void raise(u8 d) {
    dive(d);
}

void commandForwardMotor(ForwardMotorDirection dir) {
    RGB(BLUE);
    TwoWire_beginTransmission(&twi0, FORWARD_MOTOR);
    TwoWire_write(&twi0, (u8)dir);
    TwoWire_endTransmission(&twi0, 1);
    RGB(GREEN);
}

void commandSideMotors(SideMotorDirection dir) {
    RGB(BLUE);
    if (dir == SMD_LEFT) {
        TwoWire_beginTransmission(&twi0, LEFT_MOTOR);
        TwoWire_write(&twi0, (u8)SMS_OFF);
        TwoWire_endTransmission(&twi0, 1);
        _delay_ms(100);
        TwoWire_beginTransmission(&twi0, RIGHT_MOTOR);
        TwoWire_write(&twi0, (u8)SMS_ON);
        TwoWire_endTransmission(&twi0, 1);
    } else if (dir == SMD_RIGHT) {
        TwoWire_beginTransmission(&twi0, RIGHT_MOTOR);
        TwoWire_write(&twi0, (u8)SMS_OFF);
        TwoWire_endTransmission(&twi0, 1);
        _delay_ms(100);
        TwoWire_beginTransmission(&twi0, LEFT_MOTOR);
        TwoWire_write(&twi0, (u8)SMS_ON);
        TwoWire_endTransmission(&twi0, 1);
    } else if (dir == SMD_NONE) {
        TwoWire_beginTransmission(&twi0, RIGHT_MOTOR);
        TwoWire_write(&twi0, (u8)SMS_OFF);
        TwoWire_endTransmission(&twi0, 1);
        _delay_ms(100);
        TwoWire_beginTransmission(&twi0, LEFT_MOTOR);
        TwoWire_write(&twi0, (u8)SMS_OFF);
        TwoWire_endTransmission(&twi0, 1);
    }
    RGB(GREEN);
}

ISR(RTC_CNT_vect) {
    RGB(BLUE);
    switch(auv.sonarIndex) {
        case LOWER: {
            auv.sonars[LOWER].distance = ping(LOWER);
            RGB(GREEN);
            handleDistanceResponse(SD_LOWER);
            break;
        }
        case FORWARD: {
            auv.sonars[FORWARD].distance = ping(FORWARD);
            RGB(GREEN);
            handleDistanceResponse(SD_FORWARD);
            break;
        }
        case LEFT: {
            auv.sonars[LEFT].distance = ping(LEFT);
            RGB(GREEN);
            handleDistanceResponse(SD_LEFT);
            break;
        }
        case RIGHT: {
            auv.sonars[RIGHT].distance = ping(RIGHT);
            RGB(GREEN);
            handleDistanceResponse(SD_RIGHT);
            break;
        }
        default: break;
    }
    AUV_incrementSonarIndex(&auv);
    commandForwardMotor(auv.mainMotorDirection);
    commandSideMotors(auv.sideMotorDirection);
    
    if ((depth == 0) || (depth == 1)) {
        AUV_increaseSurfaceTimer(&auv);
        if (AUV_shouldDive(&auv) == 1) {
            dive(255);
        }
    } else {
        AUV_increaseSubmergeTimer(&auv);
        if (AUV_shouldSurface(&auv)) {
            raise(0);
        }
    }
    
    
    RTC.INTFLAGS = RTC_OVF_bm;
}