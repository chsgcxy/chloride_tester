#ifndef __RD_TPRINTER_H
#define __RD_TPRINTER_H

#include "stm32f2xx.h"

struct tprinter {
    const char *name;
    int (*send)(uint8_t *buf, int len);
};

extern int tprinter_prepare(struct tprinter *printer);
extern int tprinter_flush(struct tprinter *printer);
extern int tprinter_flush_wrap(struct tprinter *printer);
extern int tprinter_newline(struct tprinter *printer);
extern int tprinter_flush_step(struct tprinter *printer, uint8_t step);
extern int tprinter_flush_line(struct tprinter *printer, uint8_t line);
extern int tprinter_reverse_ctrl(struct tprinter *printer, uint8_t enable);
extern int tprinter_zoomin(struct tprinter *printer, uint8_t nx, uint8_t ny);
extern int tprinter_send(struct tprinter *printer, uint8_t *buf, int len);
extern int tprinter_pos(struct tprinter *printer, int pos);

#endif
