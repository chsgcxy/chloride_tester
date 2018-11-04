#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f2xx.h"

extern int spi1_init(void);
extern u8 spi1_data_rw(u8 TxData);

#endif
