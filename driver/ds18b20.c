#include "ds18b20.h"
#include "stm32f2xx.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"

#define CFG_IN     GPIO_Mode_IN   
#define CFG_OUT    GPIO_Mode_OUT   
  
static void ds18b20_gpio_cfg(GPIOMode_TypeDef mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void ds18b20_gpio_set(uint8_t val)
{
    if (val)
        GPIO_SetBits(GPIOC, GPIO_Pin_4);
    else
        GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

static uint8_t ds18b20_gpio_get(void)
{
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
}

static int ds18b20_init(void)  
{  
    int res = 500;
  
    ds18b20_gpio_cfg(CFG_OUT);
    ds18b20_gpio_set(1);
    delay_us(100);
    ds18b20_gpio_set(0);
    delay_us(600);  
    ds18b20_gpio_set(1);
  
    ds18b20_gpio_cfg(CFG_IN);  
    while (ds18b20_gpio_get() && res--);

    return res;
}

int ds18b20_open(void)  
{  
    int flag = 0;  

    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_4);
  
    flag = ds18b20_init();  
    if (flag == 0) {  
        printf("open ds18b20 failed\r\n");  
        return -1;
    }  
    printf("open ds18b20 successful\r\n");  
    return 0;  
}  

static void write_byte(unsigned char data)  
{  
    int i = 0;  
  
    ds18b20_gpio_cfg(CFG_OUT);
  
    for (i = 0; i < 8; i++) {  
        ds18b20_gpio_set(0);  
        delay_us(1);
        ds18b20_gpio_set(data & 0x01);  
        delay_us(80);
        ds18b20_gpio_set(1);
        delay_us(15);
        data >>= 1;
    }  
    ds18b20_gpio_set(1);
}  
  
static unsigned char read_byte(void)  
{  
    int i;  
    unsigned char data = 0;  
  
    for (i = 0; i < 8; i++) {
        ds18b20_gpio_cfg(CFG_OUT);  
        ds18b20_gpio_set(0);
        delay_us(1);
        ds18b20_gpio_set(1);
        ds18b20_gpio_cfg(CFG_IN);
        delay_us(4);
        data >>= 1;
        if (ds18b20_gpio_get())  
            data |= 0x80;  
        delay_us(60);
    }
    return data;  
}  
  
float ds18b20_get_temp(void)  
{  
    int flag;
    unsigned char result[2] = { 0x00, 0x00 }; 
    int val;
    float temp = 0;
  
    flag = ds18b20_init();  
    if (flag == 0) {  
        printf("ds18b20 init failed\r\n");  
        return 0;
    }  
  
    delay_us(420);
    write_byte(0xcc);  
    write_byte(0x44);  
    delay_us(800);

    flag = ds18b20_init();  
    if (flag == 0)  
        return 0;  
    delay_us(400);
    write_byte(0xcc);  
    write_byte(0xbe);  
  
    result[0] = read_byte();
    result[1] = read_byte();
  
    val = result[1];
    val <<= 8;
    val |= result[0];
    temp = (float)val * 0.0625;
   
    return temp;
}  
