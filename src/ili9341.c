#include "ili9341.h"
#include "spi.h"
#include "stm32f1xx_hal.h"

#define LCD_CS_PIN GPIO_PIN_0
#define LCD_RST_PIN GPIO_PIN_1
#define LCD_DC_PIN GPIO_PIN_10

#define ILI9341_SWRESET   0x01   // Software Reset
#define ILI9341_SLPOUT    0x11   // Sleep Out
#define ILI9341_DISPON    0x29   // Display On
#define ILI9341_MADCTL    0x36   // Memort Data Access Control
#define ILI9341_PIXFMT    0x3A   // Pixel Format
#define ILI9341_CASET     0x2A   // Column Address Set
#define ILI9341_PASET     0x2B   // Page Address Set
#define ILI9341_RAMWR     0x2C   // Memory Write

#define windowX0 0
#define windowX1 319
#define windowY0 0
#define windowY1 239

static void ILI9341_GPIO_Init(void){
    // Enable clock
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LCD CS
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = LCD_CS_PIN | LCD_RST_PIN | LCD_DC_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, LCD_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LCD_DC_PIN, GPIO_PIN_SET);
}

static void LCD_Select(void){
    HAL_GPIO_WritePin(GPIOB, LCD_CS_PIN, GPIO_PIN_RESET);
}

static void LCD_Unselect(void){
    HAL_GPIO_WritePin(GPIOB, LCD_CS_PIN, GPIO_PIN_SET);
}

static void LCD_Reset(void){
    HAL_GPIO_WritePin(GPIOB, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(10);

    HAL_GPIO_WritePin(GPIOB, LCD_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);

    HAL_GPIO_WritePin(GPIOB, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(50);
}

static bool LCD_WriteCommand(uint8_t command){
    HAL_GPIO_WritePin(GPIOB, LCD_DC_PIN, GPIO_PIN_RESET);
    LCD_Select();
    bool status = SPI1_Transmit(&command, 1);
    LCD_Unselect();
    
    return status;
}

static bool LCD_WriteData(const uint8_t *data, uint16_t size){
    HAL_GPIO_WritePin(GPIOB, LCD_DC_PIN, GPIO_PIN_SET);
    LCD_Select();
    bool status = SPI1_Transmit(data, size);
    LCD_Unselect();

    return status;
}

void ILI9341_Init(void){
    ILI9341_GPIO_Init();
    LCD_Reset();
    // Software Reset
    LCD_WriteCommand(ILI9341_SWRESET);
    HAL_Delay(150);
    // Sleep Out
    LCD_WriteCommand(ILI9341_SLPOUT);
    HAL_Delay(120);
    // Pixel Format
    LCD_WriteCommand(ILI9341_PIXFMT);
    uint8_t pixelFormat = 0x55;
    LCD_WriteData(&pixelFormat, 1);
    // Memory Access Control
    LCD_WriteCommand(ILI9341_MADCTL);
    uint8_t madctl = 0xE8;
    LCD_WriteData(&madctl, 1);
    // Display ON
    LCD_WriteCommand(ILI9341_DISPON);
    HAL_Delay(20);
}

static bool LCD_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    uint8_t data[4];

    // X coordinates
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;

    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;

    if(!LCD_WriteCommand(ILI9341_CASET))
    {
        return false;
    }
    if(!LCD_WriteData(data, 4))
    {
        return false;
    }

    // Y coordinates
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;

    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;

    if(!LCD_WriteCommand(ILI9341_PASET))
    {
        return false;
    }
    if(!LCD_WriteData(data, 4))
    {
        return false;
    }

    // Pixel
    if(!LCD_WriteCommand(ILI9341_RAMWR))
    {
        return false;
    }

    return true;
}

bool ILI9341_FillScreenOneColor(uint16_t color){
    uint8_t buffer[128];

    uint8_t colorHigh = color >> 8;
    uint8_t colorLow = color & 0xFF;

    for(uint8_t i = 0; i < 64; i++)
    {
        buffer[2*i] = colorHigh;
        buffer[2*i+1] = colorLow;
    }

    if(!LCD_SetAddressWindow(windowX0, windowY0, windowX1, windowY1))
    {
        return false;
    }

    for(uint16_t i = 0; i < 1200; i++)
    {
        if(!LCD_WriteData(buffer, 128))
        {
            return false;
        }
    }
    return true;
}

bool ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image){
    uint16_t x1 = x + width - 1;
    uint16_t y1 = y + height - 1;

    uint16_t imageSize = width * height * 2;

    if(!LCD_SetAddressWindow(x, y, x1, y1))
    {
        return false;
    }

    if(!LCD_WriteData(image, imageSize))
    {
        return false;
    }

    return true;
}

bool ILI9341_DrawImageScaled_240x240(uint16_t x, uint16_t y, uint16_t srcWidth, uint16_t srcHeight, uint16_t destWidth, uint16_t destHeight, const uint8_t *image){
    if(destWidth > 240)
    {
        return false;
    }

    if (srcWidth == 0 || srcHeight == 0 || destWidth == 0 || destHeight == 0)
    {
        return false;
    }

    if ((uint32_t)x + destWidth > 320 || (uint32_t)y + destHeight > 240)
    {
        return false;
    }
    
    uint16_t x1 = x + destWidth -1;
    uint16_t y1 = y + destHeight -1;

    if(!LCD_SetAddressWindow(x, y, x1, y1))
    {
        return false;
    }

    uint8_t rowBuffer[240*2];

    for (uint16_t destY = 0; destY < destHeight; destY++)
    {
        uint16_t srcY =
            ((uint32_t)destY * srcHeight) / destHeight;

        for (uint16_t destX = 0; destX < destWidth; destX++)
        {
            uint16_t srcX =
                ((uint32_t)destX * srcWidth) / destWidth;

            uint32_t sourceIndex =
                ((uint32_t)srcY * srcWidth + srcX) * 2U;

            rowBuffer[2 * destX] =
                image[sourceIndex];

            rowBuffer[2 * destX + 1] =
                image[sourceIndex + 1];
        }

        if(!LCD_WriteData(rowBuffer, destWidth*2))
        {
            return false;
        }
    }

    return true;
}

bool ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color){
    if(width == 0 || height == 0)
    {
        return false;
    }

    if((uint32_t)x + width > 320 || (uint32_t)y + height > 240)
    {
        return false;
    }

    uint16_t x1 = x + width - 1;
    uint16_t y1 = y + height - 1;

    uint32_t pixelCount = (uint32_t)width * (uint32_t)height;

    uint8_t buffer[128];

    uint8_t colorHigh = color >> 8;
    uint8_t colorLow = color & 0xFF;
    
    for(uint8_t i = 0; i < 64; i++)
    {
        buffer[2*i] = colorHigh;
        buffer[2*i+1] = colorLow;
    }

    uint32_t fullBlocks = pixelCount / 64;
    uint32_t remainingPixels = pixelCount % 64;

    if(!LCD_SetAddressWindow(x, y, x1, y1))
    {
        return false;
    }

    for(uint32_t i = 0; i<fullBlocks; i++)
    {
        if(!LCD_WriteData(buffer, 128))
        {
            return false;
        }
    }

    if(remainingPixels>0)
    {
        if(!LCD_WriteData(buffer, remainingPixels*2))
        {
            return false;
        };
    }

    return true;
}