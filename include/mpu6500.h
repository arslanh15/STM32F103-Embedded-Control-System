#ifndef MPU6500_H
#define MPU6500_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} MPU6500_RawData;

typedef struct
{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
} MPU6500_Data;

bool MPU6500_Init(void);
bool MPU6500_ReadAccelGyroRaw(MPU6500_RawData *raw);
bool MPU6500_CalibrateAccelGyro(void);
bool MPU6500_ReadAccelGyro(MPU6500_Data *data);

#endif