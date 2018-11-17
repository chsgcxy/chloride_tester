#include "delay.h"

void delay_ms(int ms)
{
    int i;

    while (ms--)
        for (i = 0; i < 24000; i++);
}

void delay_us(int us)
{
    int i;

    while (us--)
        for (i = 0; i < 28; i++);
}
