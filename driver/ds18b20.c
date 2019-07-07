#include "ds18b20.h"
#include "delay.h"
#include "stm32f2xx.h"
#include "stdio.h"


#define GPIO_MODE_SET(GPIOx, pinpos, mode)    do { \
    GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2)); \
    GPIOx->MODER |= (((uint32_t)mode) << (pinpos * 2)); \
} while(0);

#define GPIO_PIN_VAL_SET(GPIOx, GPIO_Pin, BitVal)    do { \
  if (BitVal != Bit_RESET) \
    GPIOx->BSRRL = GPIO_Pin; \
  else \
    GPIOx->BSRRH = GPIO_Pin; \
} while(0);


#define DS18B20_IO_IN()    GPIO_MODE_SET(GPIOC, 4, GPIO_Mode_IN)
#define DS18B20_IO_OUT()    GPIO_MODE_SET(GPIOC, 4, GPIO_Mode_OUT)
#define DS18B20_DQ_OUT(bitval)    GPIO_PIN_VAL_SET(GPIOC, GPIO_Pin_4, bitval)
#define DS18B20_DQ_IN    GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)

void ds18b20_reset(void)
{
    DS18B20_IO_OUT();   //SET PG11 OUTPUT
    DS18B20_DQ_OUT(0); //拉低DQ
    delay_us(750);      //拉低750us
    DS18B20_DQ_OUT(1); //DQ=1
    delay_us(15);       //15US
}

static int ds18b20_check(void)
{
    u8 retry = 0;
    DS18B20_IO_IN(); //SET PG11 INPUT

    while (DS18B20_DQ_IN && retry < 200)
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 200)
        return 1;
    else
        retry = 0;

    while (!DS18B20_DQ_IN && retry < 240)
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 240)
        return 1;
    return 0;
}

static u8 ds18b20_read_bit(void)
{
    u8 data;
    DS18B20_IO_OUT(); //SET PG11 OUTPUT
    DS18B20_DQ_OUT(0);
    delay_us(2);
    DS18B20_DQ_OUT(1);
    DS18B20_IO_IN(); //SET PG11 INPUT
    delay_us(12);
    if (DS18B20_DQ_IN)
        data = 1;
    else
        data = 0;
    delay_us(50);
    return data;
}

static u8 ds18b20_read_byte(void)
{
    u8 i, j, dat;
    dat = 0;
    for (i = 1; i <= 8; i++)
    {
        j = ds18b20_read_bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}

static void ds18b20_write_byte(u8 dat)
{
    u8 j;
    u8 testb;
    DS18B20_IO_OUT(); //SET PG11 OUTPUT;
    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        if (testb)
        {
            DS18B20_DQ_OUT(0); // Write 1
            delay_us(2);
            DS18B20_DQ_OUT(1);
            delay_us(60);
        }
        else
        {
            DS18B20_DQ_OUT(0); // Write 0
            delay_us(60);
            DS18B20_DQ_OUT(1);
            delay_us(2);
        }
    }
}

static void ds18b20_start_convert(void)
{
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc); // skip rom
    ds18b20_write_byte(0x44); // convert
}

int ds18b20_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_4); //输出1

    ds18b20_reset();

    return ds18b20_check();
}

float ds18b20_get_temp(void)
{
    u8 temp;
    u8 TL, TH;
    short val;
    float result;

    ds18b20_start_convert();
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc); // skip rom
    ds18b20_write_byte(0xbe); // convert
    TL = ds18b20_read_byte(); // LSB
    TH = ds18b20_read_byte(); // MSB

    if (TH > 7)
    {
        TH = ~TH;
        TL = ~TL;
        temp = 0; //温度为负
    }
    else
        temp = 1; //温度为正
    val = TH;
    val <<= 8;
    val += TL;
    result = (float)val * 0.0625;
    if (temp)
        return result;
    else
        return -result;
}
