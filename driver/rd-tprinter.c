/**
  * reference RD-Thermal printer instruction manual-V1.1
  * the printer dirver
  */
#include "rd-tprinter.h"

/**
  * @ notes
  * @ baudrate : 9600 (it seems can not be changed)
  * @ 
  */

/**
  * ESC @
  */
int tprinter_prepare(struct tprinter *printer)
{
    uint8_t cmd[2];

    cmd[0] = 0x1B;
    cmd[1] = 0x40;
    return printer->send(cmd, 2);
}

int tprinter_zoomin(struct tprinter *printer, uint8_t nx, uint8_t ny)
{
    uint8_t cmd[4];

    cmd[0] = 0x1B;
    cmd[1] = 0x58;
    cmd[2] = nx;
    cmd[3] = ny;

    return printer->send(cmd, 4);
}

int tprinter_send(struct tprinter *printer, uint8_t *buf, int len)
{
    return printer->send(buf, len);
}

/**
  * FF 
  */
int tprinter_flush(struct tprinter *printer)
{
    uint8_t cmd[2];

    cmd[0] = 0x0C;
    return printer->send(cmd, 1);
}

/**
  * LF 
  */
int tprinter_flush_wrap(struct tprinter *printer)
{
    uint8_t cmd[2];

    cmd[0] = 0x0A;
    return printer->send(cmd, 1);
}

/**
  * CR 
  */
int tprinter_newline(struct tprinter *printer)
{
    uint8_t cmd[2];

    cmd[0] = 0x0D;
    return printer->send(cmd, 1);
}

/**
  * ESC J
  * flush and move to (step * 0.125mm)
  */
int tprinter_flush_step(struct tprinter *printer, uint8_t step)
{
    uint8_t cmd[3];

    cmd[0] = 0x1B;
    cmd[1] = 0x4A;
    cmd[2] = step;
    return printer->send(cmd, 3);
}

/**
  * ESC D
  * flush and move n line
  * a line = 24 * 0.125mm
  */
int tprinter_flush_line(struct tprinter *printer, uint8_t line)
{
    uint8_t cmd[3];

    cmd[0] = 0x1B;
    cmd[1] = 0x64;
    cmd[2] = line;
    return printer->send(cmd, 3);
}

int tprinter_pos(struct tprinter *printer, int pos)
{
    uint8_t cmd[4];

    cmd[0] = 0x1B;
    cmd[1] = 0x24;
    cmd[2] = (uint8_t)(pos & 0xff);
    cmd[3] = (uint8_t)((pos >> 8) & 0xff);
    return printer->send(cmd, 4);   
}

/**
  * ESC c
  * enable / diable reverse
  */
int tprinter_reverse_ctrl(struct tprinter *printer, uint8_t enable)
{
    uint8_t cmd[3];

    cmd[0] = 0x1B;
    cmd[1] = 0x63;
    if (enable)
        cmd[2] = 1;
    else
        cmd[2] = 0;
    return printer->send(cmd, 3);
}
