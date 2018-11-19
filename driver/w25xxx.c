#include "w25xxx.h"
#include "stm32_spi.h"

#define W25_RD_STAT       0x05

#define W25_RD_DATA_FAST  0x0B
#define W25_WR_PAGE       0x02

#define W25_ERASE_SECTOR  0x20
#define W25_ERASE_CHIP    0xC7

#define W25_RD_DEVID      0x90

#define W25_CS_HIGH()
#define W25_CS_LOW()

static uint8_t sector_buf[4096];

static u8 w25xxx_send_byte(u8 byte)
{
    return (spi_send_byte(SPI2, byte, SPI_MODE_LIB) & 0xff);
}

void w25xxx_wait(void)
{
    u8 stat;

    W25_CS_LOW();
    w25xxx_send_byte(W25_RD_STAT);

    do {
        stat = w25xxx_sendbyte(0xff);
    } while (stat & 0x01);

    W25_CS_HIGH();
}

void w25xxx_read_page(uint8_t *buf, u32 sector_num)
{
    u32 offset = sector_num * W25X20_SECTOR_SIZE;
    W25_CS_LOW();
    w25xxx_send_byte(W25_RD_DATA_FAST);
    w25xxx_send_byte((offset & 0xFF0000) >> 16);
    w25xxx_send_byte((offset & 0xFF00) >> 8);
    w25xxx_send_byte(offset & 0xFF);
    w25xxx_send_byte(0xff);
    while (count--)
        *buf++ = w25xxx_send_byte(0xff);
    W25_CS_HIGH();
}

void w25xxx_erase_sector(u32 offset)
{
    W25_CS_LOW();
    w25xxx_send_byte(W25_ERASE_SECTOR);
    w25xxx_send_byte((offset & 0xFF0000) >> 16);
    w25xxx_send_byte((offset & 0xFF00) >> 8);
    w25xxx_send_byte(offset & 0xFF);
    W25_CS_HIGH();
    w25xxx_wait();
}

void w25xxx_erase_chip(void)
{
    W25_CS_LOW();
    w25xxx_send_byte(W25_ERASE_CHIP);
    W25_CS_HIGH();
    w25xxx_wait();
}

void w25xxx_read_id(uint8_t *fid, uint8_t *did)
{
    W25_CS_LOW();
    w25xxx_send_byte(W25_RD_DEVID);

    w25xxx_send_byte(0xff);
    w25xxx_send_byte(0xff);
    w25xxx_send_byte(0x00);
    *fid = w25xxx_send_byte(0xff);
    *did = w25xxx_send_byte(0xff);
    W25_CS_HIGH();
}

void w25xxx_write_page(uint8_t *buf, u32 offset, uint8_t count)
{
    w25xxx_WriteEnable();
    W25_CS_HIGH();
    w25xxx_send_byte(Page_Program);
    w25xxx_send_byte((WriteAddr & 0xFF0000) >> 16);
    w25xxx_send_byte((WriteAddr & 0xFF00) >> 8);
    w25xxx_send_byte(WriteAddr & 0xFF);

    while (NumByteToWrite--)
        w25xxx_send_byte(*pBuffer++);

    W25_CS_HIGH();
    w25xxx_WaitForWriteEnd();
}
