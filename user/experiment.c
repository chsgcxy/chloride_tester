#include "experiment.h"
#include "string.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "WM.h"
#include "main.h"

static struct exper_msg priv;
static struct WM_MESSAGE msg;

static void exper_start(void)
{
    int i;

    for (i = 0; i <= 100; i++) {
        priv.progress = i;
        msg.MsgId = WM_USER;
        msg.hWinSrc = 0;
        msg.Data.p = &priv;
        WM_BroadcastMessage(&msg);
        vTaskDelay(50);
    }
}

void exper_task(void *args)
{
    while (1) {
        switch (priv.msg) {
        case EXPER_MSG_NONE:
            break;
        case EXPER_MSG_START:
            exper_start();
            priv.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_STOP:
            break;
        default:
            break;
        }
    }
}


void exper_msg_set(struct exper_msg *msg)
{
    memcpy(&priv, msg, sizeof(struct exper_msg));
}
