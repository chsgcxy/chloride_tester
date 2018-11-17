#ifndef __STEP_MOTOR_H__
#define __STEP_MOTOR_H__

#include "stm32f2xx.h"

extern int stepmotor_init(void);
extern void stepmotor_run(int dir, int step);


#endif
