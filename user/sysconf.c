#include "sysconf.h"
#include "stdio.h"
#include "w25xxx.h"
#include "string.h"

#define SYSCONF_SECTOR    0
#define SYSCONF_MAGIC     0xA55A0005

static struct sysconf g_cfg;
static uint8_t buf[W25X20_SECTOR_SIZE];
static int valid_flag;

int sysconf_load(void)
{
    w25xxx_read_sector(buf, SYSCONF_SECTOR);
    memcpy(&g_cfg, buf, sizeof(struct sysconf));
    if (g_cfg.magic == SYSCONF_MAGIC)
        valid_flag = 1;
    else
        valid_flag = 0;
    return valid_flag;
}

int sysconf_save(void)
{
    g_cfg.magic = SYSCONF_MAGIC;
    memcpy(buf, &g_cfg, sizeof(struct sysconf));
    w25xxx_write(buf, SYSCONF_SECTOR);
    return 0;
}

struct sysconf *sysconf_get(void)
{
    return &g_cfg;
}

int sysconf_is_valid(void)
{
    return valid_flag;
}

