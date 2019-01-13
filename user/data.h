#ifndef __DATA_H__
#define __DATA_H__

#include "stm32f2xx.h"

#define DATA_MAX_NUM         100

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

/* this listbox index table provide a method that find
 * data index in data_table fast */
struct lb_idx {
    uint8_t data_idx;
};

struct data_ui {
    char string[26];
    uint8_t valid;
};

extern int data_save(struct result_data *stream);
extern int data_del(int idx);
extern int data_delall(void);

extern int data_get(struct result_data *stream, int idx);
extern struct data_ui *data_ui_get(int index);

extern int data_count(void);

extern int data_init(void);
extern int data_indextable_update(struct lb_idx *lbi);

#endif
