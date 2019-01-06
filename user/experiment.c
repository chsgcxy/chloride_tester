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
#include "sysconf.h"
#include "data.h"

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

//#define EXPER_TEST

#define EXPER_CNT     2

struct exper_ctrl {
    struct exper_buf buf[50];
    float volt_diff;
    int count;
    int jump;
    int steps;
};

struct experiment {
    char *name;
    
    struct exper_data data;
    struct result_data res;

    struct exper_ctrl *ctrl;
    struct exper_stat *stat;
    struct exper_msg *msg;
    struct WM_MESSAGE *ui;
};

static struct experiment gexper[EXPER_CNT];
static struct experiment *cur_exper = NULL;

static int exper_agno3_stock = 0;
static float exper_stock_percentage = 0.0;

void exper_init(void)
{
    int i;
    
    static struct exper_stat stat;
    static struct exper_msg msg;
    static struct exper_ctrl ctrl;
    static struct WM_MESSAGE ui;
    
    gexper[0].name = "shuini";
    gexper[1].name = "qita";

    for (i = 0; i < EXPER_CNT; i++) {
        gexper[i].ctrl = &ctrl;
        gexper[i].stat = &stat;
        gexper[i].msg = &msg;
        gexper[i].ui = &ui;
        
        gexper[i].msg->stop = 0;
        gexper[i].msg->msg = EXPER_MSG_NONE;
        gexper[i].ui->Data.p = gexper[i].stat;    
        gexper[i].ui->MsgId = WM_USER;
        gexper[i].ui->hWinSrc = 0;

        gexper[i].data.sample_weight = 5.0;
        gexper[i].data.agno3_dosage = 0.02;
        gexper[i].data.nacl_dosage = 0.02;
        gexper[i].data.sample_volume = 100;
        gexper[i].data.agno3_stock = &exper_agno3_stock;
        gexper[i].data.stock_percentage = &exper_stock_percentage;
    }
}

static int is_run = 0;

int exper_busy_get(void)
{
    return is_run;
}

void exper_print_report(int idx)
{
    if (idx >= EXPER_CNT)
        return;
    
    report_show(&gexper[idx].res);
}
              
void exper_data_get(struct exper_data *data, int idx)
{
    struct exper_data *selfdata;
    if (idx >= EXPER_CNT)
        return;

    selfdata = &gexper[idx].data;
    memcpy(data, selfdata, sizeof(struct exper_data));
}

void exper_data_set(struct exper_data *data, int idx)
{
    struct exper_data *selfdata;
    if (idx >= EXPER_CNT)
        return;

    selfdata = &gexper[idx].data;
    memcpy(selfdata, data, sizeof(struct exper_data));
}

void exper_update_ui(struct experiment *exper)
{
    memcpy(&exper->stat->data, &exper->data, sizeof(struct exper_data));
    WM_BroadcastMessage(exper->ui);
}

static void _exper_oil_get(struct experiment *exper)
{
    int correct = 0;
    
    relay_ctrl(MOTOR_WATER_GET);
    while (1) {
        if (exper->msg->stop) {
            EXPER_DBG_PRINT("stoped.\r\n");
            break;
        }

        if (stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML)) {
            exper_agno3_stock = EXPER_TOTAL_ML;
            break;
        } else {
            exper_agno3_stock++;
            if (exper_agno3_stock >= EXPER_TOTAL_ML) {
                exper_agno3_stock = EXPER_TOTAL_ML - 1;
                correct++;
            }
        }

        if (correct > 20) {
            /* report error */
            exper_agno3_stock = 0;
            exper_stock_percentage = 0.0;
            exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
            exper_update_ui(exper);
            exper->stat->stat = EXPER_STAT_ERR_MOTOR;
            exper_update_ui(exper);
            return;
        } else {
            /* update progress */
            exper_stock_percentage = exper_agno3_stock * 100 / EXPER_TOTAL_ML;
            exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
            exper_update_ui(exper);
        }
    }

    exper_stock_percentage = exper_agno3_stock * 100 / EXPER_TOTAL_ML;
    exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
    exper_update_ui(exper);
}

static void exper_oil_get(struct experiment *exper)
{   
    _exper_oil_get(exper);
    exper->stat->stat = EXPER_STAT_OIL_GET_FINISHED;
    exper_update_ui(exper);
}

static void _exper_oil_put(struct experiment *exper, int delay)
{
    int correct = 0;
    
    if (exper_agno3_stock == 0)
        return;

    exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;

    EXPER_DBG_PRINT("oil put\r\n");
    relay_ctrl(MOTOR_WATER_PUT);
    while (1) {
        if (exper->msg->stop) {
            EXPER_DBG_PRINT("stoped.\r\n");
            break;
        }
            
        if (stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML)) {
            exper_agno3_stock = 0;
            break;
        } else {
            exper_agno3_stock--;
            if (exper_agno3_stock <= 0) {
                exper_agno3_stock = 1;
                correct++;
            }
        }

        if (delay)
            vTaskDelay(delay);
        
        if (correct > 20) {
            exper_agno3_stock = 0;
            exper_stock_percentage = 0.0;
            exper_update_ui(exper);
            exper->stat->stat = EXPER_STAT_ERR_MOTOR;
            exper_update_ui(exper);
            return;
        } else {
            exper_stock_percentage = exper_agno3_stock * 100 / EXPER_TOTAL_ML;
            exper_update_ui(exper);
        }
    }

    exper_stock_percentage = exper_agno3_stock * 100 / EXPER_TOTAL_ML;
    exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
    exper_update_ui(exper);
}

static void exper_oil_put(struct experiment *exper)
{
    _exper_oil_put(exper, 500);
    exper->stat->stat = EXPER_STAT_OIL_PUT_FINISHED;
    exper_update_ui(exper);
}

static void exper_oil_clear(struct experiment *exper)
{
    int i;

    for (i = 0; i < 3; i++) {
        _exper_oil_get(exper);
        _exper_oil_put(exper, 0);
    }

    exper->stat->stat = EXPER_STAT_OIL_CLEAR_FINISHED;
    exper_update_ui(exper); 
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
    static float volt_buff[20];

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

    return volt;
}

float exper_volt_get(void)
{
    return ad7705_read();
}

static float count_agno3_used(struct experiment *exper)
{
    float delta_pre, delta_delta_pre, delta_after, delta_delta_after, delta_cur;
    float res;
    struct exper_ctrl *ctrl = exper->ctrl;
    struct exper_buf *buf = ctrl->buf;

    EXPER_DBG_PRINT("count AgNO3 used\r\n");
    delta_pre = buf[ctrl->jump].volt - buf[ctrl->jump - 1].volt;
    delta_cur = buf[ctrl->jump + 1].volt - buf[ctrl->jump].volt;
    delta_after = buf[ctrl->jump + 2].volt - buf[ctrl->jump + 1].volt;
    delta_delta_pre = delta_cur - delta_pre;
    delta_delta_after = delta_after - delta_cur;
    EXPER_DBG_PRINT("exper->jump = %d\r\n", ctrl->jump);
    EXPER_DBG_PRINT("delta_pre = %f, delta_cur = %f, delta_after = %f\r\n",
        delta_pre, delta_cur, delta_after);
    EXPER_DBG_PRINT("delta_delta_pre = %f, delta_delta_after = %f\r\n",
        delta_delta_pre, delta_delta_after);
    res = buf[ctrl->jump].agno3_used + (delta_delta_pre / (delta_delta_pre - delta_delta_after)) * 0.1;
    EXPER_DBG_PRINT("actual AgNO3 used = %f\r\n", res);
    return res;
}

static void result_data_creat(struct experiment *exper, int type)
{
    int i = 0;
    int pre_idx = 5;
    int data_cnt = 9;
    struct result_data *res = &exper->res;
    struct data_item *items = res->items;
    struct exper_ctrl *ctrl = exper->ctrl;
    struct exper_buf *buf = ctrl->buf;
    
    if (ctrl->jump < pre_idx)
        pre_idx = ctrl->jump;

    if (ctrl->count < data_cnt)
        data_cnt = ctrl->count;

    for (i = 0; i < data_cnt; i++) {
        items[i].volt = buf[ctrl->jump - pre_idx + i].volt;
        items[i].agno3_used = buf[ctrl->jump - pre_idx + i].agno3_used;
        if (i > 0)
            items[i].delta_v = items[i].volt - items[i - 1].volt;
        if (i > 1)
            items[i].delta2_v = items[i].delta_v - items[i - 1].delta_v;
    }
    
    res->type = type;
    res->items_cnt = data_cnt;
    res->cl_agno3_used = exper->data.cl_agno3_used;
    res->cl_percentage = exper->data.cl_percentage;
    res->cl_dosage = exper->data.cl_dosage;
    res->ppm = exper->data.ppm;
    res->block_agno3_used = exper->data.block_agno3_used;
    res->agno3_dosage = exper->data.agno3_dosage;

    res->year = 18;
    res->month = 12;
    res->day = 8;
    res->hour = 16;
    res->minute = 0;
}

static void result_data_save(struct experiment *exper)
{
    data_save(&exper->res);
}

static void do_test(struct experiment *exper, int mode)
{
    float volt_scale;
    float step_ml = 0.3;
    int step = 3;
    float volt_diff = 0.0;
    float volt;
    int ext_delay = 0;
    struct exper_ctrl *ctrl = exper->ctrl;
    struct exper_data *data = &exper->data;
    struct exper_stat *stat = exper->stat;
    struct exper_buf *buf = ctrl->buf;
    struct sysconf *cfg = sysconf_get();

    char jump_start_flag = 0;
    char jump_stop_flag = 0;
    float volt_line = 5.0;
    int step1_cnt = 3;

    ctrl->count = 0;
    ctrl->volt_diff = 0.0;
    data->agno3_used = 0.0;

    volt_scale = cfg->volt_scale;
    EXPER_DBG_PRINT("start experiment, volt_scale = %f\r\n",
        volt_scale);

    while (1) {
        if (*data->stock_percentage < 10)
            _exper_oil_get(exper);
        /* user stop */
        if (exper->msg->stop) {
            exper->msg->stop = 0;
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
            stat->stat = EXPER_STAT_FAIL;
            exper_update_ui(exper);
            break;
        } else {
            data->agno3_used += step_ml;
            *data->agno3_stock -= step;
            *data->stock_percentage = *data->agno3_stock * 100 / EXPER_TOTAL_ML;
        }
        stat->stat = EXPER_STAT_UPDATE_AGNO3_USED;
        exper_update_ui(exper);
        
        /* wait oil act */
        EXPER_DBG_PRINT("\r\n\r\n");
        if (step == 1) {
            vTaskDelay(10000 + ext_delay);
            volt = exper_filter();
        } else {
            vTaskDelay(3000);
            volt = exper_filter();
        }
        data->volt = volt;
        EXPER_DBG_PRINT("step = %d, volt = %f, AgNo3 used %.3fmL\r\n",
            step, volt, data->agno3_used);
        
        /* do not care step 0.3 */
        if (step == 1) {
            buf[ctrl->count].volt = volt;
            buf[ctrl->count].agno3_used = data->agno3_used;
            ctrl->count++;
            if (ctrl->count > 1) {
                volt_diff = (buf[ctrl->count - 1].volt) - (buf[ctrl->count - 2].volt);
                if (ctrl->volt_diff < volt_diff) {
                    ctrl->volt_diff = volt_diff;
                    ctrl->jump = ctrl->count - 2;
                }
                
                if (volt_diff > volt_line)
                    ext_delay = 5000;
                else
                    ext_delay = 0;

                /* adjust experiment finished */
                if (volt_diff > volt_line && jump_start_flag == 0)
                    jump_start_flag = 1;
                if (volt_diff < volt_line && jump_start_flag == 1 && jump_stop_flag == 0)
                    jump_stop_flag = 1;
                if (jump_stop_flag)
                    step1_cnt--;

                EXPER_DBG_PRINT("derta = %f, gdiff = %f, jump = %d, ext_delay = %d, ext_step = %d\r\n",
                    volt_diff, ctrl->volt_diff, ctrl->jump, ext_delay, step1_cnt);
            }
        }

        /* update UI */
        stat->stat = EXPER_STAT_UPDATE_GRAPH;
        exper_update_ui(exper);
        stat->stat = EXPER_STAT_UPDATE_PROGRESS;
        exper_update_ui(exper);

        /* experiment finished */
        if (step1_cnt == 0) {
            switch (mode) {
            case EXPER_MSG_AGNO3_START:
                data->agno3_agno3_used = count_agno3_used(exper);
                data->agno3_dosage = data->nacl_dosage * 10 / data->agno3_agno3_used;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\nAgNo3 test finished.\r\n");
                EXPER_DBG_PRINT("ctrl->count = %d, ctrl->jump = %d.\r\n",
                    ctrl->count, ctrl->jump);
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", data->agno3_agno3_used);
                EXPER_DBG_PRINT("AgNo3 nongdu is %.4f\r\n", data->agno3_dosage);

                stat->stat = EXPER_STAT_AGNO3_FINISHED;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_BLOCK_START:
                data->block_agno3_used = count_agno3_used(exper);

                EXPER_DBG_PRINT("\r\n\r\n\r\nblock test finished.\r\n");
                EXPER_DBG_PRINT("ctrl->count = %d, ctrl->jump = %d.\r\n",
                    ctrl->count, ctrl->jump);
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", data->block_agno3_used);

                stat->stat = EXPER_STAT_BLOCK_FINISHED;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_CL_START:
                data->cl_agno3_used = count_agno3_used(exper);
                data->cl_percentage = (data->agno3_dosage * (float)3.545 * (data->cl_agno3_used - data->block_agno3_used)) / data->sample_weight;

                EXPER_DBG_PRINT("\r\n\r\n\r\ncl test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", data->cl_agno3_used);
                EXPER_DBG_PRINT("cl percentage is %f%%\r\n", data->cl_percentage);
                
                result_data_creat(exper, DATA_TYPE_CL);
                result_data_save(exper);
                stat->stat = EXPER_STAT_CL_FINISHED;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_STAND_START:
                data->cl_agno3_used = count_agno3_used(exper);
                data->cl_dosage = (data->agno3_dosage * (data->cl_agno3_used - data->block_agno3_used)) / data->sample_volume;
                data->ppm = data->cl_dosage * (float)35450;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\ncl test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %f\r\n", data->cl_agno3_used);
                EXPER_DBG_PRINT("cl dosage is %f\r\n", data->cl_dosage);
                EXPER_DBG_PRINT("ppm %f\r\n", data->ppm);

                result_data_creat(exper, DATA_TYPE_STAND);
                result_data_save(exper);
                stat->stat = EXPER_STAT_STAND_FINISHED;
                exper_update_ui(exper);                
                return;
            default:
                return;
            }
        }
    }
}

void exper_task(void *args)
{
    while (1) {
        switch (cur_exper->msg->msg) {
        case EXPER_MSG_NONE:
            break;
        case EXPER_MSG_AGNO3_START:
            EXPER_DBG_PRINT("EXPER_MSG_AGNO3_START\r\n");     
#ifdef EXPER_TEST
            cur_exper->data.agno3_agno3_used = 10.1;
            cur_exper->data.agno3_used = 11.3;
            cur_exper->data.agno3_dosage = 0.0312;
            cur_exper->stat->stat = EXPER_STAT_AGNO3_FINISHED;
            exper_update_ui(cur_exper);
#else        
            do_test(cur_exper, EXPER_MSG_AGNO3_START);
#endif            
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_BLOCK_START:
            EXPER_DBG_PRINT("EXPER_MSG_BLOCK_START\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_BLOCK_FINISHED;
            exper_update_ui(cur_exper);
#else            
            do_test(cur_exper, EXPER_MSG_BLOCK_START);
#endif            
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_CL_START:
            EXPER_DBG_PRINT("EXPER_MSG_CL_START\r\n");
#ifdef EXPER_TEST
            result_data_creat(cur_exper, REP_TYPE_CL);
            cur_exper->stat->stat = EXPER_STAT_CL_FINISHED;
            exper_update_ui(cur_exper);
#else             
            do_test(cur_exper, EXPER_MSG_CL_START);
#endif            
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_STAND_START:
            EXPER_DBG_PRINT("EXPER_MSG_STAND_START\r\n");
#ifdef EXPER_TEST
            result_data_creat(cur_exper, REP_TYPE_STAND);
            cur_exper->stat->stat = EXPER_STAT_STAND_FINISHED;
            exper_update_ui(cur_exper);
#else              
            do_test(cur_exper, EXPER_MSG_STAND_START);
#endif            
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_OIL_GET:
            exper_oil_get(cur_exper);
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_OIL_PUT:
            exper_oil_put(cur_exper);
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_OIL_CLEAR:
            exper_oil_clear(cur_exper);
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        default:
            break;
        }
    }
}

void exper_msg_set(struct exper_msg *msg, int idx)
{
    if (idx >= EXPER_CNT)
        return;    
    
    cur_exper = &(gexper[idx]);
    printf("exper %s get msg.msg=%x, stop=%d\r\n",
        cur_exper->name, msg->msg, msg->stop);
    memcpy(cur_exper->msg, msg, sizeof(struct exper_msg));

}


