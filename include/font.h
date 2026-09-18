#ifndef FONT_H
#define FONT_H

#include <stdint.h>
#include <stdbool.h>

bool Font_DrawDigit(uint16_t x, uint16_t y, uint8_t digit, uint8_t scale, uint16_t textColor, uint16_t backgroundColor);
bool Font_DrawNumber(uint16_t x, uint16_t y, uint32_t number, uint8_t scale, uint16_t textColor, uint16_t backgroundColor);
bool Font_DrawChar(uint16_t x, uint16_t y, char character, uint8_t scale, uint16_t textColor, uint16_t backgroundColor);
bool Font_DrawString(uint16_t x, uint16_t y, const char *text, uint8_t scale, uint16_t textColor, uint16_t backgroundColor);

#endif