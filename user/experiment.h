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
    int msg;
};

struct exper_buf {
    float volt;
    float agno3_used;
};

struct exper_data {
    float volt;  // the current volt value
    float agno3_used;
    int *agno3_stock; // the current agno3 stock
    float *stock_percentage;

    /* agno3 dosage test */
    float nacl_dosage;
    float agno3_dosage;
    float agno3_agno3_used;

    /* block test */
    float block_agno3_used;
    
    /* cl- test */
    float cl_agno3_used;
    // test shuini use weight
    int sample_weight;
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

#endif
