#include "mpu6500.h"
#include "i2c.h"
#include "stm32f1xx_hal.h"

#define MPU6500_ADRESS 0x68
#define MPU6500_WHO_AM_I_REG 0x75
#define MPU6500_WHO_AM_I_VAL 0x70
#define MPU6500_PWR_MGMT_1 0x6B
#define MPU6500_GYRO_CONFIG   0x1B
#define MPU6500_ACCEL_CONFIG  0x1C
#define MPU6500_ACCEL_XOUT_H  0x3B
#define MPU6500_GYRO_XOUT_H   0x43

#define MPU6500_CALIB_SAMPLES 500
#define MPU6500_ACCEL_1G      16384.0f

#define MPU6500_ACCEL_SENSITIVITY 16384.0f
#define MPU6500_GYRO_SENSITIVITY  131.0f

static float accelBiasX = 0.0f;
static float accelBiasY = 0.0f;
static float accelBiasZ = 0.0f;

static float gyroBiasX = 0.0f;
static float gyroBiasY = 0.0f;
static float gyroBiasZ = 0.0f;

bool MPU6500_Init(void){
    uint8_t whoAmI = I2C1_ReadRegister(MPU6500_ADRESS, MPU6500_WHO_AM_I_REG);
    if (whoAmI != MPU6500_WHO_AM_I_VAL)
    {
        return false;
    }
    
    I2C1_WriteRegister(MPU6500_ADRESS, MPU6500_PWR_MGMT_1, 0x00);
    uint8_t powerReg = I2C1_ReadRegister(MPU6500_ADRESS, MPU6500_PWR_MGMT_1);
    if (powerReg != 0x00)
    {
        return false;
    }

    I2C1_WriteRegister(MPU6500_ADRESS, MPU6500_ACCEL_CONFIG, 0x00);
    I2C1_WriteRegister(MPU6500_ADRESS, MPU6500_GYRO_CONFIG, 0x00);

    return true;
}

bool MPU6500_ReadAccelGyroRaw(MPU6500_RawData *raw){
    uint8_t data[14];
    if (!I2C1_ReadRegisters(MPU6500_ADRESS, MPU6500_ACCEL_XOUT_H, data, 14))
    {
        return false;
    }

    raw->ax = (int16_t)((data[0]<<8) | data[1]);
    raw->ay = (int16_t)((data[2]<<8) | data[3]);
    raw->az = (int16_t)((data[4]<<8) | data[5]);
    raw->gx = (int16_t)((data[8]<<8) | data[9]);
    raw->gy = (int16_t)((data[10]<<8) | data[11]);
    raw->gz = (int16_t)((data[12]<<8) | data[13]);

    return true;
}

bool MPU6500_CalibrateAccelGyro(void){
    int64_t sumAx = 0;
    int64_t sumAy = 0;
    int64_t sumAz = 0;

    int64_t sumGx = 0;
    int64_t sumGy = 0;
    int64_t sumGz = 0;

    MPU6500_RawData raw;

    for(uint16_t i = 0; i < MPU6500_CALIB_SAMPLES; i++)
    {
        if(!MPU6500_ReadAccelGyroRaw(&raw))
        {
            return false;
        }

        sumAx += raw.ax;
        sumAy += raw.ay;
        sumAz += raw.az;

        sumGx += raw.gx;
        sumGy += raw.gy;
        sumGz += raw.gz;

        HAL_Delay(2);
    }
    // Gyro Bias
    gyroBiasX = (float)sumGx / MPU6500_CALIB_SAMPLES;
    gyroBiasY = (float)sumGy / MPU6500_CALIB_SAMPLES;
    gyroBiasZ = (float)sumGz / MPU6500_CALIB_SAMPLES;
    // Accel Bias
    accelBiasX = (float)sumAx / MPU6500_CALIB_SAMPLES;
    accelBiasY = (float)sumAy / MPU6500_CALIB_SAMPLES;
    accelBiasZ = (float)sumAz / MPU6500_CALIB_SAMPLES + MPU6500_ACCEL_1G;

    return true;
}

bool MPU6500_ReadAccelGyro(MPU6500_Data *data)
{
    MPU6500_RawData raw;

    if (!MPU6500_ReadAccelGyroRaw(&raw))
    {
        return false;
    }
    // accel
    data->ax = ((float)raw.ax - accelBiasX) / MPU6500_ACCEL_SENSITIVITY;
    data->ay = ((float)raw.ay - accelBiasY) / MPU6500_ACCEL_SENSITIVITY;
    data->az = ((float)raw.az - accelBiasZ) / MPU6500_ACCEL_SENSITIVITY;
    // gyro
    data->gx = ((float)raw.gx - gyroBiasX) / MPU6500_GYRO_SENSITIVITY;
    data->gy = ((float)raw.gy - gyroBiasY) / MPU6500_GYRO_SENSITIVITY;
    data->gz = ((float)raw.gz - gyroBiasZ) / MPU6500_GYRO_SENSITIVITY;

    if (data->gx < 0.2){data->gx = 0.0f;} if (data->gy < 0.2){data->gy = 0.0f;} if (data->gz < 0.2){data->gz = 0.0f;}

    return true;
}
