#ifndef __DATA_H__
#define __DATA_H__

#include "stm32f2xx.h"

struct data_item {
    float agno3_used;
    float volt;
    float delta_v;
    float delta2_v;
};

struct result_data {
    uint8_t magic;
    uint8_t crc;

    uint16_t index;

#define DATA_TYPE_STAND  0x01
#define DATA_TYPE_CL     0x02
    uint8_t type;

    uint8_t valid;
    
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t items_cnt;
    struct data_item items[10];
    float cl_agno3_used;
    float cl_dosage;
    float cl_percentage;
    float ppm;
    float block_agno3_used;
    float agno3_dosage;
};

extern struct result_data *data_get(void);
extern int data_cfg_init(void);
extern int data_save(struct result_data *stream);
extern int data_del(int idx);
extern int data_foreach(struct result_data *stream);
extern int data_get_by_idx(struct result_data *stream, int idx);
extern void data_foreach_start(void);
extern int data_count(void);

#endif
