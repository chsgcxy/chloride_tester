#ifndef __RD_TPRINTER_H
#define __RD_TPRINTER_H

#include "stm32f2xx.h"

struct tprinter {
    const char *name;
    int (*send)(u8 *buf, int len);
};

extern int tprinter_init(struct tprinter *printer);
extern int tprinter_flush(struct tprinter *printer);
extern int tprinter_flush_wrap(struct tprinter *printer);
extern int tprinter_flush_enter(struct tprinter *printer);
extern int tprinter_flush_step(struct tprinter *printer, u8 step);
extern int tprinter_flush_line(struct tprinter *printer, u8 line);

#endif
