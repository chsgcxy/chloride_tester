#include "touch.h"
#include "stm32_spi.h"
#include "stdio.h"
#include "GUI.h"
#include "stdlib.h"

#define CMD_RDY 0X90
#define CMD_RDX	0XD0

#define FILTER_DISCARD    5
#define FILTER_WINDOW     10
#define FILTER_BUF_LEN    (FILTER_DISCARD * 2 + FILTER_WINDOW)

#define TOUCH_DBG

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

extern void delay_ms(int ms); 

struct phy {
	uint16_t x;
	uint16_t y;
};

struct pos {
	uint16_t x;
	uint16_t y;
};

struct point {
    struct pos _pos;
    struct phy _phy;
};

struct touch {
    struct phy buf[FILTER_BUF_LEN];
    struct phy cur_phy;

    uint32_t filter_len;
    uint32_t window;
    uint32_t discard;

	struct point point1;
	struct point point2;
	struct point point_centre;
	
	float x_coe;
	float y_coe;
	int precision;
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
	
    /* sort */
	for (i = 0; i < ptouch->filter_len; i++) {
		for (j = ptouch->filter_len; j > i; j--) {
			if (ptouch->buf[j].x < ptouch->buf[j - 1].x)
				phy_swap(&ptouch->buf[j], &ptouch->buf[j - 1]);
        }
    }

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

static uint16_t _read_phy(uint8_t cmd)	  
{ 	 
	uint16_t val = 0;
	
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
        }
	}
}

static void touch_read_phy(void) 
{
	int i;

    for (i = 0; i < g_touch.filter_len; i++) {
        g_touch.buf[i].x = _read_phy(CMD_RDY);
        g_touch.buf[i].y = _read_phy(CMD_RDX);
    }
    _do_filter(&g_touch);
}

static void touch_trans(GUI_PID_STATE *state, struct touch *ptouch)
{
	struct pos pos_calc;
	float temp;

	temp = (float)(ptouch->cur_phy.x) / ptouch->x_coe;
	pos_calc.x = (uint16_t)temp;
	temp = (float)(ptouch->cur_phy.y) / ptouch->y_coe;
	pos_calc.y = (uint16_t)temp;
	TOUCH_DBG_PRINT("get point(%d,%d).\r\n", pos_calc.x, pos_calc.y);
	
	if (pos_calc.x > TOUCH_X || pos_calc.y > TOUCH_Y)
		state->Pressed = 0;
	else {
		pos_calc.x = TOUCH_X - pos_calc.x;
		state.x = pos_calc.x;
		state.y = pos_calc.y;
		state.Pressed = 1;
	}
}

void touch_calibrate(void)
{
	struct touch *ptouch = &g_touch;
	float _phy, _pos;
	struct pos pos_calc;
	float temp;
	int _presision;

do_calc:
	TOUCH_DBG_PRINT("do touch calibrate...\r\n");
	
	/* draw p1 */
	TOUCH_DBG_PRINT("draw p1(%d,%d), waitting for press.\r\n",
		ptouch->point1.pos.x, ptouch->point1.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	touch_read_phy();
	ptouch->point1.phy.x = ptouch->cur_phy.x;
	ptouch->point1.phy.y = ptouch->cur_phy.y;
	TOUCH_DBG_PRINT("get p1 adc value (%d,%d).\r\n",
		ptouch->point1.phy.x, ptouch->point1.phy.y);
	delay_ms(500);

	/* draw p2 */
	TOUCH_DBG_PRINT("draw p2(%d,%d), waitting for press.\r\n",
		ptouch->point2.pos.x, ptouch->point2.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	touch_read_phy();
	ptouch->point2.phy.x = ptouch->cur_phy.x;
	ptouch->point2.phy.y = ptouch->cur_phy.y;
	TOUCH_DBG_PRINT("get p2 adc value (%d,%d).\r\n",
		 ptouch->point2.phy.x, ptouch->point2.phy.y);
	delay_ms(500);

	/* calc coe */
	TOUCH_DBG_PRINT("calculate coe\r\n");
	_phy = (float)(ptouch->point1.phy.x - ptouch->point2.phy.x);
	_pos = (float)(ptouch->point2.pos.x - ptouch->point1.pos.x);
	ptouch->x_coe = _phy / _pos;
	_phy = (float)(ptouch->point2.phy.y - ptouch->point1.phy.y);
	_pos = (float)(ptouch->point2.pos.y - ptouch->point1.pos.y);
	ptouch->y_coe = _phy / _pos;
	
	/* draw point_centre */
	TOUCH_DBG_PRINT("draw p3(%d,%d), waitting for press.\r\n",
		ptouch->point_centre.pos.x, ptouch->point_centre.pos.y);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10));
	touch_read_phy();
	ptouch->point_centre.phy.x = ptouch->cur_phy.x;
	ptouch->point_centre.phy.y = ptouch->cur_phy.y;
	TOUCH_DBG_PRINT("get p2 adc value (%d,%d).\r\n",
		 ptouch->point_centre.phy.x, ptouch->point_centre.phy.y);

	/* use point_centre to verify */
	TOUCH_DBG_PRINT("use point_centre to verify\r\n");
	temp = (float)(ptouch->point_centre.phy.x) / ptouch->x_coe;
	pos_calc.x = (uint16_t)temp;
	temp = (float)(ptouch->point_centre.phy.y) / ptouch->y_coe;
	pos_calc.y = (uint16_t)temp;
	TOUCH_DBG_PRINT("get point_centre(%d,%d).\r\n", pos_calc.x, pos_calc.y);

	/* adjust */
	_precision = abs((int)(ptouch->point_centre.pos.x - pos_calc.x));
	if (ptouch->precision > _precision) {
		TOUCH_DBG_PRINT("calibrate fail, retry\r\n");
		goto do_calc;
	}
	_precision = abs((int)(ptouch->point_centre.pos.y - pos_calc.y));
	if (ptouch->precision > _precision) {
		TOUCH_DBG_PRINT("calibrate fail, retry\r\n");
		goto do_calc;
	}
	TOUCH_DBG_PRINT("calibrate succeed!\r\n");
}

void touch_update(void)
{
	GUI_PID_STATE pstate;

	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
        pstate.Pressed = 0;
		goto store;
    }
    
    touch_read_phy();
    touch_trans(&pstate, &g_touch);

store:
	GUI_PID_StoreState(&pstate);
}

int touch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

	/* spi cs pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* irq pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
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
	g_touch.precision = 5;
    return 0;
}

