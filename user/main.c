#include "stm32f2xx_gpio.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"

extern int uart1_init(void);

static void delay_ms(int ms)
{
	int i;
	
	while (ms--)
		for (i = 0; i < 28000; i++);
}

static int gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	return 0;
}

static void led_task1(void *param)
{
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_10);
		vTaskDelay(200);
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);
		vTaskDelay(200);
	}
}

static void led_task2(void *param)
{
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_11);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_11);
		vTaskDelay(500);
	}
}

static void led_task3(void *param)
{
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_12);
		vTaskDelay(1000);
		GPIO_ResetBits(GPIOC, GPIO_Pin_12);
		vTaskDelay(1000);
	}
}

static void task_init(void)
{
	xTaskCreate(led_task1, "led1", 64, NULL, 1, NULL);
    xTaskCreate(led_task2, "led2", 64, NULL, 1, NULL);
	xTaskCreate(led_task3, "led3", 64, NULL, 1, NULL);
}

int main(void)
{
	/* disable global interrupt, it will be opened by prvStartFirstTask int port.c */
	__set_PRIMASK(1);
	/* enable CRC, for stemwin */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	/* rtos will not run if I set group_2, group_4 is work */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	uart1_init();
	gpio_init();
	task_init();

	vTaskStartScheduler();

	/* if run here, system error */
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
		delay_ms(100);
		GPIO_ResetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
		delay_ms(100);
	}
	return 0;
}


