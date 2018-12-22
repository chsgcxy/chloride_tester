#include "experiment.h"
#include "string.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "WM.h"
#include "main.h"
#include "stepmotor.h"
#include "ad770x.h"
#include "report.h"

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
    float volt[50];
    float agno3_used[50];
    float volt_diff;
    int count;
    int jump;
    int steps;
    float pre_AgNO3_dosage;
    float pre_AgNO3_used;
    float block_AgNO3_used;
    float true_AgNO3_used;
    float cl_percentage;
};

static struct exper_stat g_exper_stat;
static struct exper_msg g_exper_msg;
static struct WM_MESSAGE msg;
static int g_oil_stock = 0;

static float volt_buff[20];
static struct exper g_exper;
static struct report report_test;

struct report *exper_get_report(void)
{
    return &report_test;
}

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
            g_exper_stat.oil_stock = 0;
            g_oil_stock = 0;
            WM_BroadcastMessage(&msg);
            g_exper_stat.stat = EXPER_STAT_ERR_MOTOR;
            WM_BroadcastMessage(&msg);
            return;
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
            vTaskDelay(50);
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
            g_exper_stat.oil_stock = 0;
            g_oil_stock = 0;
            WM_BroadcastMessage(&msg);
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

#if 0
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

static float count_agno3_used(struct exper *exp)
{
    float delta_pre, delta_delta_pre, delta_after, delta_delta_after, delta_cur;
    float res;

    EXPER_DBG_PRINT("count AgNO3 used\r\n");
    delta_pre = exp->volt[exp->jump] - exp->volt[exp->jump - 1];
    delta_cur = exp->volt[exp->jump + 1] - exp->volt[exp->jump];
    delta_after = exp->volt[exp->jump + 2] - exp->volt[exp->jump + 1];
    delta_delta_pre = delta_cur - delta_pre;
    delta_delta_after = delta_after - delta_cur;
    EXPER_DBG_PRINT("exp->jump = %d\r\n", exp->jump);
    EXPER_DBG_PRINT("delta_pre = %f, delta_cur = %f, delta_after = %f\r\n",
        delta_pre, delta_cur, delta_after);
    EXPER_DBG_PRINT("delta_delta_pre = %f, delta_delta_after = %f\r\n",
        delta_delta_pre, delta_delta_after);
    res = exp->agno3_used[exp->jump] + (delta_delta_pre / (delta_delta_pre - delta_delta_after)) * 0.1;
    EXPER_DBG_PRINT("actual AgNO3 used = %f\r\n", res);
    return res;
}

static void do_test(int mode)
{
    float volt_scale = 210.0; // 230mV to change step
    float step_ml = 0.3;
    int step = 3;
    float volt_diff = 0.0;
    float volt;
    int index = 0;
    int ext_delay = 0;

    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    g_exper_stat.graph_pos.x = 0;
    g_exper.count = 0;
    g_exper.volt_diff = 0.0;
    g_exper_stat.agno3_used = 0;
    g_exper_stat.graph_pos.x = 0;

    switch (mode) {
    case EXPER_MSG_AGNO3_START:
        g_exper.steps = 25;
        break;
    case EXPER_MSG_CL_START:
    case EXPER_MSG_BLOCK_START:
        g_exper.steps = 20;
        break;
    default:
        return;
    }

    while (1) {
        /* user stop */
        if (g_exper_msg.stop)
            return;

        /* beyond scale, change to 0.1ml per step */
        if (volt > volt_scale) {
            step_ml = 0.1;
            step = 1;
        }

        if (g_exper_stat.oil_stock < 10)
            _exper_oil_get();

        /* put AgNo3 oil */
        relay_ctrl(MOTOR_WATER_PUT);
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
       
        EXPER_DBG_PRINT("\r\n\r\n");
        if (step == 1) {
            vTaskDelay(10000 + ext_delay);
            volt = exper_filter();
        } else {
            vTaskDelay(3000);
            volt = exper_filter();
        }
        EXPER_DBG_PRINT("step = %d, volt = %f, AgNo3 used %.3fmL\r\n",
            step, volt, g_exper_stat.agno3_used);
        
        /* do not care step 0.3 */
        if (step == 1) {
            g_exper.volt[g_exper.count] = volt;
            g_exper.agno3_used[g_exper.count] = g_exper_stat.agno3_used;
            g_exper.count++;
            g_exper.steps--;
            if (g_exper.count > 1) {
                volt_diff = (g_exper.volt[g_exper.count - 1]) - (g_exper.volt[g_exper.count - 2]);
                if (g_exper.volt_diff < volt_diff) {
                    g_exper.volt_diff = volt_diff;
                    g_exper.jump = g_exper.count - 2;
                }
                
                if (volt_diff > 5.0)
                    ext_delay = 5000;
                else
                    ext_delay = 0;
                EXPER_DBG_PRINT("derta = %f, gdiff = %f, jump = %d, ext_delay = %d\r\n",
                    volt_diff, g_exper.volt_diff, g_exper.jump, ext_delay);
            }
        }

        /* update UI */
        g_exper_stat.graph_pos.y = (int)volt;
        g_exper_stat.stat = EXPER_STAT_UPDATE_GRAPH;
        g_exper_stat.volt = volt;
        WM_BroadcastMessage(&msg);

        g_exper_stat.oil_stock = g_oil_stock * 100 / EXPER_TOTAL_ML;
        g_exper_stat.stat = EXPER_STAT_UPDATE_PROGRESS;
        WM_BroadcastMessage(&msg);

        if (g_exper.steps == 0) {
            switch (mode) {
            case EXPER_MSG_AGNO3_START:
                g_exper.pre_AgNO3_used = count_agno3_used(&g_exper);
                g_exper.pre_AgNO3_dosage = (float)0.2 / g_exper.pre_AgNO3_used;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\nAgNo3 test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", g_exper.pre_AgNO3_used);
                EXPER_DBG_PRINT("AgNo3 nongdu is %.4f\r\n", g_exper.pre_AgNO3_dosage);

                g_exper_stat.agno3_used = g_exper.pre_AgNO3_used;
                g_exper_stat.agno3_consistence = g_exper.pre_AgNO3_dosage;
                g_exper_stat.stat = EXPER_STAT_AGNO3_FINISHED;
                WM_BroadcastMessage(&msg);
                return;
            case EXPER_MSG_BLOCK_START:
                g_exper.block_AgNO3_used = count_agno3_used(&g_exper);

                EXPER_DBG_PRINT("\r\n\r\n\r\nblock test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", g_exper.block_AgNO3_used);

                g_exper_stat.agno3_used = g_exper.block_AgNO3_used;
                g_exper_stat.stat = EXPER_STAT_BLOCK_FINISHED;
                WM_BroadcastMessage(&msg);
                return;
            case EXPER_MSG_CL_START:
                g_exper.true_AgNO3_used = count_agno3_used(&g_exper);
                g_exper.cl_percentage = (g_exper.pre_AgNO3_dosage * (float)3.545 * (g_exper.true_AgNO3_used - g_exper.block_AgNO3_used)) / (float)5;

                EXPER_DBG_PRINT("\r\n\r\n\r\ncl test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", g_exper.true_AgNO3_used);
                EXPER_DBG_PRINT("cl percentage is %f%%\r\n", g_exper.cl_percentage);
                
                g_exper_stat.agno3_used = g_exper.true_AgNO3_used;
                g_exper_stat.cl_percentage = g_exper.cl_percentage;
                g_exper_stat.stat = EXPER_STAT_CL_FINISHED;
                WM_BroadcastMessage(&msg);

                for (index = 0; index < 12; index++)
                    report_test.data[index] = g_exper.volt[g_exper.jump - 5 + index] - g_exper.volt[g_exper.jump - 6 + index];
                report_test.data_num = 12;
                report_test.nitrate_dosage = g_exper.true_AgNO3_used;
                report_test.percentage = g_exper.cl_percentage;
                report_test.year = 18;
                report_test.month = 12;
                report_test.day = 8;
                report_test.hour = 16;
                report_test.minute = 0;
                return;
            default:
                return;
            }
        }
    }
}

void exper_task(void *args)
{
    msg.MsgId = WM_USER;
    msg.hWinSrc = 0;
    msg.Data.p = &g_exper_stat;
    
    while (1) {
        switch (g_exper_msg.msg) {
        case EXPER_MSG_NONE:
            break;
        case EXPER_MSG_AGNO3_START:
            EXPER_DBG_PRINT("EXPER_MSG_AGNO3_START\r\n");
            //do_test(EXPER_MSG_AGNO3_START);
            g_exper_stat.stat = EXPER_STAT_AGNO3_FINISHED;
            WM_BroadcastMessage(&msg);
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_BLOCK_START:
            EXPER_DBG_PRINT("EXPER_MSG_BLOCK_START\r\n");
            //do_test(EXPER_MSG_BLOCK_START);
            g_exper_stat.stat = EXPER_STAT_BLOCK_FINISHED;
            WM_BroadcastMessage(&msg);
            g_exper_msg.msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_CL_START:
            EXPER_DBG_PRINT("EXPER_MSG_CL_START\r\n");
            //do_test(EXPER_MSG_CL_START);
            g_exper_stat.stat = EXPER_STAT_CL_FINISHED;
            WM_BroadcastMessage(&msg);
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
