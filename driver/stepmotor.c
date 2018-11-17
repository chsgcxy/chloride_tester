#include "stepmotor.h"
#include "delay.h"

/*  DIR:  PC7
 *  STEP: PC6
*/
int stepmotor_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7);
    return 0;
}

void stepmotor_run(int dir, int step)
{   
    if (dir)
        GPIO_SetBits(GPIOC, GPIO_Pin_7);
    else
        GPIO_ResetBits(GPIOC, GPIO_Pin_7);
    delay_us(10);

    while (step--) {
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        delay_us(10);
        GPIO_SetBits(GPIOC, GPIO_Pin_6);
        delay_us(10);
    }
}
