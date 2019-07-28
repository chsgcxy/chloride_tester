#ifndef __EXPERIMENT_H__
#define __EXPERIMENT_H__

#include "WM.h"

struct exper_msg {
    int stop;

#define EXPER_MSG_NONE           0x00
#define EXPER_MSG_AGNO3_START    0x01
#define EXPER_MSG_BLOCK_START    0x02
#define EXPER_MSG_CL_START       0x03
#define EXPER_MSG_STAND_START    0x04
#define EXPER_MSG_OIL_GET        0x05
#define EXPER_MSG_OIL_PUT        0x06
#define EXPER_MSG_OIL_CLEAR      0x07
#define EXPER_MSG_DROPPER_START  0x08

/* add this for extest, a new exper method, shit..... */
#define EXPER_MSG_AGNO3_EXTEST_START1    0x10
#define EXPER_MSG_AGNO3_EXTEST_START2    0x11
#define EXPER_MSG_BLOCK_EXTEST_START1    0x12
#define EXPER_MSG_BLOCK_EXTEST_START2    0x13
#define EXPER_MSG_CL_EXTEST_START1       0x14
#define EXPER_MSG_CL_EXTEST_START2       0x15

#define DATA_MSG_DEL             0xA0
#define DATA_MSG_DEL_ALL         0xA1

#define ERROR_MOTO               0xB1
#define ERROR_DATA_LOOKUP        0xB2

#define INFO_DATE_SAVE           0xC1
#define INFO_ZSB_CALI            0xC2

#define INFO_DATA_EXPORT         0xD1
#define INFO_DATA_EXPROTING      0xD2
    int msg;
};

struct exper_buf {
    float volt;
    float agno3_used;
};

/**
 * about agno3_used, every exper has to recode for used
 * agno3_agno3_used means agno3_used when agno3 dosage test
 * experiment, agno3_agno3_used2 means a extern test method
 * for agno3_used when agno3 dosage test. The same with block
 * test and cl- test.
 * About extest, you can find it with doc/GBT-8077-2012
 */
struct exper_data {
    float volt;  // the current volt value
    float agno3_used;
    int *agno3_stock; // the current agno3 stock
    float *stock_percentage;

    /* agno3 dosage test */
    float nacl_dosage;
    float agno3_dosage;
    float agno3_agno3_used;
    float agno3_agno3_used2; // only extest use
    
    /* block test */
    float block_agno3_used;
    float block_agno3_used2; // only extest use

    /* cl- test */
    float cl_agno3_used;
    float cl_agno3_used2; // only extest use

    // test shuini use weight
    float sample_weight;
    float cl_percentage;

    // test solution use volume
    int sample_volume;
    float cl_dosage;
    float ppm;
};

struct exper_stat {
#define EXPER_STAT_ERR_MOTOR             0x01
#define EXPER_STAT_UPDATE_PROGRESS       0x02
#define EXPER_STAT_UPDATE_GRAPH          0x03
#define EXPER_STAT_OIL_GET_FINISHED      0x04
#define EXPER_STAT_OIL_PUT_FINISHED      0x05
#define EXPER_STAT_OIL_CLEAR_FINISHED    0x06
#define EXPER_STAT_AGNO3_FINISHED        0x07
#define EXPER_STAT_BLOCK_FINISHED        0x08
#define EXPER_STAT_CL_FINISHED           0x09
#define EXPER_STAT_STAND_FINISHED        0x0A
#define EXPER_STAT_FAIL                  0x0B
#define EXPER_STAT_UPDATE_AGNO3_USED     0x0C
#define EXPER_STAT_DROPPER_FINISHED      0x0D

/**
 * add this for extest, correspond to exper_msg,
 * oh shit................
 */
#define EXPER_STAT_AGNO3_EXTEST_FINISHED1    0x20
#define EXPER_STAT_AGNO3_EXTEST_FINISHED2    0x21
#define EXPER_STAT_BLOCK_EXTEST_FINISHED1    0x22
#define EXPER_STAT_BLOCK_EXTEST_FINISHED2    0x23
#define EXPER_STAT_CL_EXTEST_FINISHED1       0x24
#define EXPER_STAT_CL_EXTEST_FINISHED2       0x25

    int stat;
    struct exper_data data;
};

extern void exper_msg_set(struct exper_msg *msg, int idx);
extern void exper_task(void *args);
extern char exper_busy(void);
extern float exper_volt_get(void);
extern void exper_init(void);
extern void exper_print_report(int index);
extern void exper_data_get(struct exper_data *data, int idx);
extern void exper_data_set(struct exper_data *data, int idx);
extern float exper_filter(void);

#endif
