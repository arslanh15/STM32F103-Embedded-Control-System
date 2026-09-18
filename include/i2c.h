#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

void I2C1_Init(void);

uint8_t I2C1_ReadRegister(uint8_t deviceAddress, uint8_t reg);
void I2C1_WriteRegister(uint8_t deviceAddress, uint8_t reg, uint8_t data);
bool I2C1_ReadRegisters(uint8_t deviceAddress, uint8_t startReg, uint8_t *data, uint16_t length);

#endif