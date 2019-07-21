#include "stm32f2xx_gpio.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"
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
#include "ds18b20.h"
#include "data.h"
#include "rtc.h"

struct ui_msg g_ui_msg;

static struct tprinter g_printer;
static TaskHandle_t handle_touch, handle_gui;
static struct ui_exper_test test_func;

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

extern int main_menu_creat(void);
extern int ui_blocktest_creat(struct ui_exper_test *test);
extern int ui_setting_creat(void);
extern int ui_test_creat(void);
extern int ui_data_creat(void);
extern int data_detail_creat(int item);
extern void lcd_io_init(void);

extern const GUI_BITMAP bmlogo;
extern const GUI_FONT GUI_FontHZ_kaiti_28;
extern const GUI_FONT GUI_FontHZ_kaiti_20;

static void task_touch(void *args)
{
	while (1) {
		touch_update();
		vTaskDelay(10);
	}
}

static void task_usb(void *args)
{
	while (1) {
		USBH_Process(&USB_OTG_Core, &USB_Host);
		vTaskDelay(10);
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
			test_func.func = MSG_LOAD_UI_BLOCKTEST;
			ui_blocktest_creat(&test_func);
			break;
		case MSG_LOAD_UI_STAND:
			test_func.func = MSG_LOAD_UI_STAND;
			ui_blocktest_creat(&test_func);
			break;
		case MSG_LOAD_UI_SETTING:
			ui_setting_creat();
			break;
		case MSG_LOAD_UI_DATA:
			ui_data_creat();
			break;
		case MSG_LOAD_UI_DETAIL:
			data_detail_creat(g_ui_msg.param0);
			break;
		case MSG_LOAD_UI_TOUCH_CALC:
			vTaskSuspend(handle_touch);
			touch_calibrate(1);
			vTaskResume(handle_touch);
			ui_setting_creat();
			break;
		case MSG_LOAD_UI_DROPPER:
			test_func.func = MSG_LOAD_UI_DROPPER;
			ui_blocktest_creat(&test_func);
			break;
		case MSG_LOAD_UI_EXTEST:
			test_func.func = MSG_LOAD_UI_EXTEST;
			ui_blocktest_creat(&test_func);
			break;
		default:
			break;
		}
	}
}

static void task_init(void)
{
	xTaskCreate(task_touch, "touch", 128, NULL, 2, &handle_touch);
	xTaskCreate(task_ui, "ui", 512, NULL, 1, &handle_gui);
	xTaskCreate(exper_task, "exper", 512, NULL, 1, NULL);
	xTaskCreate(task_usb, "usb", 256, NULL, 1, NULL);
}

static int g_printer_send(uint8_t *buf, int len)
{
	return uart_send_buf(PRINTER_PORT, buf, len);
}

int main(void)
{
    //int status = 0;
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
	rtc_init();
	lcd_io_init();
	GUI_Init();

	/* show logo */
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_DrawBitmap(&bmlogo, 315, 150);
	GUI_SetFont(&GUI_FontHZ_kaiti_20);
	GUI_SetColor(GUI_BLACK);
	GUI_DispStringHCenterAt("ZCL-II型氯离子(全自动电位滴定仪)", 400, 60);
	GUI_SetFont(&GUI_FontHZ_kaiti_28);
	GUI_DispStringHCenterAt("北京同创中仪科技为您服务", 400, 350);
	delay_ms(1500);
	
	spi1_init();
	spi2_init();
	spi3_init();
	ad770x_init();
	beep_init();
	w25xxx_init();
	ds18b20_init();
	//w25xxx_erase_chip();
	stepmotor_init();

#if 0	
	while (1) {
		status = uart_get_status();
		switch (status) {
		case 0x01:
			relay_ctrl(MOTOR_WATER_PUT);
			printf("OK!\r\n");
			break;
		case 0x02:
			relay_ctrl(MOTOR_WATER_GET);
			printf("OK!\r\n");
			break;
		case 0x03:
			stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML);
			printf("OK!\r\n");
			break;
		case 0x04:
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML);
			printf("OK!\r\n");
			break;
		case 0x05:
			stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML * 100);
			printf("OK!\r\n");
			break;
		case 0x06:
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * 100);
			printf("OK!\r\n");
			break;
		case 0x07:
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * 3);
			printf("OK!\r\n");			
		default:
			break;
		}
		status = 0;
	}
#endif	
	
	/* load from flash */
	sysconf_load();
	data_init();

	touch_init();
	touch_calibrate(0);

	g_printer.name = "simple printer";
    g_printer.send = g_printer_send;
	report_init(&g_printer);

	exper_init();

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


