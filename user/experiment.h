#ifndef __EXPERIMENT_H__
#define __EXPERIMENT_H__

struct exper_msg {
#define EXPER_MSG_NONE     0x00
#define EXPER_MSG_START    0x01
#define EXPER_MSG_STOP     0x02
    int msg;
    int stop;
    int progress;
};

extern void exper_msg_set(struct exper_msg *msg);
extern void exper_task(void *args);

#endif
