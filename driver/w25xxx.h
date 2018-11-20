#ifndef __W25XXX_H__
#define __W25XXX_H__

#include "stm32f2xx.h"

#define W25X20_SECTOR_COUNT          (1024)
#define W25X20_SECTOR_SIZE           (256)
#define W25X20_SECTOR_PER_BLOCK      (8)
#define W25X20_BLOCK_SIZE            (W25X20_SECTOR_PER_BLOCK * W25X20_SECTOR_SIZE)


extern void w25xxx_write(uint8_t *buf, u32 sector);
extern void w25xxx_read_sector(uint8_t *buf, u32 sector);

#endif 
