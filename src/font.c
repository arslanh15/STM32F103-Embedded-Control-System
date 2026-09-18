#include "font.h"
#include "ili9341.h"

#define FONT_MAX_SCALE 6

static uint8_t digitBuffer[5 * FONT_MAX_SCALE * 7 * FONT_MAX_SCALE * 2];

static const uint8_t digits[10][7] =
{
    // 0
    {
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110
    },
    // 1
    {
        0b00100,
        0b01100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b01110
    },
    // 2
    {
        0b01110,
        0b10001,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b11111
    },
    // 3
    {
        0b11110,
        0b00001,
        0b00001,
        0b01110,
        0b00001,
        0b00001,
        0b11110
    },
    // 4
    {
        0b00010,
        0b00110,
        0b01010,
        0b10010,
        0b11111,
        0b00010,
        0b00010
    },
    // 5
    {
        0b11111,
        0b10000,
        0b10000,
        0b11110,
        0b00001,
        0b00001,
        0b11110
    },
    // 6
    {
        0b01110,
        0b10000,
        0b10000,
        0b11110,
        0b10001,
        0b10001,
        0b01110
    },
    // 7
    {
        0b11111,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b01000,
        0b01000
    },
    // 8
    {
        0b01110,
        0b10001,
        0b10001,
        0b01110,
        0b10001,
        0b10001,
        0b01110
    },
    // 9
    {
        0b01110,
        0b10001,
        0b10001,
        0b01111,
        0b00001,
        0b00001,
        0b01110
    }
};

static const uint8_t letters[26][7] =
{
    // A
    {
        0b01110,
        0b10001,
        0b10001,
        0b11111,
        0b10001,
        0b10001,
        0b10001
    },

    // B
    {
        0b11110,
        0b10001,
        0b10001,
        0b11110,
        0b10001,
        0b10001,
        0b11110
    },

    // C
    {
        0b01111,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b01111
    },

    // D
    {
        0b11110,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b11110
    },

    // E
    {
        0b11111,
        0b10000,
        0b10000,
        0b11110,
        0b10000,
        0b10000,
        0b11111
    },

    // F
    {
        0b11111,
        0b10000,
        0b10000,
        0b11110,
        0b10000,
        0b10000,
        0b10000
    },

    // G
    {
        0b01110,
        0b10001,
        0b10000,
        0b10111,
        0b10001,
        0b10001,
        0b01110
    },

    // H
    {
        0b10001,
        0b10001,
        0b10001,
        0b11111,
        0b10001,
        0b10001,
        0b10001
    },

    // I
    {
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b11111
    },

    // J
    {
        0b00111,
        0b00010,
        0b00010,
        0b00010,
        0b00010,
        0b10010,
        0b01100
    },

    // K
    {
        0b10001,
        0b10010,
        0b10100,
        0b11000,
        0b10100,
        0b10010,
        0b10001
    },

    // L
    {
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b10000,
        0b11111
    },

    // M
    {
        0b10001,
        0b11011,
        0b10101,
        0b10101,
        0b10001,
        0b10001,
        0b10001
    },

    // N
    {
        0b10001,
        0b11001,
        0b11001,
        0b10101,
        0b10011,
        0b10011,
        0b10001
    },

    // O
    {
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110
    },

    // P
    {
        0b11110,
        0b10001,
        0b10001,
        0b11110,
        0b10000,
        0b10000,
        0b10000
    },

    // Q
    {
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b10101,
        0b10010,
        0b01101
    },

    // R
    {
        0b11110,
        0b10001,
        0b10001,
        0b11110,
        0b10100,
        0b10010,
        0b10001
    },

    // S
    {
        0b01111,
        0b10000,
        0b10000,
        0b01110,
        0b00001,
        0b00001,
        0b11110
    },

    // T
    {
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    },

    // U
    {
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110
    },

    // V
    {
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01010,
        0b00100
    },

    // W
    {
        0b10001,
        0b10001,
        0b10001,
        0b10101,
        0b10101,
        0b11011,
        0b10001
    },

    // X
    {
        0b10001,
        0b10001,
        0b01010,
        0b00100,
        0b01010,
        0b10001,
        0b10001
    },

    // Y
    {
        0b10001,
        0b10001,
        0b01010,
        0b00100,
        0b00100,
        0b00100,
        0b00100
    },

    // Z
    {
        0b11111,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b10000,
        0b11111
    }
};

static const uint8_t charColon[7] =
{
    0b00000,
    0b00100,
    0b00100,
    0b00000,
    0b00100,
    0b00100,
    0b00000
};

static const uint8_t charLeft[7] =
{
    0b00010,
    0b00100,
    0b01000,
    0b10000,
    0b01000,
    0b00100,
    0b00010
};

static const uint8_t charRight[7] =
{
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b00010,
    0b00100,
    0b01000
};

static bool Font_DrawBitmap(uint16_t x, uint16_t y, const uint8_t bitmap[7], uint8_t scale, uint16_t textColor, uint16_t backgroundColor){
    if(scale==0 || scale > FONT_MAX_SCALE)
    {
        return false;
    }

    uint16_t width = 5 * scale;
    uint16_t height = 7 * scale;

    for(uint16_t destY = 0; destY < height; destY++)
    {
        for(uint16_t destX = 0; destX < width; destX++)
        {
            uint8_t fontCol = destX / scale;
            uint8_t fontRow = destY / scale;

            uint8_t mask = 1U << (4 - fontCol);

            bool pixelOn = bitmap[fontRow] & mask;

            uint16_t color;
            if(pixelOn)
            {
                color = textColor;
            }
            else
            {
                color = backgroundColor;
            }

            uint32_t index =((uint32_t)destY * width + destX) * 2U;
            digitBuffer[index] = color >> 8;
            digitBuffer[index+1] = color & 0xFF;
        }
    }
    return ILI9341_DrawImage(x, y, width, height, digitBuffer);
}

bool Font_DrawDigit(uint16_t x, uint16_t y, uint8_t digit, uint8_t scale, uint16_t textColor, uint16_t backgroundColor){
    if(digit > 9)
    {
        return false;
    }

    return Font_DrawBitmap(x, y, digits[digit], scale, textColor, backgroundColor);
}

bool Font_DrawNumber(uint16_t x, uint16_t y, uint32_t number, uint8_t scale, uint16_t textColor, uint16_t backgroundColor){
    if(scale == 0)
    {
        return false;
    }
    
    if(number == 0)
    {
        return Font_DrawDigit(
            x,
            y,
            0,
            scale,
            textColor,
            backgroundColor
        );
    }
    
    uint8_t digitsBuffer[10];
    uint8_t digitCount = 0;

    while(number > 0)
    {
        digitsBuffer[digitCount] = number % 10;
        digitCount++;

        number = number / 10;
    }

    for(int8_t i = digitCount - 1; i >= 0; i--)
    {
        if(!Font_DrawDigit(x, y, digitsBuffer[i], scale, textColor, backgroundColor))
        {
            return false;
        };
        x = x + 6 * scale;
    }

    return true;
}

bool Font_DrawChar(uint16_t x, uint16_t y, char character, uint8_t scale, uint16_t textColor, uint16_t backgroundColor){
    if(character >= '0' && character <= '9')
    {
        return Font_DrawBitmap(x, y, digits[character-'0'], scale, textColor, backgroundColor);
    }

    if(character >= 'A' && character <= 'Z')
    {
        return Font_DrawBitmap(x, y, letters[character-'A'], scale, textColor, backgroundColor);
    }

    if(character == ':')
    {
        return Font_DrawBitmap(x, y, charColon, scale, textColor, backgroundColor);
    }

    if(character == '<')
    {
        return Font_DrawBitmap(x, y, charLeft, scale, textColor, backgroundColor);
    }

    if(character == '>')
    {
        return Font_DrawBitmap(x, y, charRight, scale, textColor, backgroundColor);
    }

    return false;
}

bool Font_DrawString(uint16_t x, uint16_t y, const char *text, uint8_t scale, uint16_t textColor, uint16_t backgroundColor){
    while(*text != '\0')
    {
        Font_DrawChar(x, y, *text, scale, textColor, backgroundColor);

        x = x + 6 * scale;

        text++;
    }
}