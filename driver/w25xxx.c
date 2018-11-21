#include "w25xxx.h"
#include "stm32_spi.h"
#include "string.h"
#include "stdio.h"

#define W25_RD_STAT       0x05
#define W25_WR_ENABLE     0x06
#define W25_WR_DISABLE    0x04
#define W25_RD_DATA       0x03
#define W25_WR_PAGE       0x02

#define W25_ERASE_SECTOR  0x20
#define W25_ERASE_CHIP    0xC7

#define W25_RD_DEVID      0x90

#define W25_CS_HIGH()     GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define W25_CS_LOW()      GPIO_ResetBits(GPIOB, GPIO_Pin_12)

static uint8_t block_buf[W25X20_BLOCK_SIZE];

static u8 w25xxx_send_byte(u8 byte)
{
    return (spi_send_byte(SPI2, byte, SPI_MODE_LIB) & 0xff);
}

void w25xxx_write_enable(void)
{
    W25_CS_LOW();
    w25xxx_send_byte(W25_WR_ENABLE);
    W25_CS_HIGH();
}
void w25xxx_write_disable(void)
{
    W25_CS_LOW();
    w25xxx_send_byte(W25_WR_DISABLE);
    W25_CS_HIGH();
}

static void w25xxx_wait(void)
{
    u8 stat;

    W25_CS_LOW();
    w25xxx_send_byte(W25_RD_STAT);

    do {
        stat = w25xxx_send_byte(0xff);
    } while (stat & 0x01);
    printf("stat = 0x%02x\r\n", stat);
    W25_CS_HIGH();
}

void w25xxx_read_sector(uint8_t *buf, u32 sector)
{
    u32 offset = sector * W25X20_SECTOR_SIZE;
    int count = W25X20_SECTOR_SIZE;

    W25_CS_LOW();
    w25xxx_send_byte(W25_RD_DATA);
    w25xxx_send_byte((offset & 0xFF0000) >> 16);
    w25xxx_send_byte((offset & 0xFF00) >> 8);
    w25xxx_send_byte(offset & 0xFF);
    while (count--)
        *buf++ = w25xxx_send_byte(0xff);
    W25_CS_HIGH();
}

static void w25xxx_erase_block(u32 offset)
{
    w25xxx_write_enable();
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
    w25xxx_write_enable();
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

static void w25xxx_write_sector(uint8_t *buf, u32 sector)
{
    u32 offset = sector * W25X20_SECTOR_SIZE;
    int count = W25X20_SECTOR_SIZE;
    
    w25xxx_write_enable();
    W25_CS_LOW();
    w25xxx_send_byte(W25_WR_PAGE);
    w25xxx_send_byte((offset & 0xFF0000) >> 16);
    w25xxx_send_byte((offset & 0xFF00) >> 8);
    w25xxx_send_byte(offset & 0xFF);

    while (count--)
        w25xxx_send_byte(*buf++);

    W25_CS_HIGH();
    w25xxx_wait();
}

void w25xxx_write(uint8_t *buf, u32 sector)
{ 
    u32 sector_align, block_addr;
    int i;
    int erase_flag = 0;
    uint8_t *p = block_buf;

    sector_align = (sector / W25X20_SECTOR_PER_BLOCK) * W25X20_SECTOR_PER_BLOCK;
    block_addr = sector_align * W25X20_SECTOR_SIZE;
    printf("%s: sector_align = %d, block_addr = 0x%04x\r\n",
        __FUNCTION__, sector_align, block_addr);

    w25xxx_read_sector(p, sector);
    for (i = 0; i < W25X20_SECTOR_SIZE; i++) {
        if ((p[i] != buf[i]) && (p[i] != 0xFF)) {
            erase_flag = 1;
            printf("%s: need erase\r\n", __FUNCTION__);
            break;
        }
    }

    if (erase_flag) {
        for (i = 0; i < W25X20_SECTOR_PER_BLOCK; i++, p += W25X20_SECTOR_SIZE) {
            if ((sector_align + i) == sector)
                memcpy(p, buf, W25X20_SECTOR_SIZE);
            else
                w25xxx_read_sector(p, sector_align + i);
        }
            
        w25xxx_erase_block(block_addr);

        p = block_buf;
        for (i = 0; i < W25X20_SECTOR_PER_BLOCK; i++, p += W25X20_SECTOR_SIZE)
            w25xxx_write_sector(p, sector_align + i);
    } else
        w25xxx_write_sector(buf, sector);
}

int w25xxx_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    return 0;
}
