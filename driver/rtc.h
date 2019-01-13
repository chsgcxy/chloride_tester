#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f2xx.h"

struct date_time {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t week;
};					 

#endif
