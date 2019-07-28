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

#define DATA_TYPE_STAND    0x01
#define DATA_TYPE_CL       0x02
#define DATA_TYPE_DORRPER  0x03
#define DATA_TYPE_EXTEST   0x04
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

    float block_agno3_used2;
    float cl_agno3_used2;
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

struct data_usb_cmd {
    struct lb_idx *table;
    int len;

#define USB_INVALID       0xA0    
#define USB_READY         0xA1
#define USB_EXPORT        0xA2
#define USB_ERROR         0xA3
#define USB_QUIT          0xA4
#define USB_MKDIR         0xA5    
    int cmd;
};

extern int data_save(struct result_data *stream);
extern int data_del(int idx);
extern int data_delall(void);

extern int data_get(struct result_data *stream, int idx);
extern struct data_ui *data_ui_get(int index);

extern int data_count(void);

extern int data_init(void);
extern int data_indextable_update(struct lb_idx *lbi);
extern int data_export(struct lb_idx *table, int len);
extern int data_usb_detect(void);
extern int data_mkdir(void);

#endif
