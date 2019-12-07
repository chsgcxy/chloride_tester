#ifndef __BSP_H__
#define __BSP_H__

#include "stm32f2xx.h"
#include "stdio.h"

#define BSP_DBG

#ifdef BSP_DBG
    #define BSP_ERR(fmt, args...)      printf(fmt, ##args)
    #define BSP_INFO(fmt, args...)      printf(fmt, ##args)
#else
    #define BSP_ERR(fmt, args...)
    #define BSP_INFO(fmt, args...)
#endif

enum SPI_CS {
    SPI_CS_LOW = 0,
    SPI_CS_HIGH,
};

struct spi_bus {
    int (*init)(void);
    int (*send_byte)(uint8_t data);
    void (*cs_ctrl)(int cs);
};

#endif
