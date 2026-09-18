#include "touch.h"
#include "spi.h"

#define TOUCH_CS_PIN GPIO_PIN_11
#define TOUCH_CS_PORT GPIOB

#define TOUCH_CMD_Y 0xD0 // Native X
#define TOUCH_CMD_X 0x90 // Native Y

#define TOUCH_X_MIN 350
#define TOUCH_X_MAX 3850

#define TOUCH_Y_MIN 250
#define TOUCH_Y_MAX 3800

static void Touch_Select(void){
    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_RESET);
}

static void Touch_Unselect(void){
    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_SET);
}

void Touch_Init(void){
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = TOUCH_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(TOUCH_CS_PORT, &GPIO_InitStruct);

    Touch_Unselect();
}

bool Touch_ReadRawX(uint16_t *rawX){
    if(rawX == NULL)
    {
        return false;
    }

    uint8_t command = TOUCH_CMD_X;
    uint8_t rx[2];

    Touch_Select();

    if(!SPI1_Transmit(&command, 1))
    {
        Touch_Unselect();
        return false;
    }

    if(!SPI1_Receive(rx, 2))
    {
        Touch_Unselect();
        return false;
    }

    uint16_t data = ((uint16_t)rx[0] << 8) | rx[1];

    *rawX = data >> 3;

    Touch_Unselect();

    return true;
}

bool Touch_ReadRawY(uint16_t *rawY){
    if(rawY == NULL)
    {
        return false;
    }

    uint8_t command = TOUCH_CMD_Y;
    uint8_t rx[2];

    Touch_Select();

    if(!SPI1_Transmit(&command, 1))
    {
        Touch_Unselect();
        return false;
    }

    if(!SPI1_Receive(rx, 2))
    {
        Touch_Unselect();
        return false;
    }

    uint16_t data = ((uint16_t)rx[0] << 8) | rx[1];

    *rawY = data >> 3;

    Touch_Unselect();

    return true;
}

bool Touch_Read(uint16_t *x, uint16_t *y){
    if(x == NULL || y == NULL)
    {
        return false;
    }

    uint16_t rawX;
    uint16_t rawY;

    if(!Touch_ReadRawX(&rawX) || !Touch_ReadRawY(&rawY))
    {
        return false;
    }
    
    if(rawX < TOUCH_X_MIN || rawX > TOUCH_X_MAX || rawY < TOUCH_Y_MIN || rawY > TOUCH_Y_MAX)
    {
        return false;
    }

    *x = ((uint32_t)(rawX - TOUCH_X_MIN) * 319) / (TOUCH_X_MAX - TOUCH_X_MIN);
    *y = ((uint32_t)(rawY - TOUCH_Y_MIN) * 239) / (TOUCH_Y_MAX - TOUCH_Y_MIN);

    return true;
}