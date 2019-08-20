#ifndef __AD770X_H__
#define __AD770X_H__

#include "stm32f2xx.h"

extern int ad770x_init(void);
extern int ad770x_read_val(u16 *val);
extern float ad7705_read(void);
extern void volt_base_clear(void);
extern void volt_base_set(float volt);

#endif




















