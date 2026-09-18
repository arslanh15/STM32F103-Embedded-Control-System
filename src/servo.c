#include "servo.h"
#include "stm32f1xx_hal.h"

static TIM_HandleTypeDef htim2;

void Servo_Init(void){
    // clocklar
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM_OC_InitTypeDef sConfigOC = {0};

    // PA1 alternate function push-pull
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    htim2.Instance = TIM2;

    htim2.Init.Prescaler = 7; // Timer clock 8 Mhz -> 8 / (7+1) = 1 Mhz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 19999; // 20000 us = 20 ms = 50 Hz
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    HAL_TIM_PWM_Init(&htim2);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(
        &htim2,
        &sConfigOC,
        TIM_CHANNEL_2
    );

    HAL_TIM_PWM_ConfigChannel(
        &htim2,
        &sConfigOC,
        TIM_CHANNEL_3
    );
}

void Servo_Start(void){
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

void Servo_SetPulse(uint32_t pulse){
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
}

void Servo2_SetPulse(uint32_t pulse){
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
}