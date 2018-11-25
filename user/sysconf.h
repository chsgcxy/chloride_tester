#ifndef __SYSCONF_H__
#define __SYSCONF_H__

#include "stm32f2xx.h"

struct sysconf {
    uint32_t magic;
	double x_coe;
	double y_coe;
	int32_t x_correct;
	int32_t y_correct;
};

extern int sysconf_load(void);
extern int sysconf_save(void);
extern struct sysconf *sysconf_get(void);
extern int sysconf_is_valid(void);

#endif
