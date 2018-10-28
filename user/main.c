#include "stm32f2xx_gpio.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"
#include "usbh_usr.h" 
#include "uart.h"
#include "report.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
static struct tprinter g_printer;
static struct report report_test;

void delay_ms(int ms)
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
		vTaskDelay(200);
		GPIO_ResetBits(GPIOC, GPIO_Pin_11);
		vTaskDelay(200);
	}
}

static void led_task3(void *param)
{
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_12);
		vTaskDelay(200);
		GPIO_ResetBits(GPIOC, GPIO_Pin_12);
		vTaskDelay(200);
        printf("hello world!\r\n");
	}
}

static void task_init(void)
{
	xTaskCreate(led_task1, "led1", 64, NULL, 1, NULL);
    xTaskCreate(led_task2, "led2", 64, NULL, 1, NULL);
	xTaskCreate(led_task3, "led3", 64, NULL, 1, NULL);
}

static int g_printer_send(uint8_t *buf, int len)
{
	return uart_send_buf(PRINTER_PORT, buf, len);
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
	uart4_init();
	printf("System Init!\r\n");
	printf("CoreClock = %dMHz\r\n", SystemCoreClock / 1000000);
    gpio_init();
	//lcd_init();
	task_init();

	g_printer.name = "simple printer";
    g_printer.send = g_printer_send;
	report_init(&g_printer);
	
	report_test.data[0] = 12.31;
	report_test.data[1] = 22.5;
	report_test.data[2] = 23.84;
	report_test.data[3] = 128.1;
	report_test.data[4] = 129.82;
	report_test.data[5] = 130.9;
	report_test.data[6] = 222.22;
	report_test.data[7] = 223.31;
	report_test.data[8] = 224.4;
	report_test.data[9] = 225.50;
	report_test.data[10] = 24.31;
	report_test.data[11] = 34.5;

	report_test.data_num = 12;
	report_test.nitrate_dosage = 32.78;
	report_test.percentage = 0.78;
	report_test.year = 18;
	report_test.month = 10;
	report_test.day = 28;
	report_test.hour = 14;
	report_test.minute = 28;
	report_show(&report_test);

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


