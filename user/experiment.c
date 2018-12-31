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
    
    int agno3_stock;
    
    int stand_v;
    float stand_agno3_dosage;

    float test_cl_dosage;
    float test_agno3_dosage;
    float test_agno3_used;

    float block_agno3_used;
    
    float cl_agno3_used;
    float cl_percentage;
    int cement_weight;
    
    struct exper_stat estat;
    struct exper_msg emsg;
    struct WM_MESSAGE wmsg;
    
    struct report rep;
};

static struct exper g_exper;
static char exper_is_run = 0;

char exper_busy(void)
{
    return exper_is_run;
}

struct report *exper_get_report(void)
{
    return &g_exper.rep;
}

int exper_agno3_stock_get(void)
{
    return g_exper.agno3_stock * 100 / EXPER_TOTAL_ML;
}

void exper_stand_agno3_dosage_set(float dosage)
{
    g_exper.stand_agno3_dosage = dosage;
}

float exper_stand_agno3_dosage_get(void)
{
    return g_exper.stand_agno3_dosage;
}

int exper_stand_v_get(void)
{
    return g_exper.stand_v;
}

void exper_stand_v_set(int v)
{
    g_exper.stand_v = v;
}

void exper_cement_weight_set(float weight)
{
    g_exper.cement_weight = weight;
}

int exper_cement_weight_get(void)
{
    return g_exper.cement_weight;
}

void exper_test_cl_dosage_set(float dosage)
{
    g_exper.test_cl_dosage = dosage;
}

float exper_test_cl_dosage_get(void)
{
    return g_exper.test_cl_dosage;
}

static void _exper_oil_get(struct exper *exp)
{
    int correct = 0;
    
    exp->estat.stat = EXPER_STAT_UPDATE_PROGRESS;

    relay_ctrl(MOTOR_WATER_GET);
    while (exp->agno3_stock < EXPER_TOTAL_ML) {
        if (exp->emsg.stop) {
            EXPER_DBG_PRINT("stoped.\r\n");
            return;
        }

        if (stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML))
            exp->agno3_stock = EXPER_TOTAL_ML;
        else {
            exp->agno3_stock++;
            if (exp->agno3_stock >= EXPER_TOTAL_ML) {
                exp->agno3_stock = EXPER_TOTAL_ML - 1;
                correct++;
            }
        }

        if (correct > 20) {
            exp->estat.oil_stock = 0;
            exp->agno3_stock = 0;
            WM_BroadcastMessage(&exp->wmsg);
            exp->estat.stat = EXPER_STAT_ERR_MOTOR;
            WM_BroadcastMessage(&exp->wmsg);
            return;
        } else {
            exp->estat.oil_stock = exp->agno3_stock * 100 / EXPER_TOTAL_ML;
            WM_BroadcastMessage(&exp->wmsg);
        }
    }
}

static void exper_oil_get(struct exper *exp)
{
    _exper_oil_get(exp);
    exp->emsg.stop = 0;
    exp->wmsg.MsgId = WM_USER;
    exp->wmsg.hWinSrc = 0;
    exp->wmsg.Data.p = &exp->estat;
    exp->estat.stat = EXPER_STAT_OIL_GET_FINISHED;
    WM_BroadcastMessage(&exp->wmsg);
}

static void _exper_oil_put(struct exper *exp)
{
    int correct = 0;
    
    exp->estat.stat = EXPER_STAT_UPDATE_PROGRESS;

    EXPER_DBG_PRINT("oil put\r\n");
    relay_ctrl(MOTOR_WATER_PUT);
    while (exp->agno3_stock > 0) {
        if (exp->emsg.stop) {
            EXPER_DBG_PRINT("stoped.\r\n");
            return;
        }
            
        if (stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML))
            exp->agno3_stock = 0;
        else {
            exp->agno3_stock--;
            if (exp->agno3_stock <= 0) {
                exp->agno3_stock = 3;
                correct++;
            }
        }
        
        if (correct > 20) {
            exp->estat.oil_stock = 0;
            exp->agno3_stock = 0;
            WM_BroadcastMessage(&exp->wmsg);
            exp->estat.stat = EXPER_STAT_ERR_MOTOR;
            WM_BroadcastMessage(&exp->wmsg);
        } else {
            exp->estat.oil_stock = exp->agno3_stock * 100 / EXPER_TOTAL_ML;
            WM_BroadcastMessage(&exp->wmsg);
        }
    }
    EXPER_DBG_PRINT("stop while\r\n");
}

static void exper_oil_put(struct exper *exp)
{
    _exper_oil_put(exp);
    exp->emsg.stop = 0;
    exp->wmsg.MsgId = WM_USER;
    exp->wmsg.hWinSrc = 0;
    exp->wmsg.Data.p = &exp->estat;
    exp->estat.stat = EXPER_STAT_OIL_PUT_FINISHED;
    WM_BroadcastMessage(&exp->wmsg);
}

static void exper_oil_clear(struct exper *exp)
{
    int i;

    for (i = 0; i < 3; i++) {
        _exper_oil_get(exp);
        _exper_oil_put(exp);
    }

    exp->wmsg.MsgId = WM_USER;
    exp->wmsg.hWinSrc = 0;
    exp->wmsg.Data.p = &exp->estat;
    exp->estat.stat = EXPER_STAT_OIL_CLEAR_FINISHED;
    WM_BroadcastMessage(&exp->wmsg); 
}

static void float_swap(float *float1, float *float2)  
{  
    float temp;

    temp = *float1;
    *float1 = *float2;
    *float2 = temp;
}

static float volt_buff[20];

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

static void do_test(struct exper *exp, int mode)
{
    float volt_scale = 210.0; // 230mV to change step
    float step_ml = 0.3;
    int step = 3;
    float volt_diff = 0.0;
    float volt;
    int index = 0;
    int ext_delay = 0;

    exp->estat.graph_pos.x = 0;
    exp->count = 0;
    exp->volt_diff = 0.0;
    exp->estat.agno3_used = 0;
    exp->estat.graph_pos.x = 0;

    switch (mode) {
    case EXPER_MSG_AGNO3_START:
        exp->steps = 25;
        break;
    case EXPER_MSG_CL_START:
    case EXPER_MSG_BLOCK_START:
        exp->steps = 20;
        break;
    default:
        return;
    }

    while (1) {
        if (exp->estat.oil_stock < 10)
            _exper_oil_get(exp);
        /* user stop */
        if (exp->emsg.stop) {
            exp->emsg.stop = 0;
            return;
        }

        /* beyond scale, change to 0.1ml per step */
        if (volt > volt_scale) {
            step_ml = 0.1;
            step = 1;
        }

        /* put AgNo3 oil */
        relay_ctrl(MOTOR_WATER_PUT);
        if (stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * step)) {
            exp->estat.stat = EXPER_STAT_FAIL;
            WM_BroadcastMessage(&exp->wmsg);
            break;
        } else {
            exp->estat.graph_pos.x += step;
            exp->estat.agno3_used += step_ml;
            exp->agno3_stock -= step;           
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
            step, volt, exp->estat.agno3_used);
        
        /* do not care step 0.3 */
        if (step == 1) {
            exp->volt[exp->count] = volt;
            exp->agno3_used[exp->count] = exp->estat.agno3_used;
            exp->count++;
            exp->steps--;
            if (exp->count > 1) {
                volt_diff = (exp->volt[exp->count - 1]) - (exp->volt[exp->count - 2]);
                if (exp->volt_diff < volt_diff) {
                    exp->volt_diff = volt_diff;
                    exp->jump = exp->count - 2;
                }
                
                if (volt_diff > 5.0)
                    ext_delay = 5000;
                else
                    ext_delay = 0;
                EXPER_DBG_PRINT("derta = %f, gdiff = %f, jump = %d, ext_delay = %d\r\n",
                    volt_diff, exp->volt_diff, exp->jump, ext_delay);
            }
        }

        /* update UI */
        exp->estat.graph_pos.y = (int)volt;
        exp->estat.stat = EXPER_STAT_UPDATE_GRAPH;
        exp->estat.volt = volt;
        WM_BroadcastMessage(&exp->wmsg);

        exp->estat.oil_stock = exp->agno3_stock * 100 / EXPER_TOTAL_ML;
        exp->estat.stat = EXPER_STAT_UPDATE_PROGRESS;
        WM_BroadcastMessage(&exp->wmsg);

        if (exp->steps == 0) {
            switch (mode) {
            case EXPER_MSG_AGNO3_START:
                exp->test_agno3_used = count_agno3_used(exp);
                exp->test_agno3_dosage = exp->test_cl_dosage * 10 / exp->test_agno3_used;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\nAgNo3 test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", exp->test_agno3_used);
                EXPER_DBG_PRINT("AgNo3 nongdu is %.4f\r\n", exp->test_agno3_dosage);

                exp->estat.agno3_used = exp->test_agno3_used;
                exp->estat.agno3_consistence = exp->test_agno3_dosage;
                exp->estat.stat = EXPER_STAT_AGNO3_FINISHED;
                WM_BroadcastMessage(&exp->wmsg);
                return;
            case EXPER_MSG_BLOCK_START:
                exp->block_agno3_used = count_agno3_used(exp);

                EXPER_DBG_PRINT("\r\n\r\n\r\nblock test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", exp->block_agno3_used);

                exp->estat.agno3_used = exp->block_agno3_used;
                exp->estat.stat = EXPER_STAT_BLOCK_FINISHED;
                WM_BroadcastMessage(&exp->wmsg);
                return;
            case EXPER_MSG_CL_START:
                exp->cl_agno3_used = count_agno3_used(exp);
                exp->cl_percentage = (exp->test_agno3_dosage * (float)3.545 * (exp->cl_agno3_used - exp->block_agno3_used)) / exp->cement_weight;

                EXPER_DBG_PRINT("\r\n\r\n\r\ncl test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", exp->cl_agno3_used);
                EXPER_DBG_PRINT("cl percentage is %f%%\r\n", exp->cl_percentage);
                
                exp->estat.agno3_used = exp->cl_agno3_used;
                exp->estat.cl_percentage = exp->cl_percentage;
                exp->estat.stat = EXPER_STAT_CL_FINISHED;
                WM_BroadcastMessage(&exp->wmsg);

                for (index = 0; index < 12; index++)
                    exp->rep.data[index] = exp->volt[exp->jump - 5 + index] - exp->volt[exp->jump - 6 + index];
                exp->rep.data_num = 12;
                exp->rep.nitrate_dosage = exp->cl_agno3_used;
                exp->rep.percentage = exp->cl_percentage;
                exp->rep.year = 18;
                exp->rep.month = 12;
                exp->rep.day = 8;
                exp->rep.hour = 16;
                exp->rep.minute = 0;
                return;
            default:
                return;
            }
        }
    }
}

void exper_task(void *args)
{
    g_exper.wmsg.MsgId = WM_USER;
    g_exper.wmsg.hWinSrc = 0;
    g_exper.wmsg.Data.p = &g_exper.estat;
    exper_is_run = 0;

    while (1) {
        switch (g_exper.emsg.msg) {
        case EXPER_MSG_NONE:
            break;
        case EXPER_MSG_AGNO3_START:
            EXPER_DBG_PRINT("EXPER_MSG_AGNO3_START\r\n");
            exper_is_run = 1;
            do_test(&g_exper, EXPER_MSG_AGNO3_START);
            g_exper.emsg.msg = EXPER_MSG_NONE;
            exper_is_run = 0;
            break;
        case EXPER_MSG_BLOCK_START:
            EXPER_DBG_PRINT("EXPER_MSG_BLOCK_START\r\n");
            exper_is_run = 1;
            do_test(&g_exper, EXPER_MSG_BLOCK_START);
            g_exper.emsg.msg = EXPER_MSG_NONE;
            exper_is_run = 0;
            break;
        case EXPER_MSG_CL_START:
            EXPER_DBG_PRINT("EXPER_MSG_CL_START\r\n");
            exper_is_run = 1;
            do_test(&g_exper, EXPER_MSG_CL_START);
            g_exper.emsg.msg = EXPER_MSG_NONE;
            exper_is_run = 0;
            break;
        case EXPER_MSG_OIL_GET:
            exper_is_run = 1;
            exper_oil_get(&g_exper);
            g_exper.emsg.msg = EXPER_MSG_NONE;
            exper_is_run = 0;
            break;
        case EXPER_MSG_OIL_PUT:
            exper_is_run = 1;
            exper_oil_put(&g_exper);
            g_exper.emsg.msg = EXPER_MSG_NONE;
            exper_is_run = 0;
            break;
        case EXPER_MSG_OIL_CLEAR:
            exper_is_run = 1;
            exper_oil_clear(&g_exper);
            g_exper.emsg.msg = EXPER_MSG_NONE;
            exper_is_run = 0;
            break;
        default:
            break;
        }
    }
}

void exper_msg_set(struct exper_msg *msg)
{
    memcpy(&g_exper.emsg, msg, sizeof(struct exper_msg));
    printf("get msg.msg=%x, stop=%d\r\n", msg->msg, msg->stop);
}

void exper_init(void)
{
    g_exper.cement_weight = 5.0;
    g_exper.stand_agno3_dosage = 0.02;
    g_exper.test_cl_dosage = 0.02;
    g_exper.stand_v = 100;
}
