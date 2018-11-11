#include "touch.h"
#include "stm32_spi.h"
#include "stdio.h"
#include "GUI.h"

#define CMD_RDY 0X90
#define CMD_RDX	0XD0

#define FILTER_DISCARD    5
#define FILTER_WINDOW     10
#define FILTER_BUF_LEN    (FILTER_DISCARD * 2 + FILTER_WINDOW)

struct point {
    uint32_t x;
    uint32_t y;
};

struct touch {
    struct point adc_val[FILTER_BUF_LEN];
    struct point filtered_val;
    uint32_t filter_len;
    uint32_t window;
    uint32_t discard;
};

static struct touch g_touch;

static void point_swap(struct point *p1, struct point *p2)  
{  
    struct point temp;

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
			if (ptouch->adc_val[j].x < ptouch->adc_val[j - 1].x)
				point_swap(&ptouch->adc_val[j], &ptouch->adc_val[j - 1]);
        }
    }

    /* get avarage */
    ptouch->filtered_val.x = 0;
    ptouch->filtered_val.y = 0;
	for (i = ptouch->discard; i < (ptouch->discard + ptouch->window); i++) {
        ptouch->filtered_val.x += ptouch->adc_val[i].x;
        ptouch->filtered_val.y += ptouch->adc_val[i].y;
    }
	
    ptouch->filtered_val.x = ptouch->filtered_val.x / ptouch->window;
    ptouch->filtered_val.y = ptouch->filtered_val.y / ptouch->window;
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
    struct point p;

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
        g_touch.adc_val[i].x = _read_phy(CMD_RDY);
        g_touch.adc_val[i].y = _read_phy(CMD_RDX);
    }
    _do_filter(&g_touch);
}

static void touch_trans(GUI_PID_STATE *state, struct touch *ptouch)
{

}

void touch_calibrate(void)
{


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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    g_touch.filter_len = FILTER_BUF_LEN;
    g_touch.window = FILTER_WINDOW;
    g_touch.discard = FILTER_DISCARD;

    return 0;
}
