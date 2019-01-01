#ifndef __DATA_H__
#define __DATA_H__

#include "stm32f2xx.h"


struct item {
    float agno3_used;
    float volt;
    float delta_v;
    float delta2_v;
};

struct data {
    uint8_t magic;
    uint8_t type;
    uint16_t index;
    uint8_t crc8;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t items_cnt;
    struct item items[9];
    float agno3_used;
    float res;
    float ppm;
};

extern struct data *data_get(void);

#endif
