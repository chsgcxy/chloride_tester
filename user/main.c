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
#include "stepmotor.h"
#include "delay.h"
#include "w25xxx.h"
#include "ff.h"
#include "diskio.h"
#include "main.h"
#include "experiment.h"
#include "sysconf.h"
#include "beep.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
struct ui_msg g_ui_msg;

static struct tprinter g_printer;
static TaskHandle_t handle_touch, handle_gui;

extern WM_HWIN main_menu_creat(void);
extern WM_HWIN ui_blocktest_creat(void);
extern WM_HWIN ui_setting_creat(void);
extern WM_HWIN ui_test_creat(void);
extern WM_HWIN ui_data_creat(void);
extern WM_HWIN data_detail_creat(void);

static void task_helloworld(void *args)
{
	while (1) {
		printf("hello world!\r\n");
		vTaskDelay(10000);
	}
}

static void task_touch(void *args)
{
	while (1) {
		touch_update();
		vTaskDelay(20);
	}
}

static void task_ui(void *args)
{
	g_ui_msg.msg = MSG_LOAD_UI_MENU;
	while (1) {
		switch (g_ui_msg.msg) {
		case MSG_LOAD_UI_MENU:
			main_menu_creat();
			break;
		case MSG_LOAD_UI_BLOCKTEST:
			ui_blocktest_creat();
			break;
		case MSG_LOAD_UI_SETTING:
			ui_setting_creat();
			break;
		case MSG_LOAD_UI_DATA:
			ui_data_creat();
			break;
		case MSG_LOAD_UI_DETAIL:
			data_detail_creat();
			break;
		default:
			break;
		}
	}
}

static void task_init(void)
{
	//xTaskCreate(task_helloworld, "hello world", 64, NULL, 1, NULL);
	xTaskCreate(task_touch, "touch", 128, NULL, 2, &handle_touch);
	xTaskCreate(task_ui, "ui", 512, NULL, 1, &handle_gui);
	xTaskCreate(exper_task, "exper", 512, NULL, 1, NULL);
}

static int g_printer_send(uint8_t *buf, int len)
{
	return uart_send_buf(PRINTER_PORT, buf, len);
}

int main(void)
{
	int status;

	/* disable global interrupt, it will be opened by prvStartFirstTask int port.c */
	//__set_PRIMASK(1);
	/* enable CRC, for stemwin */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	/* rtos will not run if I set group_2, group_4 is work */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* console and uart init, depent on config.h */
	uart1_init();
	uart4_init();
	printf("System Init!\r\n");
	printf("CoreClock = %dMHz\r\n", SystemCoreClock / 1000000);
	spi1_init();
	spi2_init();
	spi3_init();
	ad770x_init();
	beep_init();
	w25xxx_init();
	//w25xxx_erase_chip();
	stepmotor_init();
	sysconf_load();
	GUI_Init();
	touch_init();
	touch_calibrate();

	g_printer.name = "simple printer";
    g_printer.send = g_printer_send;
	report_init(&g_printer);

#if 0
	while (1) {
		status = uart_get_status();
		switch (status) {
		case 0x11:
			printf("run cmd 1: motor up 0.3ml\r\n");
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * 3);
			printf("finished\r\n");
			break;
		case 0x12:
			printf("run cmd 2: motor down\r\n");
			stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML);
			printf("finished\r\n");
			break;
		case 0x01:
			printf("run cmd 5: motor down fast, change dir to get water\r\n");
			relay_ctrl(MOTOR_WATER_GET);
			stepmotor_run(MOTOR_DIR_DOWN, 8000);
			printf("finished\r\n");
			break;
		case 0x02:
			printf("run cmd 6: motor up fast, change dir to put water\r\n");
			relay_ctrl(MOTOR_WATER_PUT);
			stepmotor_run(MOTOR_DIR_UP, 8000);
			printf("finished\r\n");
			break;
		case 7:
			printf("run cmd 7: motor up 0.1ml\r\n");
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML);
			printf("finished\r\n");
			break;
		case 8:
			printf("run cmd 8: motor up 1ml\r\n");
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * 10);
			printf("finished\r\n");
			break;
		case 0x03:
			printf("beep work 100ms\r\n");
			beep_work(100);
			printf("finished\r\n");
			break;
		case 0x04:
			printf("ad7705 read\r\n");
			ad7705_read();
			printf("finished\r\n");
			break;
		case 0x0C:
			break;
		case 0x0D:
			break;
		default:
			break;
		}
	}
#endif

	/* USB Test */
#if 0	
	USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host,
		&USBH_MSC_cb, &USR_cb);
	printf("USB inited...\r\n");
	while (1) {
		USBH_Process(&USB_OTG_Core, &USB_Host);
	}
#endif

	/* creat freertos task */
	task_init();

	/* run rtos */
	vTaskStartScheduler();
	/* if run here, system error */
	while (1) {
		printf("system error!\r\n");
        delay_ms(2000);
	}
	return 0;
}


