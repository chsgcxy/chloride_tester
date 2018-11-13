#include "stm32f2xx_gpio.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"
#include "usbh_usr.h" 
#include "stm32_uart.h"
#include "stm32_spi.h"
#include "report.h"
#include "ad770x.h"
#include "touch.h"
#include "GUI.h"
#include "WM.h"

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

static void task_helloworld(void *args)
{
	while (1) {
		printf("hello world!\r\n");
		vTaskDelay(2000);
	}
}

static void task_touch(void *args)
{
	while (1) {
		touch_update();
		vTaskDelay(10);
	}
}

static void task_init(void)
{
	xTaskCreate(task_helloworld, "hello world", 64, NULL, 1, NULL);
	xTaskCreate(task_touch, "task", 512, NULL, 1, NULL);
}

static int g_printer_send(uint8_t *buf, int len)
{
	return uart_send_buf(PRINTER_PORT, buf, len);
}

int main(void)
{
	/* disable global interrupt, it will be opened by prvStartFirstTask int port.c */
	//__set_PRIMASK(1);
	/* enable CRC, for stemwin */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	/* rtos will not run if I set group_2, group_4 is work */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* console and uart init, depent on config.h */
	uart1_init();
	uart4_init();
	spi1_init();
	spi3_init();
	printf("System Init!\r\n");
	printf("CoreClock = %dMHz\r\n", SystemCoreClock / 1000000);

	/* USB Test */
#if 0	
	USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host,
		&USBH_MSC_cb, &USR_cb);
	printf("USB inited...\r\n");
	while (1) {
		USBH_Process(&USB_OTG_Core, &USB_Host);
	}
#endif

#if 0
	/* ad7705 test */
	ad770x_init();
	ad7705_test();
#endif

	/* GUI test */
	GUI_Init();
	
#if 0
	GUI_GotoXY(50, 50);
	GUI_SetFont(GUI_FONT_32B_ASCII);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLUE);
	GUI_DispString("hello world!");
#endif
	
	touch_init();
	//touch_test();
	touch_calibrate();

	/* creat freertos task */
	task_init();

	/* test the printer, fill obj report,  then show */
#if 0	
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
#endif

	/* run rtos */
	vTaskStartScheduler();
	/* if run here, system error */
	while (1) {
		printf("system error!\r\n");
        delay_ms(2000);
	}
	return 0;
}


