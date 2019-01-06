#ifndef __REPORT_H__
#define __REPORT_H__

#include "stm32f2xx.h"
#include "rd-tprinter.h"

struct report_data {
    float volt;
    float agno3_used;
    float delta_volt;
};

struct report {
#define REP_TYPE_STAND  0x01
#define REP_TYPE_CL     0x02
    int type;

    struct report_data data[10];
    uint8_t data_num;
    
    float cl_agno3_used;
    float cl_dosage;
    float cl_percentage;
    float ppm;

    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
};

extern int report_init(struct tprinter *printer);
extern int report_show(struct report *rp);

#endif
