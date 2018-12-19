#include "touch.h"
#include "stm32_spi.h"
#include "stdio.h"
#include "GUI.h"
#include "stdlib.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

extern const GUI_FONT GUI_FontHZ_kaiti_20;
extern const GUI_FONT GUI_FontHZ_kaiti_20;

#define CMD_RDY 0X90
#define CMD_RDX	0XD0

#define FILTER_DISCARD    20
#define FILTER_WINDOW     5
#define FILTER_BUF_LEN    (FILTER_DISCARD * 2 + FILTER_WINDOW)

//#define TOUCH_DBG

#ifdef TOUCH_DBG
	#define TOUCH_DBG_PRINT(fmt, args...)    printf(fmt, ##args)
#else
	#define TOUCH_DBG_PRINT(fmt, args...)
#endif

#define TOUCH_X    (800)
#define TOUCH_Y    (480)
#define TOUCH_GET_Y(x)    (x * TOUCH_Y / TOUCH_X)
#define TOUCH_P1_X    (80)
#define TOUCH_P2_X    (720)
#define TOUCH_CENTRE_X    (TOUCH_X / 2)

struct phy {
	int32_t x;
	int32_t y;
};

struct pos {
	int32_t x;
	int32_t y;
};

struct point {
    struct pos pos;
    struct phy phy;
};

struct touch {
    struct phy buf[FILTER_BUF_LEN];
    struct phy cur_phy;

    int filter_len;
    int window;
    int discard;

	struct point point1;
	struct point point2;
	struct point point_centre;
	struct point point_adjust;
	struct pos pos_calc;
	
	double x_coe;
	double y_coe;
	int precision;
	struct pos correct;
};

static struct touch g_touch;

static void phy_swap(struct phy *p1, struct phy *p2)  
{  
    struct phy temp;

    temp.x = p1->x;
    temp.y = p1->y;
    p1->x = p2->x;
    p1->y = p2->y;
    p2->x = temp.x;
    p2->y = temp.y;
}

static void _do_filter(struct touch *ptouch)
{
	int i, j;

#if 1
    /* sort */
	for (i = 0; i < ptouch->filter_len; i++) {
		for (j = ptouch->filter_len - 1; j > i; j--) {
			if (ptouch->buf[j].x < ptouch->buf[j - 1].x)
				phy_swap(&ptouch->buf[j], &ptouch->buf[j - 1]);
        }
    }
#endif

    /* get avarage */
    ptouch->cur_phy.x = 0;
    ptouch->cur_phy.y = 0;
	for (i = ptouch->discard; i < (ptouch->discard + ptouch->window); i++) {
        ptouch->cur_phy.x += ptouch->buf[i].x;
        ptouch->cur_phy.y += ptouch->buf[i].y;
    }
	
    ptouch->cur_phy.x = ptouch->cur_phy.x / ptouch->window;
    ptouch->cur_phy.y = ptouch->cur_phy.y / ptouch->window;

#ifdef TOUCH_DBG
	for (i = 0; i < ptouch->filter_len; i++) {
		if (i == 0)	
			TOUCH_DBG_PRINT("\r\ndiscard:\r\n");
		else if (i == ptouch->discard)
			TOUCH_DBG_PRINT("\r\nwindows:\r\n");
		else if (i == (ptouch->discard + ptouch->window))
			TOUCH_DBG_PRINT("\r\ndiscard:\r\n");
		else
			;
		
		TOUCH_DBG_PRINT("(%d,%d) ", ptouch->buf[i].x, ptouch->buf[i].y);
	}
	TOUCH_DBG_PRINT("\r\nresult:\r\n");
	TOUCH_DBG_PRINT("x=%d,y=%d\r\n", ptouch->cur_phy.x, ptouch->cur_phy.y);
#endif
}

static int32_t _read_phy(uint8_t cmd)	  
{ 	 
	int32_t val = 0;
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
    spi_send_byte(SPI3, cmd, SPI_MODE_LIB); 
	val = spi_send_byte(SPI3, 0x00, SPI_MODE_LIB);
	val <<= 8;
	val |= spi_send_byte(SPI3, 0x00, SPI_MODE_LIB);
    val = (val >> 4) & 0xfff;
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
	return val;
}

void touch_test(void)
{
    struct phy p;

	while (1) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
            p.x = _read_phy(CMD_RDX);
            p.y = _read_phy(CMD_RDY);
            printf("TOUCH: xphy=%d, yphy=%d.\r\n", p.x, p.y);
			delay_ms(500);
        }
	}
}

static void touch_read_phy(void) 
{
	int i;

    for (i = 0; i < g_touch.filter_len; i++) {
        g_touch.buf[i].x = _read_phy(CMD_RDX);
        g_touch.buf[i].y = _read_phy(CMD_RDY);
    }
    _do_filter(&g_touch);
}

static void _touch_trans(struct touch *ptouch, struct phy *_phy)
{
	double temp;

	temp = (double)_phy->x / ptouch->x_coe;
	ptouch->pos_calc.x = TOUCH_X - (int32_t)temp;
	temp = (double)_phy->y / ptouch->y_coe;
	ptouch->pos_calc.y = (int32_t)temp;
}

static void touch_trans(GUI_PID_STATE *state, struct touch *ptouch)
{
	_touch_trans(ptouch, &ptouch->cur_phy);
	state->x = ptouch->pos_calc.x - ptouch->correct.x;
	state->y = ptouch->pos_calc.y - ptouch->correct.y;
	if (state->x > TOUCH_X || state->y > TOUCH_Y) {
		state->x = -1;
		state->y = -1;
		state->Pressed = 0;
	} else
		state->Pressed = 1;
	TOUCH_DBG_PRINT("get point(%d,%d), corrected(%d,%d)\r\n",
		ptouch->pos_calc.x, ptouch->pos_calc.y, state->x, state->y);
}

static void _disp_calibrate_fail(void)
{
	TOUCH_DBG_PRINT("calibrate fail, retry\r\n");
	GUI_DispStringHCenterAt("校准失败,自动重新开始校准流程", 400, 240);
	delay_ms(2000); // for Visual effects
}

void touch_calibrate(int force)
{
	struct touch *ptouch = &g_touch;
	struct sysconf *conf = sysconf_get();
	double _phy, _pos;
	int32_t _precision;
	GUI_PID_STATE state;

	/* to enable ADS7843 irq */
	_read_phy(CMD_RDX);
	_read_phy(CMD_RDY);

	if (sysconf_is_valid() && !force) {
		printf("get touch config, x_coe=%lf, y_coe=%lf, x_cor=%d, y_cor=%d\r\n",
			conf->x_coe, conf->y_coe, conf->x_correct, conf->y_correct);
		ptouch->x_coe = conf->x_coe;
		ptouch->y_coe = conf->y_coe;
		ptouch->correct.x = conf->x_correct;
		ptouch->correct.y = conf->y_correct;
		return;
	}

do_calc:
	TOUCH_DBG_PRINT("do touch calibrate...\r\n");
	GUI_SetBkColor(GUI_DARKGRAY);
	GUI_Clear();

	GUI_SetColor(GUI_BLUE);
	GUI_SetFont(&GUI_FontHZ_kaiti_20);
	GUI_DispStringHCenterAt("校准触摸屏", 400, 10);
	
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_FontHZ_kaiti_20);
	GUI_DispStringHCenterAt("请依次点击红色圆点", 400, 350);	
	GUI_SetColor(GUI_RED);
	/* draw p1 */
	GUI_FillCircle(ptouch->point1.pos.x, ptouch->point1.pos.y, 4);
	TOUCH_DBG_PRINT("draw point1(%d,%d), waitting for press.\r\n",
		ptouch->point1.pos.x, ptouch->point1.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	touch_read_phy();
	ptouch->point1.phy.x = ptouch->cur_phy.x;
	ptouch->point1.phy.y = ptouch->cur_phy.y;
	TOUCH_DBG_PRINT("get point1 adc value (%d,%d).\r\n",
		ptouch->point1.phy.x, ptouch->point1.phy.y);
	delay_ms(500); // for Visual effects

	/* draw p2 */
	GUI_SetColor(GUI_DARKGRAY);
	GUI_FillCircle(ptouch->point1.pos.x, ptouch->point1.pos.y, 4);
	GUI_SetColor(GUI_RED);
	GUI_FillCircle(ptouch->point2.pos.x, ptouch->point2.pos.y, 4);
	TOUCH_DBG_PRINT("draw point2(%d,%d), waitting for press.\r\n",
		ptouch->point2.pos.x, ptouch->point2.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	delay_ms(1);
	touch_read_phy();
	ptouch->point2.phy.x = ptouch->cur_phy.x;
	ptouch->point2.phy.y = ptouch->cur_phy.y;
	TOUCH_DBG_PRINT("get point2 adc value (%d,%d).\r\n",
		 ptouch->point2.phy.x, ptouch->point2.phy.y);
	delay_ms(500); // for Visual effects

	/* calc coe */
	TOUCH_DBG_PRINT("calculate coe\r\n");
	_phy = (double)(ptouch->point1.phy.x - ptouch->point2.phy.x);
	_pos = (double)(ptouch->point2.pos.x - ptouch->point1.pos.x);
	ptouch->x_coe = _phy / _pos;
	_phy = (double)(ptouch->point2.phy.y - ptouch->point1.phy.y);
	_pos = (double)(ptouch->point2.pos.y - ptouch->point1.pos.y);
	ptouch->y_coe = _phy / _pos;
	
	/* draw point_centre */
	GUI_SetColor(GUI_DARKGRAY);
	GUI_FillCircle(ptouch->point2.pos.x, ptouch->point2.pos.y, 4);
	GUI_SetColor(GUI_RED);
	GUI_FillCircle(ptouch->point_centre.pos.x, ptouch->point_centre.pos.y, 4);
	TOUCH_DBG_PRINT("draw point_centre(%d,%d), waitting for press.\r\n",
		ptouch->point_centre.pos.x, ptouch->point_centre.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	delay_ms(1);
	touch_read_phy();
	ptouch->point_centre.phy.x = ptouch->cur_phy.x;
	ptouch->point_centre.phy.y = ptouch->cur_phy.y;
	TOUCH_DBG_PRINT("get point_centre adc value (%d,%d).\r\n",
		 ptouch->point_centre.phy.x, ptouch->point_centre.phy.y);

	/* use point_centre to get correct value,
	 * for the adc value of touch screen does not start from 0
	 */
	_touch_trans(ptouch, &ptouch->point_centre.phy);
	TOUCH_DBG_PRINT("use point_centre to verify, get point(%d,%d).\r\n",
		ptouch->pos_calc.x, ptouch->pos_calc.y);
	ptouch->correct.x = ptouch->pos_calc.x - ptouch->point_centre.pos.x;
	ptouch->correct.y = ptouch->pos_calc.y - ptouch->point_centre.pos.y;
	delay_ms(500); // for Visual effects

	/* use a point to adjust */
	GUI_SetColor(GUI_DARKGRAY);
	GUI_FillCircle(ptouch->point_centre.pos.x, ptouch->point_centre.pos.y, 4);
	GUI_SetColor(GUI_RED);
	GUI_FillCircle(ptouch->point_adjust.pos.x, ptouch->point_adjust.pos.y, 4);
	TOUCH_DBG_PRINT("draw point_adjust(%d,%d), waitting for press.\r\n",
		ptouch->point_adjust.pos.x, ptouch->point_adjust.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	delay_ms(1);
	touch_read_phy();
	TOUCH_DBG_PRINT("get point_adjust adc value (%d,%d).\r\n",
		 ptouch->point_adjust.phy.x, ptouch->point_adjust.phy.y);
	touch_trans(&state, ptouch);
	
	delay_ms(500); // for Visual effects
	GUI_SetColor(GUI_DARKGRAY);
	GUI_FillCircle(ptouch->point_adjust.pos.x, ptouch->point_adjust.pos.y, 4);
	GUI_SetColor(GUI_RED);

	_precision = abs(state.x - ptouch->point_adjust.pos.x);
	if (ptouch->precision < _precision) {
		_disp_calibrate_fail();
		goto do_calc;
	}
	
	_precision = abs(state.y - ptouch->point_adjust.pos.y);
	if (ptouch->precision < _precision) {
		_disp_calibrate_fail();
		goto do_calc;
	}

	GUI_DispStringHCenterAt("校准成功", 400, 240);
	//GUI_Clear();
	TOUCH_DBG_PRINT("calibrate succeed, correct=(%d,%d)\r\n",
		ptouch->correct.x, ptouch->correct.y);

	conf->x_coe = ptouch->x_coe;
	conf->y_coe = ptouch->y_coe;
	conf->x_correct = ptouch->correct.x;
	conf->y_correct = ptouch->correct.y;
	printf("touch calc x_coe=%lf, y_coe=%lf, x_cor=%d, y_cor=%d\r\n",
		conf->x_coe, conf->y_coe, conf->x_correct, conf->y_correct);
	sysconf_save();
}

void touch_update(void)
{
	GUI_PID_STATE pstate;

	pstate.Layer = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
        pstate.Pressed = 0;
		pstate.x = 0;
		pstate.y = 0;
		GUI_PID_StoreState(&pstate);
		return;
    }
    
    touch_read_phy();
    touch_trans(&pstate, &g_touch);
	GUI_PID_StoreState(&pstate);
	/* if touch pressed, do not act within 100ms
	 * if not, gui event may exec twice.
	 * this taste bad, but it work
	 */
	vTaskDelay(150);
}

int touch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

	/* spi cs pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* irq pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    g_touch.filter_len = FILTER_BUF_LEN;
    g_touch.window = FILTER_WINDOW;
    g_touch.discard = FILTER_DISCARD;
	g_touch.point1.pos.x = TOUCH_P1_X;
	g_touch.point1.pos.y = TOUCH_GET_Y(TOUCH_P1_X);
	g_touch.point2.pos.x = TOUCH_P2_X;
	g_touch.point2.pos.y = TOUCH_GET_Y(TOUCH_P2_X);
	g_touch.point_centre.pos.x = TOUCH_CENTRE_X;
	g_touch.point_centre.pos.y = TOUCH_GET_Y(TOUCH_CENTRE_X);
	g_touch.point_adjust.pos.x = 650;
	g_touch.point_adjust.pos.y = 100;
	g_touch.precision = 12;
    return 0;
}

