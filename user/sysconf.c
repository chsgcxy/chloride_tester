#include "sysconf.h"
#include "stdio.h"
#include "w25xxx.h"
#include "string.h"

#define SYSCONF_SECTOR    0
#define SYSCONF_MAGIC     0xA55A0005

static struct sysconf g_cfg;
static int valid_flag;

int sysconf_load(void)
{
    w25xxx_read_sector((uint8_t *)&g_cfg, SYSCONF_SECTOR, sizeof(struct sysconf));
    if (g_cfg.magic == SYSCONF_MAGIC)
        valid_flag = 1;
    else
        valid_flag = 0;
    return valid_flag;
}

int sysconf_save(void)
{
    g_cfg.magic = SYSCONF_MAGIC;
    w25xxx_write_sector((uint8_t *)&g_cfg, SYSCONF_SECTOR, sizeof(struct sysconf));
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

