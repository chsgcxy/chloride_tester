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

enum exper_sm{
    STATUS_EXPER_START = 0x00,
    STATUS_PRE_STEP01ML,
    STATUS_START_STEP01ML,
    STATUS_IN_STEP01ML,
    STATUS_PREJUMP,
    STATUS_READJUMP,
    STATUS_JUMPED,
    STATUS_FINISHED,
    STATUS_EXPER_STOP,
};


uint32_t zsb_total_step = ZSB_LEN_DEFAULT;

#define EXPER_DISCARD         (5)
#define EXPER_WINDOWS         (10)
#define EXPER_BUF_CNT         (EXPER_DISCARD * 2 + EXPER_WINDOWS)

#define EXPER_DBG

#ifdef EXPER_DBG
	#define EXPER_DBG_PRINT(fmt, args...)    printf(fmt, ##args)
#else
	#define EXPER_DBG_PRINT(fmt, args...)
#endif

#define EXPER_ALGO_NEW

//#define EXPER_TEST

#define EXPER_CNT     4
#define MOTO_ERROR_SCAL         10
#define BUF_CNT            2000

struct exper_ctrl {
    struct exper_buf buf[2000];
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
    struct sysconf *cfg;

    static struct exper_stat stat;
    static struct exper_msg msg;
    static struct exper_ctrl ctrl;
    static struct WM_MESSAGE ui;
    
    gexper[0].name = "shuini";
    gexper[1].name = "qita";
    gexper[2].name = "dropper";
    gexper[3].name = "extest";

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

    /* default cl- dosage when extest */
    gexper[3].data.nacl_dosage = 0.1;

    cfg = sysconf_get();
    if (cfg->zsb_valid != ZSB_VALID_FLAG) {
        EXPER_DBG_PRINT("zsb invalid, use default.\r\n");
        zsb_total_step = ZSB_LEN_DEFAULT;
        cfg->zsb_len = ZSB_LEN_DEFAULT;
        cfg->zsb_valid = ZSB_VALID_FLAG;
        sysconf_save();
    } else {
        zsb_total_step = cfg->zsb_len;
    }
    EXPER_DBG_PRINT("zsb total step = %d\r\n", zsb_total_step);
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

static int _exper_oil_get(struct experiment *exper)
{
    int correct = 0;
    
    relay_ctrl(MOTOR_WATER_GET);
    while (1) {
        if (exper->msg->stop) {
            EXPER_DBG_PRINT("stoped.\r\n");
            break;
        }

        if (stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML)) {
            exper_agno3_stock = zsb_total_step;
            break;
        } else {
            exper_agno3_stock++;
            if (exper_agno3_stock >= zsb_total_step) {
                exper_agno3_stock = zsb_total_step - 1;
                correct++;
            }
        }

        if (correct > MOTO_ERROR_SCAL) {
            /* report error */
            exper_agno3_stock = 0;
            exper_stock_percentage = 0.0;
            exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
            exper_update_ui(exper);
            exper->stat->stat = EXPER_STAT_ERR_MOTOR;
            exper_update_ui(exper);
            return 1;
        } else {
            /* update progress */
            exper_stock_percentage = exper_agno3_stock * 100 / zsb_total_step;
            exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
            exper_update_ui(exper);           
        }
    }

    exper_stock_percentage = exper_agno3_stock * 100 / zsb_total_step;
    exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
    exper_update_ui(exper);
    return 0;
}

static void exper_oil_get(struct experiment *exper)
{   
    _exper_oil_get(exper);
    exper->stat->stat = EXPER_STAT_OIL_GET_FINISHED;
    exper_update_ui(exper);
}

static int _exper_oil_put(struct experiment *exper, int delay)
{
    int correct = 0;
    
    if (exper_agno3_stock == 0)
        return 0;

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
        
        if (correct > MOTO_ERROR_SCAL) {
            exper_agno3_stock = 0;
            exper_stock_percentage = 0.0;
            exper_update_ui(exper);
            exper->stat->stat = EXPER_STAT_ERR_MOTOR;
            exper_update_ui(exper);
            return 1;
        } else {
            exper_stock_percentage = exper_agno3_stock * 100 / zsb_total_step;
            exper_update_ui(exper);
        }
    }

    exper_stock_percentage = exper_agno3_stock * 100 / zsb_total_step;
    exper->stat->stat = EXPER_STAT_UPDATE_PROGRESS;
    exper_update_ui(exper);
    return 0;
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

    if (_exper_oil_put(exper, 0))
        goto finished;

    for (i = 0; i < 3; i++) {
        if (_exper_oil_get(exper))
            goto finished;
        if (_exper_oil_put(exper, 0))
            goto finished;
    }
    
finished:
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

float exper_filter(void)
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
    int pre_idx = 3;
    int data_cnt = 9;
    struct result_data *res = &exper->res;
    struct data_item *items = res->items;
    struct exper_ctrl *ctrl = exper->ctrl;
    struct exper_buf *buf = ctrl->buf;

    RTC_TimeTypeDef  RTC_TimeStructure;
    RTC_DateTypeDef  RTC_DateStructure;
    
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

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

    res->year = RTC_DateStructure.RTC_Year;
    res->month = RTC_DateStructure.RTC_Month;
    res->day = RTC_DateStructure.RTC_Date;
    res->hour = RTC_TimeStructure.RTC_Hours;
    res->minute = RTC_TimeStructure.RTC_Minutes;
}

static void result_data_save(struct experiment *exper)
{
    data_save(&exper->res);
}

static void do_test(struct experiment *exper, int mode)
{
    struct exper_ctrl *ctrl = exper->ctrl;
    struct exper_data *data = &exper->data;
    struct exper_stat *stat = exper->stat;
    struct exper_buf *buf = ctrl->buf;

    ctrl->count = 0;
    ctrl->volt_diff = 0.0;
    data->agno3_used = 0.0;

    uint32_t exper_sm = STATUS_EXPER_START;
    uint32_t stop_sm = STATUS_PREJUMP;

    float step_ml = 0.3;
    int step = 3;
    float volt_diff = 0.0;
    float volt;
    float prevolt = 0.0;
    TickType_t msdelay = 3000;
    float volt_line = 5.0;
    float v;
        
    while (1) {
        if (exper_agno3_stock < 10) {
            if (_exper_oil_get(exper))
                exper_sm = STATUS_EXPER_STOP;
            else {
                relay_ctrl(MOTOR_WATER_PUT);
                stepmotor_run(MOTOR_DIR_UP, 347); // STEP_01ML * 1.8
            }
        }
            
        if (exper->msg->stop) {
            exper->msg->stop = 0;
            return;
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
            *data->stock_percentage = *data->agno3_stock * 100 / zsb_total_step;
        }
        stat->stat = EXPER_STAT_UPDATE_AGNO3_USED;
        exper_update_ui(exper);
        
        /* wait oil act */
        EXPER_DBG_PRINT("\r\n\r\n");
        vTaskDelay(msdelay);
        volt = exper_filter();
        data->volt = volt;
        EXPER_DBG_PRINT("step = %d, volt = %f, AgNo3 used %.3fmL\r\n",
            step, volt, data->agno3_used);

        switch (exper_sm) {
        case STATUS_EXPER_START:
            prevolt = volt;
            exper_sm = STATUS_PRE_STEP01ML;         
            break;
        case STATUS_PRE_STEP01ML:
            volt_diff = volt - prevolt;
            prevolt = volt;
            if (volt_diff > 8.0) {
                exper_sm = STATUS_START_STEP01ML;
                msdelay = 10000;
                step = 1;
                step_ml = 0.1;
            }
            break;
        case STATUS_START_STEP01ML:
            buf[ctrl->count].volt = volt;
            buf[ctrl->count].agno3_used = data->agno3_used;
            ctrl->count++;
            exper_sm = STATUS_IN_STEP01ML;
            break;        
        case STATUS_IN_STEP01ML:
            buf[ctrl->count].volt = volt;
            buf[ctrl->count].agno3_used = data->agno3_used;
            volt_diff = (buf[ctrl->count].volt) - (buf[ctrl->count - 1].volt);
            if (ctrl->volt_diff < volt_diff) {
                ctrl->volt_diff = volt_diff;
                ctrl->jump = ctrl->count - 1;
            }
            ctrl->count++;
            EXPER_DBG_PRINT("volt diff = %f\r\n", volt_diff);
            
            if (volt_diff > volt_line)
                msdelay = 15000;
            else
                msdelay = 10000;

            /* can not beyond buf len */
            if (ctrl->count >= BUF_CNT - 3)
                stop_sm = STATUS_FINISHED;

            switch (stop_sm) {
            case STATUS_PREJUMP:
                if (volt_diff > volt_line)
                    stop_sm = STATUS_READJUMP;
                break;
            case STATUS_READJUMP:
                if (volt_diff > volt_line)
                    stop_sm = STATUS_JUMPED;
                else 
                    stop_sm = STATUS_PREJUMP;
                break;
            case STATUS_JUMPED:
                if (volt_diff < volt_line)
                    stop_sm = STATUS_FINISHED;
                break;
            case STATUS_FINISHED:
                exper_sm = STATUS_EXPER_STOP;
                break;
            default:
                break;                
            }
            break;
        case STATUS_EXPER_STOP:
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
            case EXPER_MSG_DROPPER_START:
                data->cl_agno3_used = count_agno3_used(exper);
                data->cl_agno3_used2 = data->agno3_used;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\ncl test finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %f\r\n", data->cl_agno3_used);

                result_data_creat(exper, DATA_TYPE_DORRPER);
                stat->stat = EXPER_STAT_DROPPER_FINISHED;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_AGNO3_EXTEST_START1:
                data->agno3_agno3_used = count_agno3_used(exper);
                data->agno3_agno3_used2 = data->agno3_used;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\nEXPER_MSG_AGNO3_EXTEST_START1 finished.\r\n");
                EXPER_DBG_PRINT("ctrl->count = %d, ctrl->jump = %d.\r\n",
                    ctrl->count, ctrl->jump);
                EXPER_DBG_PRINT("AgNo3 used actual %.3f\r\n", data->agno3_agno3_used);
                EXPER_DBG_PRINT("AgNo3 total used %.3f\r\n", data->agno3_agno3_used2);

                stat->stat = EXPER_STAT_AGNO3_EXTEST_FINISHED1;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_AGNO3_EXTEST_START2:
                data->agno3_agno3_used2 += count_agno3_used(exper);
                data->agno3_dosage = data->nacl_dosage * 10 / (data->agno3_agno3_used2 - data->agno3_agno3_used);
                
                EXPER_DBG_PRINT("\r\n\r\n\r\nEXPER_MSG_AGNO3_EXTEST_START2 finished.\r\n");
                EXPER_DBG_PRINT("ctrl->count = %d, ctrl->jump = %d.\r\n",
                    ctrl->count, ctrl->jump);
                EXPER_DBG_PRINT("AgNo3 used step1 %.3f\r\n", data->agno3_agno3_used);
                EXPER_DBG_PRINT("AgNo3 used step2 %.3f\r\n", data->agno3_agno3_used2);
                EXPER_DBG_PRINT("AgNo3 nongdu is %.4f\r\n", data->agno3_dosage);
                /* agno3 test stage2 finished in extest
                 * is same with agno3 finished
                 */
                stat->stat = EXPER_STAT_AGNO3_FINISHED;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_BLOCK_EXTEST_START1:
                data->block_agno3_used = count_agno3_used(exper);
                data->block_agno3_used2 = data->agno3_used;

                EXPER_DBG_PRINT("\r\n\r\n\r\nEXPER_MSG_BLOCK_EXTEST_START1 finished.\r\n");
                EXPER_DBG_PRINT("ctrl->count = %d, ctrl->jump = %d.\r\n",
                    ctrl->count, ctrl->jump);
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", data->block_agno3_used);
                EXPER_DBG_PRINT("AgNo3 total used %.3f\r\n", data->block_agno3_used2);

                stat->stat = EXPER_STAT_BLOCK_EXTEST_FINISHED1;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_BLOCK_EXTEST_START2:
                data->block_agno3_used2 += count_agno3_used(exper);

                EXPER_DBG_PRINT("\r\n\r\n\r\nEXPER_MSG_BLOCK_EXTEST_START2 finished.\r\n");
                EXPER_DBG_PRINT("ctrl->count = %d, ctrl->jump = %d.\r\n",
                    ctrl->count, ctrl->jump);
                EXPER_DBG_PRINT("AgNo3 used step1 %.3f\r\n", data->block_agno3_used);    
                EXPER_DBG_PRINT("AgNo3 used step2 %.3f\r\n", data->block_agno3_used2);

                stat->stat = EXPER_STAT_BLOCK_FINISHED;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_CL_EXTEST_START1:
                data->cl_agno3_used = count_agno3_used(exper);
                data->cl_agno3_used2 = data->agno3_used;

                EXPER_DBG_PRINT("\r\n\r\n\r\nEXPER_MSG_CL_EXTEST_START1 finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used actual is %.3f\r\n", data->cl_agno3_used);
                EXPER_DBG_PRINT("AgNo3 total used %.3f\r\n", data->cl_agno3_used2);
                
                stat->stat = EXPER_STAT_CL_EXTEST_FINISHED1;
                exper_update_ui(exper);
                return;
            case EXPER_MSG_CL_EXTEST_START2:
                data->cl_agno3_used2 += count_agno3_used(exper);
                v = ((data->cl_agno3_used - data->block_agno3_used) + (data->cl_agno3_used2 - data->block_agno3_used2)) / 2.0;
                data->cl_percentage = (data->agno3_dosage * (float)3.545 * v) / data->sample_weight;
                
                EXPER_DBG_PRINT("\r\n\r\n\r\nEXPER_MSG_CL_EXTEST_START2 finished.\r\n");
                EXPER_DBG_PRINT("AgNo3 used step1 %.3f\r\n", data->cl_agno3_used);
                EXPER_DBG_PRINT("AgNo3 used step2 %.3f\r\n", data->cl_agno3_used2);
                EXPER_DBG_PRINT("cl percentage is %f%%\r\n", data->cl_percentage);
                
                result_data_creat(exper, DATA_TYPE_CL);
                result_data_save(exper);
                stat->stat = EXPER_STAT_CL_FINISHED;
                exper_update_ui(exper);
                return;
            default:
                return;
            }
        default:
            break;
        }

        EXPER_DBG_PRINT("exper sm = 0x%x, finished sm = 0x%x\r\n",
            exper_sm, stop_sm);

        stat->stat = EXPER_STAT_UPDATE_GRAPH;
        exper_update_ui(exper);
        stat->stat = EXPER_STAT_UPDATE_PROGRESS;
        exper_update_ui(exper);
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
        case EXPER_MSG_DROPPER_START:
             EXPER_DBG_PRINT("EXPER_MSG_DROPPER_START\r\n");     
#ifdef EXPER_TEST
            cur_exper->data.agno3_agno3_used = 10.1;
            cur_exper->data.agno3_used = 11.3;
            cur_exper->data.agno3_dosage = 0.0312;
            cur_exper->stat->stat = EXPER_STAT_DROPPER_FINISHED;
            exper_update_ui(cur_exper);
#else        
            do_test(cur_exper, EXPER_MSG_DROPPER_START);
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
            result_data_creat(cur_exper, DATA_TYPE_CL);
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
            result_data_creat(cur_exper, DATA_TYPE_STAND);
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

        /* add for extest..... */
        case EXPER_MSG_AGNO3_EXTEST_START1:
            EXPER_DBG_PRINT("EXPER_MSG_AGNO3_EXTEST_START1\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_AGNO3_EXTEST_FINISHED1;
            exper_update_ui(cur_exper);
#else
            do_test(cur_exper, EXPER_MSG_AGNO3_EXTEST_START1);
#endif
            vTaskSuspendAll();
            if (cur_exper->msg->msg != EXPER_MSG_AGNO3_EXTEST_START2)
                cur_exper->msg->msg = EXPER_MSG_NONE;
            xTaskResumeAll();
            break;
        case EXPER_MSG_AGNO3_EXTEST_START2:
            EXPER_DBG_PRINT("EXPER_MSG_AGNO3_EXTEST_START2\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_AGNO3_FINISHED;
            exper_update_ui(cur_exper);
#else
            do_test(cur_exper, EXPER_MSG_AGNO3_EXTEST_START2);
#endif            
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_BLOCK_EXTEST_START1:
            EXPER_DBG_PRINT("EXPER_MSG_BLOCK_EXTEST_START1\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_BLOCK_EXTEST_FINISHED1;
            exper_update_ui(cur_exper);
#else            
            do_test(cur_exper, EXPER_MSG_BLOCK_EXTEST_START1);
#endif     
            vTaskSuspendAll();
            if (cur_exper->msg->msg != EXPER_MSG_BLOCK_EXTEST_START2)
                cur_exper->msg->msg = EXPER_MSG_NONE;
            xTaskResumeAll();
            break;
        case EXPER_MSG_BLOCK_EXTEST_START2:
            EXPER_DBG_PRINT("EXPER_MSG_BLOCK_EXTEST_START2\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_BLOCK_FINISHED;
            exper_update_ui(cur_exper);
#else                       
            do_test(cur_exper, EXPER_MSG_BLOCK_EXTEST_START2);
#endif
            cur_exper->msg->msg = EXPER_MSG_NONE;
            break;
        case EXPER_MSG_CL_EXTEST_START1:
            EXPER_DBG_PRINT("EXPER_MSG_CL_EXTEST_START1\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_CL_EXTEST_FINISHED1;
            exper_update_ui(cur_exper);
#else
            do_test(cur_exper, EXPER_MSG_CL_EXTEST_START1);
#endif
            vTaskSuspendAll();
            if (cur_exper->msg->msg != EXPER_MSG_CL_EXTEST_START2)
                cur_exper->msg->msg = EXPER_MSG_NONE;
            xTaskResumeAll();
            break;
        case EXPER_MSG_CL_EXTEST_START2:
            EXPER_DBG_PRINT("EXPER_MSG_CL_EXTEST_START2\r\n");
#ifdef EXPER_TEST
            cur_exper->stat->stat = EXPER_STAT_CL_FINISHED;
            exper_update_ui(cur_exper);
#else
            do_test(cur_exper, EXPER_MSG_CL_EXTEST_START2);
#endif
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
    EXPER_DBG_PRINT("exper %s get msg.msg=%x, stop=%d\r\n",
        cur_exper->name, msg->msg, msg->stop);
    memcpy(cur_exper->msg, msg, sizeof(struct exper_msg));

}


