#include "stepmotor.h"
#include "delay.h"
#include "stdio.h"

/*  DIR:  PC7
 *  STEP: PC6
 *  上限位： PC2
 *  下限位： PC3
*/
int stepmotor_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    return 0;
}

void stepmotor_run(int dir, int step)
{   
    if (MOTOR_DIR_UP == dir)
        GPIO_SetBits(GPIOC, GPIO_Pin_7);
    else if (MOTOR_DIR_DOWN == dir)
        GPIO_ResetBits(GPIOC, GPIO_Pin_7);
    else
        return;

    delay_us(200);

    while (step) {
        if (MOTOR_DIR_UP == dir) {
            if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3))
                return;
        } else if (MOTOR_DIR_DOWN == dir) {
            if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2))
                return;
        } else
            return;

        if (step > 0)
            step--;
            
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        delay_us(200);
        GPIO_SetBits(GPIOC, GPIO_Pin_6);
        delay_us(200);
    }
}

void relay_ctrl(int ctrl)
{
    if (ctrl) {
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
        GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    } else {
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
        GPIO_SetBits(GPIOC, GPIO_Pin_1);
    }
}
