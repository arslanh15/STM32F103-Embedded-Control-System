#include "orientation.h"
#include <math.h>

#define RAD_TO_DEG 57.2957795f
#define COMPLEMENTARY_ALPHA 0.90f

static float roll = 0.0f;

void Orientation_Init(float ax, float az){
    roll = atan2f(ax, -az) * RAD_TO_DEG;
}

float Orientation_Update_Roll(float ax, float az, float gy, float dt){
    float rollAcc = atan2f(ax, -az) * RAD_TO_DEG;
    float rollGyro = roll + gy * dt;

    roll = (1-COMPLEMENTARY_ALPHA) * rollAcc + COMPLEMENTARY_ALPHA * rollGyro;

    return roll;
}