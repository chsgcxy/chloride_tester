#ifndef __SYSCONF_H__
#define __SYSCONF_H__

#include "stm32f2xx.h"

#define ZSB_VALID_FLAG      0xAA55A5A0
#define TOUCH_VALID_FLAG    0xAA55A5A1
#define DJDW_VALID_FLAG     0xAA55A5A2

struct sysconf {
    uint32_t magic;
	double x_coe;
	double y_coe;
	int32_t x_correct;
	int32_t y_correct;
	float volt_scale;
	uint32_t touch_valid;

	uint32_t zsb_valid;
	uint32_t zsb_len;

	uint32_t djdw_valid;
	float djdw_val;
};

extern int sysconf_load(void);
extern int sysconf_save(void);
extern struct sysconf *sysconf_get(void);
extern int sysconf_is_valid(void);

#endif
