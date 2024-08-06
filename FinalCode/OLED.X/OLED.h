#ifndef OLED_H
#define OLED_H

#include <avr/io.h>
#include <stdio.h>
#include "TWI.h"
#include "font.h"

#define OLED_ADDR 0x3C
#define COMMAND 0x00
#define DATA 0x40

void OLED_Init(void);
void OLED_Command(uint8_t);
void OLED_Data(uint8_t);
void OLED_Clear(void);
void OLED_SetCursor(uint8_t, uint8_t);
void OLED_DrawPixel(uint8_t, uint8_t);
void OLED_PrintChar(char);
void OLED_PrintString(const char*);
void OLED_PrintFloat(float);

#endif
