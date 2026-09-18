#include "spi.h"
#include "stm32f1xx_hal.h"

static SPI_HandleTypeDef hspi1;

void SPI1_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    // PA5 = SCK | PA7 = MOSI
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA6 = MISO
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hspi1.Instance = SPI1;

    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;

    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;

    hspi1.Init.NSS = SPI_NSS_SOFT;

    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;

    HAL_SPI_Init(&hspi1);
}

bool SPI1_Transmit(const uint8_t *data, uint16_t size){
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, (uint8_t *)data, size, HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }
    
    return true;
}

bool SPI1_Receive(uint8_t *data, uint16_t size){
    HAL_StatusTypeDef status = HAL_SPI_Receive(&hspi1, data, size, HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    return true;
}