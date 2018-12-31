#ifndef __REPORT_H__
#define __REPORT_H__

#include "stm32f2xx.h"
#include "rd-tprinter.h"

struct report {
#define REP_TYPE_STAND  0x01
#define REP_TYPE_CL     0x02
    int type;

    float data[30];
    uint8_t data_num;
    // AgNO3 used
    float nitrate_dosage;
    float percentage;
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
