#include "adc.h"
#include "stm32f1xx_hal.h"

static ADC_HandleTypeDef hadc1;

void ADC1_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_ChannelConfTypeDef ADC_ChannelConf = {0};

    // GPIOA ve ADC1 clockları
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Analog input PA0
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // ADC clock prescaler
    __HAL_RCC_ADC_CONFIG(RCC_ADCPCLK2_DIV6);
    __HAL_RCC_ADC1_CLK_ENABLE();

    // ADC1 ayarları
    hadc1.Instance = ADC1;

    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;

    HAL_ADC_Init(&hadc1);

    // ADC1 Channel 0 -> PA0
    ADC_ChannelConf.Channel = ADC_CHANNEL_0;
    ADC_ChannelConf.Rank = ADC_REGULAR_RANK_1;
    ADC_ChannelConf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    HAL_ADC_ConfigChannel(&hadc1, &ADC_ChannelConf);

    // ADC kalibrasyonu
    HAL_ADCEx_Calibration_Start(&hadc1);
}

uint32_t ADC1_Read(void){
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    
    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    return adcValue;
}