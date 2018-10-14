/**
  * reference RD-Thermal printer instruction manual-V1.1
  * the printer dirver
  */
#include "rd-tprinter.h"

/**
  * ESC @
  */
int tprinter_init(struct tprinter *printer)
{
    u8 cmd[2];

    cmd[0] = 0x1B;
    cmd[1] = 0x40;
    return printer->send(cmd, 2);
}

/**
  * FF 
  */
int tprinter_flush(struct tprinter *printer)
{
    u8 cmd[2];

    cmd[0] = 0x0C;
    return printer->send(cmd, 1);
}

/**
  * LF 
  */
int tprinter_flush_wrap(struct tprinter *printer)
{
    u8 cmd[2];

    cmd[0] = 0x0A;
    return printer->send(cmd, 1);
}

/**
  * CR 
  */
int tprinter_flush_enter(struct tprinter *printer)
{
    u8 cmd[2];

    cmd[0] = 0x0D;
    return printer->send(cmd, 1);
}

/**
  * ESC J
  * flush and move to (step * 0.125mm)
  */
int tprinter_flush_step(struct tprinter *printer, u8 step)
{
    u8 cmd[3];

    cmd[0] = 0x1B;
    cmd[1] = 0x4A;
    cmd[3] = step;
    return printer->send(cmd, 3);
}

/**
  * ESC D
  * flush and move n line
  * a line = 24 * 0.125mm
  */
int tprinter_flush_line(struct tprinter *printer, u8 line)
{
    u8 cmd[3];

    cmd[0] = 0x1B;
    cmd[1] = 0x64;
    cmd[3] = line;
    return printer->send(cmd, 3);
}

/**
  * ESC c
  * enable / diable reverse
  */
int tprinter_flush_line(struct tprinter *printer, u8 enable)
{
    u8 cmd[3];

    cmd[0] = 0x1B;
    cmd[1] = 0x63;
    if (enable)
        cmd[3] = 1;
    else
        cmd[3] = 0;
    return printer->send(cmd, 3);
}
