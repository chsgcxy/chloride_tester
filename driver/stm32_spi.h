#ifndef __STM32_SPI_H__
#define __STM32_SPI_H__

#include "stm32f2xx.h"

extern int spi1_init(void);
extern int spi2_init(void);
extern int spi3_init(void);


#define SPI_MODE_REG    0
#define SPI_MODE_LIB    1

extern uint16_t spi_send_byte(SPI_TypeDef *SPIx, uint8_t data, int mode);

#endif
