#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdbool.h>

void SPI1_Init(void);
bool SPI1_Transmit(const uint8_t *data, uint16_t size);
bool SPI1_Receive(uint8_t *data, uint16_t size);

#endif