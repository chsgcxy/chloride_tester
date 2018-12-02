#include "experiment.h"
#include "string.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "WM.h"
#include "main.h"
#include "stepmotor.h"
#include "ad770x.h"

#define EXPER_TOTAL_ML        (228)
#define EXPER_DISCARD         (5)
#define EXPER_WINDOWS         (10)
#define EXPER_BUF_CNT         (EXPER_DISCARD * 2 + EXPER_WINDOWS)

#define EXPER_DBG

#ifdef EXPER_DBG
	#define EXPER_DBG_PRINT(fmt, args...)    printf(fmt, ##args)
#else
	#define EXPER_DBG_PRINT(fmt, args...)
#endif

struct exper {
    float volt[250];
    float volt_diff;
    int cur_idx;
    int jump_idx;
    int step_change_idx;
    int ex_step;
};

static struct exper_stat g_exper_stat;
static struct exper_msg g_exper_msg;
static struct WM_MESSAGE msg;
static int g_oil_stock = 0;

static float volt_buff[20];
static struct exper g_exper;


static void _exper_oil_get(void)
{
    int correct = 0;
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_UPDATE_PROGRESS;

    relay_ctrl(MOTOR_WATER_GET);
    while (g_oil_stock < EXPER_TOTAL_ML) {
        if (stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML))
            g_oil_stock = EXPER_TOTAL_ML;
        else {
            g_oil_stock++;
            if (g_oil_stock >= EXPER_TOTAL_ML) {
                g_oil_stock = EXPER_TOTAL_ML - 1;
                correct++;
            }
        }

        if (correct > 20) {
             g_exper_stat.stat = EXPER_STAT_ERR_MOTOR;
             WM_BroadcastMessage(&msg);
        } else {
            g_exper_stat.oil_stock = g_oil_stock * 100 / EXPER_TOTAL_ML;
            WM_BroadcastMessage(&msg);
        }
    }
}

static void exper_oil_get(void)
{
    _exper_oil_get();
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_OIL_GET_FINISHED;
    WM_BroadcastMessage(&msg);
}

static void _exper_oil_put(void)
{
    int correct = 0;
    
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_UPDATE_PROGRESS;
    EXPER_DBG_PRINT("oil put\r\n");
    relay_ctrl(MOTOR_WATER_PUT);
    while (g_oil_stock > 0) {
        if (g_exper_msg.stop) {
            g_exper_msg.stop = 0;
            EXPER_DBG_PRINT("stoped.\r\n");
            return;
        }
            
        if (stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML))
            g_oil_stock = 0;
        else {
            g_oil_stock--;
            if (g_oil_stock <= 0) {
                g_oil_stock = 3;
                correct++;
            }
        }
        
        if (correct > 20) {
             g_exper_stat.stat = EXPER_STAT_ERR_MOTOR;
             WM_BroadcastMessage(&msg);
        } else {
            g_exper_stat.oil_stock = g_oil_stock * 100 / EXPER_TOTAL_ML;
            WM_BroadcastMessage(&msg);
        }
    }
    EXPER_DBG_PRINT("stop while\r\n");
}

static void exper_oil_put(void)
{
    _exper_oil_put();
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_OIL_PUT_FINISHED;
    WM_BroadcastMessage(&msg);
}

static void exper_oil_clear(void)
{
    int i;

    for (i = 0; i < 3; i++) {
        _exper_oil_get();
        _exper_oil_put();
    }
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_OIL_CLEAR_FINISHED;
    WM_BroadcastMessage(&msg); 
}

static void float_swap(float *float1, float *float2)  
{  
    float temp;

    temp = *float1;
    *float1 = *float2;
    *float2 = temp;
}

static float exper_filter(void)
{
	int i, j;
    float volt = 0;

    for (i = 0; i < EXPER_BUF_CNT; i++)
        volt_buff[i] = ad7705_read();

    /* sort */
	for (i = 0; i < EXPER_BUF_CNT; i++) {
		for (j = EXPER_BUF_CNT - 1; j > i; j--) {
			if (volt_buff[j] < volt_buff[j - 1])
				float_swap(&volt_buff[j], &volt_buff[j - 1]);
        }
    }

    /* get avarage */
	for (i = EXPER_DISCARD; i < (EXPER_DISCARD + EXPER_WINDOWS); i++)
        volt += volt_buff[i];
    volt = volt / (float)EXPER_WINDOWS;

#ifdef EXPER_DBG_PRINT
	for (i = 0; i < EXPER_BUF_CNT; i++) {
		if (i == 0)
			EXPER_DBG_PRINT("\r\ndiscard:\r\n");
		else if (i == EXPER_DISCARD)
			EXPER_DBG_PRINT("\r\nwindows:\r\n");
		else if (i == (EXPER_DISCARD + EXPER_WINDOWS))
			EXPER_DBG_PRINT("\r\ndiscard:\r\n");
		else
			;
		EXPER_DBG_PRINT("%lf ", volt_buff[i]);
	}
	EXPER_DBG_PRINT("\r\nresult:\r\n");
	EXPER_DBG_PRINT("%lf\r\n", volt);
#endif
    return volt;
}


static void agno3_start(void)
{
    float volt_scale = 0.0; // 230mV to change step
    float step_ml = 0.3;
    int step = 3;
    float volt_diff = 0.0;
    float volt;

    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.graph_pos.x = 0;

    relay_ctrl(MOTOR_WATER_PUT);
    /* pre get once */
    g_exper.cur_idx = 0;
    g_exper.volt_diff = 0.0;
    g_exper.ex_step = 0;

    while (1) {
        /* user stop */
        if (g_exper_msg.stop)
            return;

        /* beyond scale, change to 0.1ml per step */
        if (volt > volt_scale) {
            step_ml = 0.1;
            step = 1;
        }

        EXPER_DBG_PRINT("step = %d\r\n", step);

        /* put AgNo3 oil */
        if (stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * step)) {
            g_exper_stat.stat = EXPER_STAT_FAIL;
            WM_BroadcastMessage(&msg);
            break;
        } else {
            g_exper_stat.graph_pos.x += step;
            g_exper_stat.agno3_used += step_ml;
            g_oil_stock -= step;
        }
        /* wait oil act */
        vTaskDelay(2000);
        
        /* get volt */
        volt = exper_filter();
        EXPER_DBG_PRINT("volt current = %f\r\n", volt);
     
        /* do not care step 0.3 */
        if (step == 1) {
            if (g_exper.cur_idx > 0) {
                g_exper.volt[g_exper.cur_idx] = volt;
                volt_diff = (g_exper.volt[g_exper.cur_idx]) - (g_exper.volt[g_exper.cur_idx - 1]);
                EXPER_DBG_PRINT("cur volt = %f, pre volt = %f, volt diff = %2f\r\n",
                    g_exper.volt[g_exper.cur_idx], g_exper.volt[g_exper.cur_idx - 1], volt_diff);
                g_exper.cur_idx++;

                if (g_exper.ex_step == 0) {
                    if (g_exper.volt_diff <= volt_diff)
                        g_exper.volt_diff = volt_diff;
                    else {
                        /* set finished flag, and get ex 10 point */
                        EXPER_DBG_PRINT("set finished flag to 10\r\n");
                        g_exper.ex_step = 30;
                    }
                }
            } else {
                g_exper.volt[g_exper.cur_idx++] = volt;
            }

            if (g_exper.ex_step > 0) {
                g_exper.ex_step--;
                if (g_exper.ex_step <= 0) {
                    /* finished */
                    EXPER_DBG_PRINT("AgNo3 test finished.\r\n");
                    g_exper_stat.agno3_consistence = (float)0.02 / g_exper_stat.agno3_used;
                    printf("AgNo3 nongdu is %.2f\r\n", g_exper_stat.agno3_consistence);
                    g_exper_stat.stat = EXPER_STAT_AGNO3_FINISHED;
                    WM_BroadcastMessage(&msg);
                    return;
                }
            }
        }

        /* update UI */
        EXPER_DBG_PRINT("AgNo3 test update UI.\r\n");
        g_exper_stat.graph_pos.y = (int)volt;
        g_exper_stat.stat = EXPER_STAT_UPDATE_GRAPH;
        g_exper_stat.volt = volt;
        WM_BroadcastMessage(&msg);

        g_exper_stat.oil_stock = g_oil_stock * 100 / EXPER_TOTAL_ML;
        g_exper_stat.stat = EXPER_STAT_UPDATE_PROGRESS;
        WM_BroadcastMessage(&msg);
    }
}

static void block_start(void)
{
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_BLOCK_FINISHED;
    WM_BroadcastMessage(&msg);
}

static void cl_start(void)
{
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.stat = EXPER_STAT_CL_FINISHED;
    WM_BroadcastMessage(&msg);
}

void exper_task(void *args)
{
    while (1) {
        switch (g_exper_msg.msg) {
        case EXPER_MSG_NONE:
            break;
        case EXPER_MSG_AGNO3_START:
            agno3_start();
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_BLOCK_START:
            block_start();
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_CL_START:
            cl_start();
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_OIL_GET:
            exper_oil_get();
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_OIL_PUT:
            exper_oil_put();
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_OIL_CLEAR:
            exper_oil_clear();
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        default:
            break;
        }
    }
}


void exper_msg_set(struct exper_msg *msg)
{
    memcpy(&g_exper_msg, msg, sizeof(struct exper_msg));
    printf("get msg.msg=%x, stop=%d\r\n", g_exper_msg.msg, g_exper_msg.stop);
}
