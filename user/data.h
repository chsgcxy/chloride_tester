#ifndef __DATA_H__
#define __DATA_H__

#include "stm32f2xx.h"


struct item {
    float agno3_used;
    float e_mv;
    float delta_emv;
    float delta_vml;
    float delta_ev;
    float delta_2ev2;
};

struct data {
    uint16_t magic;
    uint8_t crc8;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    struct item items[10];
    float nitrate_dosage;
    float percentage;
};


#endif
