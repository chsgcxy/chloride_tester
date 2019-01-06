#ifndef __REPORT_H__
#define __REPORT_H__

#include "stm32f2xx.h"
#include "rd-tprinter.h"
#include "data.h"

extern int report_init(struct tprinter *printer);
extern int report_show(struct result_data *rp);

#endif
