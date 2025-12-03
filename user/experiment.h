#ifndef __EXPERIMENT_H__
#define __EXPERIMENT_H__

#include "WM.h"

#define EXPER_MSG_NONE                                0x00
#define EXPER_TYPE_CEMENT_SLIVER_NITRATE              0x01
#define EXPER_TYPE_CEMENT_BLOCK                       0x02
#define EXPER_TYPE_CEMENT_CHLORIDE_ION                0x03
#define EXPER_TYPE_OTHER_SLIVER_NITRATE               0x11
#define EXPER_TYPE_OTHER_BLOCK                        0x12
#define EXPER_TYPE_OTHER_CHLORIDE_ION                 0x13
#define EXPER_TYPE_MANUAL_TITRATION                   0x21
#define EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE1          0x31
#define EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE2          0x32
#define EXPER_TYPE_ADMIXTURE_BLOCK1                   0x33
#define EXPER_TYPE_ADMIXTURE_BLOCK2                   0x34
#define EXPER_TYPE_ADMIXTURE_CHLORIDE_ION1            0x35
#define EXPER_TYPE_ADMIXTURE_CHLORIDE_ION2            0x36
#define EXPER_TYPE_ADMIXTURE_SLIVER_NITRATE1_FINISHED 0x37
#define EXPER_TYPE_ADMIXTURE_BLOCK1_FINISHED          0x38
#define EXPER_TYPE_ADMIXTURE_CHLORIDE_ION1_FINISHED   0x39
#define EXPER_MSG_OIL_GET                             0x41
#define EXPER_MSG_OIL_PUT                             0x42
#define EXPER_MSG_OIL_CLEAR                           0x43

#define DATA_MSG_DEL             0xA0
#define DATA_MSG_DEL_ALL         0xA1

#define ERROR_MOTO               0xB1
#define ERROR_DATA_LOOKUP        0xB2

#define INFO_DATE_SAVE           0xC1
#define INFO_ZSB_CALI            0xC2
#define INFO_DJDW_CALC           0xC3

#define INFO_DATA_EXPORT         0xD1
#define INFO_DATA_EXPROTING      0xD2

#define EXPER_STAT_ERR_MOTOR             0xE1
#define EXPER_STAT_UPDATE_PROGRESS       0xE2
#define EXPER_STAT_UPDATE_GRAPH          0xE3
#define EXPER_STAT_OIL_GET_FINISHED      0xE4
#define EXPER_STAT_OIL_PUT_FINISHED      0xE5
#define EXPER_STAT_OIL_CLEAR_FINISHED    0xE6
#define EXPER_STAT_FAIL                  0xF0
#define EXPER_STAT_UPDATE_AGNO3_USED     0xF1

struct exper_msg {
    int stop;
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
    int stat;
    struct exper_data data;
};

extern signed long timer_handle;

extern void exper_msg_set(struct exper_msg *msg, int func);
extern void exper_task(void *args);
extern char exper_busy(void);
extern float exper_volt_get(void);
extern void exper_init(void);
extern void exper_print_report(int index);
extern void exper_data_get(struct exper_data *data, int idx);
extern void exper_data_set(struct exper_data *data, int idx);
extern float exper_filter(void);
extern float djdw_calc(void);
extern void exper_stock_clear(void);

#endif
