#ifndef __STM32_SPI_H__
#define __STM32_SPI_H__

#include "stm32f2xx.h"

extern int spi1_init(void);
extern uint16_t spi1_send_byte(uint8_t data);

extern int spi3_init(void);

#endif
