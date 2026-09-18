#include "i2c.h"
#include <stdbool.h>
#include "stm32f1xx_hal.h"

static I2C_HandleTypeDef hi2c1;

void I2C1_Init(void){
    // clocks
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    // PB6 PB7
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //I2C
    hi2c1.Instance = I2C1;

    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c1);
}

uint8_t I2C1_ReadRegister(uint8_t deviceAddress, uint8_t reg){
    uint8_t data = 0;

    HAL_I2C_Mem_Read(
        &hi2c1,
        deviceAddress << 1,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        HAL_MAX_DELAY);
    
    return data;
}

bool I2C1_ReadRegisters(uint8_t deviceAddress, uint8_t startReg, uint8_t *data, uint16_t length){
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(
        &hi2c1,
        deviceAddress << 1,
        startReg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        length,
        HAL_MAX_DELAY
    );

    if (status != HAL_OK)
    {
        return false;
    }

    return true;
}

void I2C1_WriteRegister(uint8_t deviceAddress, uint8_t reg, uint8_t data){
    HAL_I2C_Mem_Write(
        &hi2c1,
        deviceAddress << 1,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        HAL_MAX_DELAY
    );
}