#ifndef ILI_9341_H
#define ILI_9341_H

#define ILI9341_BLACK   0x0000
#define ILI9341_WHITE   0xFFFF
#define ILI9341_RED     0xF800
#define ILI9341_GREEN   0x07E0
#define ILI9341_BLUE    0x001F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_DARKBLUE 0x0010

#include <stdbool.h>
#include <stdint.h>

void ILI9341_Init(void);
bool ILI9341_FillScreenOneColor(uint16_t color);
bool ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image);
bool ILI9341_DrawImageScaled_240x240(uint16_t x, uint16_t y, uint16_t srcWidth, uint16_t srcHeight,
                             uint16_t destWidth, uint16_t destHeight, const uint8_t *image);
bool ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

#endif