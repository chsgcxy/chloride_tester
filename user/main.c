#include "stm32f2xx_gpio.h"
#include "stdio.h"


extern int uart1_init(void);

static void delay_ms(int ms)
{
	int i;
	
	while (ms--)
		for (i = 0; i < 28000; i++);
}

static int led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	return 0;
}

int main(void)
{
	uart1_init();
	gpio_init();

	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
		printf("led on\r\b");
		delay_ms(1000);
		GPIO_ResetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);
		delay_ms(1000);
		printf("led off\r\n");
	}
	return 0;
}


