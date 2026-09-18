#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void Servo_Init(void);
void Servo_Start(void);
void Servo_SetPulse(uint32_t pulse);
void Servo2_SetPulse(uint32_t pulse);

#endif