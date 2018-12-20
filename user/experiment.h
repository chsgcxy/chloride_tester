#ifndef __EXPERIMENT_H__
#define __EXPERIMENT_H__

#include "WM.h"

struct exper_msg {
    int stop;

#define EXPER_MSG_NONE           0x00
#define EXPER_MSG_AGNO3_START    0x01
#define EXPER_MSG_BLOCK_START    0x02
#define EXPER_MSG_CL_START       0x03
#define EXPER_MSG_OIL_GET        0x04
#define EXPER_MSG_OIL_PUT        0x05
#define EXPER_MSG_OIL_CLEAR      0x06
    int msg;
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
#define EXPER_STAT_FAIL                  0x0A
    int stat;
    GUI_POINT graph_pos;
    int oil_stock;
    float volt;
    float agno3_used;
    float agno3_consistence;
    float cl_consistence;
    float cl_percentage;
};

extern void exper_msg_set(struct exper_msg *msg);
extern void exper_task(void *args);
extern struct report *exper_get_report(void);

#endif
