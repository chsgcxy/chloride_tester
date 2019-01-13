#include "rtc.h"
#include "stm32f2xx_rtc.h"
#include "stdio.h"

#define RTC_Debug

#ifdef RTC_Debug
	#define RTC_DBG_PRINT(fmt, args...)    printf(fmt, ##args)
#else
	#define RTC_DBG_PRINT(fmt, args...)
#endif

#define RTC_CLOCK_SOURCE_LSE       /* LSE */
//#define RTC_CLOCK_SOURCE_LSI     /* LSI */ 

RTC_TimeTypeDef  RTC_TimeStructure;
RTC_DateTypeDef  RTC_DateStructure;

__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
uint8_t aShowTime[50] = {0};

void RTC_TimeShow(void)
{
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	printf("%0.2d:%0.2d:%0.2d\r\n",
		RTC_TimeStructure.RTC_Hours,
		RTC_TimeStructure.RTC_Minutes,
		RTC_TimeStructure.RTC_Seconds);
}

void RTC_DateShow(void)
{
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

	printf("20%0.2d-%0.2d-%0.2d-%0.1d\r\n",
		RTC_DateStructure.RTC_Year,
		RTC_DateStructure.RTC_Month,
		RTC_DateStructure.RTC_Date,
		RTC_DateStructure.RTC_WeekDay);
}

void RTC_AlarmShow(void)
{	
	RTC_AlarmTypeDef RTC_AlarmStructure;

	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	printf("%0.2d:%0.2d:%0.2d",
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, 
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, 
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}

static void RTC_Config(void)
{
	RTC_InitTypeDef  RTC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;
#elif defined (RTC_CLOCK_SOURCE_LSE)
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;
#else
#error Please select the RTC Clock source inside the main.c file
#endif 
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	RTC_DateStructure.RTC_Year = 0x19;
	RTC_DateStructure.RTC_Month = RTC_Month_January;
	RTC_DateStructure.RTC_Date = 0x11;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

	RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x05;
	RTC_TimeStructure.RTC_Minutes = 0x20;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

	RTC_WriteBackupRegister(RTC_BkpRegister_0, 0xAA55A501);	
}

void rtc_init(void) 
{ 
	if (RTC_ReadBackupRegister(RTC_BkpRegister_0) != 0xAA55A501) {
		RTC_Config();
		RTC_DBG_PRINT("use rtc first\r\n");
	} else { 
		RTC_DBG_PRINT("use rtc n times\r\n");
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		PWR_BackupAccessCmd(ENABLE);
		RTC_WaitForSynchro();
	}
}

void RTC_TimeRegulate(void)
{
	RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours = 11;
	RTC_TimeStructure.RTC_Minutes = 10;
	RTC_TimeStructure.RTC_Seconds = 20;
	
	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
		printf("\r\n>> !! time set fail !! <<\r\n");
	else {
		printf("\r\n>> !! time set succeed !! <<\r\n");
		RTC_TimeShow();
		RTC_WriteBackupRegister(RTC_BkpRegister_0, 0xAA55A501);
	}
}
