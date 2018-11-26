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

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
struct ui_msg g_ui_msg;

static struct tprinter g_printer;
static struct report report_test;
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
			ui_blocktest_creat();
			break;
		case MSG_LOAD_UI_TEST:
			ui_test_creat();
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
	xTaskCreate(task_helloworld, "hello world", 64, NULL, 1, NULL);
	xTaskCreate(task_touch, "touch", 128, NULL, 2, &handle_touch);
	xTaskCreate(task_ui, "ui", 512, NULL, 1, &handle_gui);
	xTaskCreate(exper_task, "exper", 256, NULL, 1, NULL);
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
	printf("System Init!\r\n");
	printf("CoreClock = %dMHz\r\n", SystemCoreClock / 1000000);
	spi1_init();
	spi2_init();
	spi3_init();
	w25xxx_init();
	sysconf_load();

	/* ad7705 test */
	ad770x_init();
	///ad7705_test();
#if 0
	stepmotor_init();
	while (1) {
		status = uart_get_status();
		switch (status) {
		case 1:
			printf("run cmd 1: motor up 0.3ml\r\n");
			stepmotor_run(MOTOR_DIR_UP, MOTOR_WATER_01ML * 3);
			printf("finished\r\n");
			break;
		case 2:
			printf("run cmd 2: motor down\r\n");
			stepmotor_run(MOTOR_DIR_DOWN, MOTOR_WATER_01ML);
			printf("finished\r\n");
			break;
		case 3:
			printf("run cmd 3: switch to put\r\n");
			relay_ctrl(MOTOR_WATER_PUT);
			printf("finished\r\n");
			break;
		case 4:
			printf("run cmd 4: switch to get\r\n");
			relay_ctrl(MOTOR_WATER_GET);
			printf("finished\r\n");
			break;
		case 5:
			printf("run cmd 5: motor down fast\r\n");
			stepmotor_run(MOTOR_DIR_DOWN, 8000);
			printf("finished\r\n");
			break;
		case 6:
			printf("run cmd 6: motor up fast\r\n");
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
		case 9:
			res = f_mount(SPI, &spiflash_fs);
			if (res) {
				printf("remount fatfs fail, res = %d\r\n", res);
				break;
			}
			
			res = f_open(&gfp, "1:/hello.txt", FA_CREATE_NEW | FA_WRITE);
			if (res == FR_NO_FILESYSTEM) {
				printf("f_open fail, no filesystem\r\n");
				printf("mk a fatfs\r\n");
				printf("prepare to erase chip\r\n");
				w25xxx_erase_chip();
            	printf("chip erased!\r\n");
				res = f_mkfs(SPI, 1, W25X20_BLOCK_SIZE);
				if (res) {
					printf("mkfs fail, res = %d\r\n", res);
					break;
				}
			} else if (res) {
				printf("f_open fail, res=%d\r\n", res);
				break;
			}

			res = f_open(&gfp, "1:/hello.txt", FA_CREATE_NEW | FA_WRITE);
			if (res) {
				printf("file open error, res = %d\r\n", res);
				break;
			}

			res = f_write(&gfp, "Hello, World!\r\n", 15, &gbw);
			if (gbw != 15) {
				printf("write file fail, res = %d, gbw = %d\r\n", res, gbw);
				break;
			}

			f_close(&gfp);
			f_mount(SPI, NULL);
			break;
		case 0x0B:
			printf("read test\r\n");
			res = f_mount(SPI, &spiflash_fs);
			if (res) {
				printf("mount fatfs fail, res = %d\r\n", res);
				break;
			}
			
			res = f_open(&gfp, "1:/hello.txt", FA_READ);
			if (res) {
				printf("f_open error res = %d\r\n", res);
				break;
			}

			res = f_read(&gfp, buffer, 15, &gbw);
			if (res) {
				printf("f_read error res = %d\r\n", res);
				break;
			}
			printf("result: %s", buffer);
			f_close(&gfp);
			f_mount(SPI, NULL);
			break;
		case 0x0C:
			w25xxx_test();
			break;
		case 0x0D:
			printf("chip erase\r\n");
			w25xxx_erase_chip();
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

	GUI_Init();
	touch_init();
	touch_calibrate();
	
	/* creat freertos task */
	task_init();

	/* test the printer, fill obj report,  then show */	
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
	//report_show(&report_test);


	/* run rtos */
	vTaskStartScheduler();
	/* if run here, system error */
	while (1) {
		printf("system error!\r\n");
        delay_ms(2000);
	}
	return 0;
}


