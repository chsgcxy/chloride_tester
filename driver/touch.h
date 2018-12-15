#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "stm32f2xx.h"
#include "sysconf.h"

extern int touch_init(void);
extern void touch_test(void);
extern void touch_update(void);
extern void touch_calibrate(int force);

#endif
