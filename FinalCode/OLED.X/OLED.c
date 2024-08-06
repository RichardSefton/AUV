#include "OLED.h"
#include <util/delay.h>

void OLED_Init(void) {
    _delay_ms(100); // Wait for the OLED to power up
    OLED_Command(0xAE); // Display off
    OLED_Command(0xD5); // Set display clock divide ratio/oscillator frequency
    OLED_Command(0x80); // Default value
    OLED_Command(0xA8); // Set multiplex ratio
    OLED_Command(0x3F); // 1/64 duty
    OLED_Command(0xD3); // Set display offset
    OLED_Command(0x00); // No offset
    OLED_Command(0x40); // Set start line to 0
    OLED_Command(0x8D); // Charge pump setting
    OLED_Command(0x14); // Enable charge pump
    OLED_Command(0x20); // Memory addressing mode
    OLED_Command(0x00); // Horizontal addressing mode
    OLED_Command(0xA1); // Set segment re-map
    OLED_Command(0xC8); // COM output scan direction
    OLED_Command(0xDA); // Set COM pins hardware configuration
    OLED_Command(0x12); // Alternative COM pin configuration, disable COM left/right remap
    OLED_Command(0x81); // Set contrast control
    OLED_Command(0xCF); // Set to 127
    OLED_Command(0xD9); // Set pre-charge period
    OLED_Command(0xF1); // Phase 1 period: 15 DCLKs, Phase 2 period: 1 DCLK
    OLED_Command(0xDB); // Set VCOMH deselect level
    OLED_Command(0x40); // ~0.77 x Vcc
    OLED_Command(0xA4); // Entire display ON, resume to RAM content display
    OLED_Command(0xA6); // Set normal display (not inverted)
    OLED_Command(0xAF); // Display ON
}

void OLED_Command(uint8_t cmd) {
    TWI_Master_Start(OLED_ADDR, 0);
    TWI_Master_Write(COMMAND);
    TWI_Master_Write(cmd);
    TWI_Master_Stop();
}

void OLED_Data(uint8_t data) {
    TWI_Master_Start(OLED_ADDR, 0);
    TWI_Master_Write(DATA);
    TWI_Master_Write(data);
    TWI_Master_Stop();
}

void OLED_Clear(void) {
    //Clears the pages. 8 pages of 8 rows
    for (uint8_t i = 0; i < 8; i++) {
        OLED_SetCursor(0, i);
        //128 columns per row
        for (uint8_t j = 0; j < 128; j++) {
            OLED_Data(0x00);
        }
    }
}

void OLED_SetCursor(uint8_t x, uint8_t y) {
    OLED_Command(0x21); // Set column address
    OLED_Command(x);
    OLED_Command(127);
    OLED_Command(0x22); // Set page address
    OLED_Command(y);
    OLED_Command(7);
}

void OLED_DrawPixel(uint8_t x, uint8_t y) {
    OLED_SetCursor(x, y / 8);
    OLED_Data(1 << (y % 8));
}

void OLED_PrintChar(char c) {
    for (uint8_t i = 0; i < 5; i++) {
        OLED_Data(font[c - 32][i]);
    }
    OLED_Data(0x00);
}

void OLED_PrintString(const char* str) {
    while (*str) {
        OLED_PrintChar(*str++);
    }
}

void OLED_PrintFloat(float f) {
    char conv[20];
    sprintf(conv, "%.2f", f);
    OLED_PrintString(conv);
}
