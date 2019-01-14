#include "beep.h"
#include "stm32f2xx.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

void beep_work(int work, int idol)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    vTaskDelay(work);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
	vTaskDelay(idol);
}

void beep_work_delay(int work, int idol)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    delay_ms(work);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
	delay_ms(idol);
}

void beep_finished(void)
{
	beep_work(500, 50);
	beep_work(500, 0);
}

void beep_finished_delay(void)
{
	beep_work_delay(500, 50);
	beep_work_delay(500, 0);
}

void beep_clicked(void)
{
	beep_work(30, 0);
}

void beep_clicked_delay(void)
{
   	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    delay_ms(30);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
}
    
void beep_warning(void)
{
	beep_work(100, 30);
	beep_work(100, 30);
	beep_work(100, 0);
}

void beep_warning_delay(void)
{
	beep_work_delay(100, 30);
	beep_work_delay(100, 30);
	beep_work_delay(100, 0);
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
