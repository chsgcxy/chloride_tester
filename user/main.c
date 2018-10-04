#include "stm32f2xx_gpio.h"

static void delay_ms(int ms)
{
	int i;
	
	while (ms--)
		for (i = 0; i < 28000; i++);
}

static void gpio_test(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

		while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_10);
		GPIO_SetBits(GPIOC, GPIO_Pin_11);
		GPIO_SetBits(GPIOC, GPIO_Pin_12);
		delay_ms(1000);
			GPIO_ResetBits(GPIOC, GPIO_Pin_10);
		GPIO_ResetBits(GPIOC, GPIO_Pin_11);
		GPIO_ResetBits(GPIOC, GPIO_Pin_12);
			delay_ms(1000);
		}
}

int main(void)
{
	
	gpio_test();
	return 0;
}


