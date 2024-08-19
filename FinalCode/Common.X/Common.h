/* 
 * File:   Common.h
 * Author: richa
 *
 * Created on 18 August 2024, 21:50
 */

#ifndef COMMON_H
#define	COMMON_H

#include <avr/io.h>
#include "ShortTypes.h"

extern enum {
    RED = PIN6_bm,
    GREEN = PIN5_bm,
    BLUE = PIN4_bm
} Colours;

void setupRGB(void);
void RGB(u8);


#endif	/* COMMON_H */

