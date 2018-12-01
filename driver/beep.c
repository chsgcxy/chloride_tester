#include "beep.h"
#include "stm32f2xx.h"
#include "delay.h"

void beep_work(int ms)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    delay_ms(ms);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

int beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* spi cs pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);

    return 0;
}
