#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "uart.h"
#include "servo.h"
#include "i2c.h"
#include "mpu6500.h"
#include "orientation.h"
#include "spi.h"
#include "ili9341.h"
#include "font.h"
#include "touch.h"

#define ADC_MAX 4095U

#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500

#define SCREEN_MODE true
#define PHYSICAL_MODE false

void SysTick_Handler(void);

uint32_t adcValue = 0;
char buffer[128];

uint32_t lastPulse = 1500;
uint32_t lastPulse2 = 1500;

MPU6500_Data imuData;

uint16_t angle1;
uint16_t angle2;

bool motorMode;

uint16_t touchX; // Touch Screen X
uint16_t touchY; // Touch Screen Y

uint32_t screenPulse1 = 1500;
uint32_t screenPulse2 = 1500;

uint8_t buttonPressedCount = 0;

int16_t lastDisplayedAngle1 = -1;
int16_t lastDisplayedAngle2 = -1;

int main(void){
   HAL_Init();

   ADC1_Init();
   UART1_Init();
   Servo_Init();
   Servo_Start();
   I2C1_Init();
   SPI1_Init();
   ILI9341_Init();
   Touch_Init();

   if (!MPU6500_Init())
   {
    while(1)
    {
        snprintf(
            buffer,
            sizeof(buffer),
            "%lu\n",
            (unsigned long)31
        );
        UART1_Send(buffer);
        HAL_Delay(500);
    }
   }

   if (!MPU6500_CalibrateAccelGyro())
   {
    while(1)
    {
        snprintf(
            buffer,
            sizeof(buffer),
            "%lu\n",
            (unsigned long)62
        );
        UART1_Send(buffer);
        HAL_Delay(500); 
    }   
   }

   if (!MPU6500_ReadAccelGyro(&imuData))
   {
    while(1)
    {
        snprintf(
            buffer,
            sizeof(buffer),
            "%lu\n",
            (unsigned long)69
        );
        UART1_Send(buffer);
        HAL_Delay(500);
    }
   }

   Orientation_Init(imuData.ax, imuData. az);
   uint32_t lastADCTick = HAL_GetTick();
   uint32_t lastLCDTick = HAL_GetTick();

   motorMode = PHYSICAL_MODE;

   ILI9341_FillScreenOneColor(ILI9341_YELLOW);
   Font_DrawString(10, 60, "MOTOR1:", 2, ILI9341_DARKBLUE, ILI9341_YELLOW);
   Font_DrawString(10, 160, "MOTOR2:", 2, ILI9341_DARKBLUE, ILI9341_YELLOW);
   ILI9341_FillRect(5, 105, 55, 25, ILI9341_DARKBLUE);
   Font_DrawString(10, 110, "MODE", 2, ILI9341_YELLOW, ILI9341_DARKBLUE);
   Font_DrawChar(60, 110, ':', 2, ILI9341_DARKBLUE, ILI9341_YELLOW);
   Font_DrawString(70, 110, "PHY", 2, ILI9341_DARKBLUE, ILI9341_YELLOW);

   // Motor 1 - sol kutu
   ILI9341_FillRect(175, 45, 55, 40, ILI9341_DARKBLUE);
   // Motor 1 - sağ kutu
   ILI9341_FillRect(250, 45, 55, 40, ILI9341_DARKBLUE);
   // Motor 1 - sol ok
   Font_DrawChar(195, 54, '<', 3, ILI9341_YELLOW, ILI9341_DARKBLUE);
   // Motor 1 - sağ ok
   Font_DrawChar(270, 54, '>', 3, ILI9341_YELLOW, ILI9341_DARKBLUE);


   // Motor 2 - sol kutu
   ILI9341_FillRect(175, 145, 55, 40, ILI9341_DARKBLUE);
   // Motor 2 - sağ kutu
   ILI9341_FillRect(250, 145, 55, 40, ILI9341_DARKBLUE);
   // Motor 2 - sol ok
   Font_DrawChar(195, 154, '<', 3, ILI9341_YELLOW, ILI9341_DARKBLUE);
   // Motor 2 - sağ ok
   Font_DrawChar(270, 154, '>', 3, ILI9341_YELLOW, ILI9341_DARKBLUE);

   lastADCTick = HAL_GetTick();

   while (1)
   {
    uint32_t currentTick = HAL_GetTick();
    if(currentTick - lastADCTick >= 10)
    {   
        float dt = (currentTick - lastADCTick) / 1000.0f;
        lastADCTick = currentTick;

        adcValue = ADC1_Read();

        if(Touch_Read(&touchX, &touchY))
        {
            if(touchX >= 5 && touchX < 60 && touchY >= 105  && touchY < 130)
            {
                buttonPressedCount = buttonPressedCount + 1;
                if(buttonPressedCount == 20)
                {
                    motorMode = !motorMode;
                    buttonPressedCount = 0;
                }
            }
            else if(touchX >= 175 && touchX < 230 && touchY >= 45  && touchY < 85) // motor 1 sol
            {
                screenPulse1 = screenPulse1 - 5;
                if(screenPulse1 < 500)
                {
                    screenPulse1 = 500;
                }
            }
            else if(touchX >= 250 && touchX < 305 && touchY >= 45  && touchY < 85) // motor 1 sağ
            {
                screenPulse1 = screenPulse1 + 5;
                if(screenPulse1 > 2500)
                {
                    screenPulse1 = 2500;
                }
            }
            else if(touchX >= 175 && touchX < 230 && touchY >= 145  && touchY < 185) // motor 2 sol
            {
                screenPulse2 = screenPulse2 - 5;
                if(screenPulse2 < 500)
                {
                    screenPulse2 = 500;
                }
            }
            else if(touchX >= 250 && touchX < 305 && touchY >= 145  && touchY < 185) // motor 2 sağ
            {
                screenPulse2 = screenPulse2 + 5;
                if(screenPulse2 > 2500)
                {
                    screenPulse2 = 2500;
                }
            }
        }
        else
        {
            UART1_Send("TOUCH READ ERROR\r\n");
        }
        
        if (MPU6500_ReadAccelGyro(&imuData) && motorMode == PHYSICAL_MODE)
        {
            float roll = Orientation_Update_Roll(imuData.ax, imuData.az, imuData.gy, dt);
            if (roll < -90.0f){roll = -90.0f;} else if (roll > 90.0f){roll = 90.0f;}

            angle1 = (uint16_t)(roll + 90.0f);
            uint32_t pulse2 = SERVO_MIN_US + (uint32_t)(((roll + 90.0f) / 180.0f) * (SERVO_MAX_US - SERVO_MIN_US));
            int32_t diff = (int32_t)pulse2 - (int32_t)lastPulse2;
            if (abs(diff) >= 5)
            {
                Servo2_SetPulse(pulse2);
                lastPulse2 = pulse2;
            }

            angle2 = (adcValue * 180) / ADC_MAX;

            uint32_t pulse = SERVO_MIN_US + ((adcValue * (SERVO_MAX_US - SERVO_MIN_US)) / ADC_MAX);

            diff = (int32_t)pulse - (int32_t)lastPulse;
            if (abs(diff) >= 5)
            {
                Servo_SetPulse(pulse);
                lastPulse = pulse;
            }
        }
        if(motorMode == SCREEN_MODE)
        {
            angle1 = (uint16_t)((float)(screenPulse1 - SERVO_MIN_US) / (float)(SERVO_MAX_US - SERVO_MIN_US) * 180);
            angle2 = (uint16_t)(float)((screenPulse2 - SERVO_MIN_US) / (float)(SERVO_MAX_US - SERVO_MIN_US) * 180);
            Servo_SetPulse(screenPulse1);
            Servo2_SetPulse(screenPulse2);
        }
    /*
        snprintf(
            buffer,
            sizeof(buffer),
            "Touch X: %lu\r\n",
            (unsigned long)rawX
        );

        UART1_Send(buffer);
    */
    }
    if(currentTick - lastLCDTick >= 100)
    {   
        lastLCDTick = currentTick;
        if(angle1 != lastDisplayedAngle1)
        {
            ILI9341_FillRect(95, 50, 72, 28, ILI9341_YELLOW);
            Font_DrawNumber(95, 50, angle1, 4, ILI9341_DARKBLUE, ILI9341_YELLOW);
            lastDisplayedAngle1 = angle1;
        }
        if(angle2 != lastDisplayedAngle2)
        {
            ILI9341_FillRect(95, 150, 72, 28, ILI9341_YELLOW);
            Font_DrawNumber(95, 150, angle2, 4, ILI9341_DARKBLUE, ILI9341_YELLOW);
            lastDisplayedAngle2 = angle2;
        }
        if(motorMode == PHYSICAL_MODE)
        {
            Font_DrawString(70, 110, "PHY", 2, ILI9341_DARKBLUE, ILI9341_YELLOW);
        }
        else
        {
            Font_DrawString(70, 110, "SCR", 2, ILI9341_DARKBLUE, ILI9341_YELLOW);
        }
    }
   }
    
}

void SysTick_Handler(void){
    HAL_IncTick();
}