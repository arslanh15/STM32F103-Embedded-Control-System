#ifndef TOUCH_H
#define TOUCH_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

void Touch_Init(void);
bool Touch_ReadRawX(uint16_t *rawX);
bool Touch_ReadRawY(uint16_t *rawY);
bool Touch_Read(uint16_t *x, uint16_t *y);

#endif