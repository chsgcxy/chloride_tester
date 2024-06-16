#include "ltc2400.h"
#include "delay.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sysconf.h"

#define		BIT0                (0x01)
#define		BIT1                (0x02)
#define		BIT2                (0x04)
#define		BIT3                (0x08)
#define		BIT4                (0x10)
#define		BIT5                (0x20)
#define		BIT6                (0x40)
#define		BIT7                (0x80)

#define LTC2400_CS_LOW          GPIO_ResetBits(GPIOA, GPIO_Pin_2)
#define LTC2400_CS_HIGH         GPIO_SetBits(GPIOA, GPIO_Pin_2)
#define LTC2400_RDY_STATE       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)
#define SCLOCK1                 GPIO_SetBits(GPIOA, GPIO_Pin_6)
#define SCLOCK0                 GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define CS1                     LTC2400_CS_HIGH
#define CS0                     LTC2400_CS_LOW
#define MOSI1                   GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define MOSI0                   GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define MISO                    LTC2400_RDY_STATE
#define LTC2400_DELAY           delay_ms(1)

//LTC2400--相关变量的声明
uint32_t LTC2400_24BitADC = 0;


static volatile float volt_base = 0.0;

void ltc2400_volt_base_clear(void)
{
	volt_base = 0.0;
}

void ltc2400_volt_base_set(float volt)
{
	volt_base = volt;
}


void ltc2400_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	struct sysconf *conf;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
	/* CS  MOSI  CLK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* DRDY/MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    LTC2400_CS_HIGH;
    SCLOCK1;

	conf = sysconf_get();
	if (conf->djdw_valid == DJDW_VALID_FLAG)
		volt_base = conf->djdw_val;
	else
	{
		/* code */
		volt_base = 0.0;
	}

}

/************************************************************************* 
** 函数名称:	signed	long	LTC240_Read_3Byte(void)
** 工作环境: 	STC90C516RD+【主时钟11.0592MHZ】
** 作    者:
** 生成日期: 	2020.03.25
** 功    能:	主控器接收从器件方数据[三字节]
** 入口参数:	无
** 出口参数:	读取的数据
** 备    注:	实际使用相当于20Bit使用
** 相关文件: 
** 修改日志: 	2014.10.05 2020.03.22
*************************************************************************/
float ltc2400_read_data(void)
{
	unsigned	char	LTC2400_RxDataCn = 0;
	unsigned	char	LTC2400_RxDataChar = 0;
	float volt = 0.0;

	uint32_t LTC2400_OutData;

rerun:

	LTC2400_OutData = 0;
	
	SCLOCK0;
	CS0;
	LTC2400_DELAY;

	//Bit31--EOC,Bit30--DMY,Bit29--SIG,Bit28--EXR,Bit27--MSB .... Bit4--LSB24

	//采集LTC2400的EOC、DMY、SIG、EXR【Bit31,Bit30,Bit29,Bit28,】
	for (LTC2400_RxDataCn = 0; LTC2400_RxDataCn < 4; LTC2400_RxDataCn++) {
		LTC2400_RxDataChar <<= 1;
		SCLOCK1;
		LTC2400_DELAY;
		if (MISO && BIT0)
			LTC2400_RxDataChar |= 1;
		SCLOCK0;
		LTC2400_DELAY;
	}

	//采集LTC2400的转换结果的24位数据【有效数据】
	for (LTC2400_RxDataCn = 0; LTC2400_RxDataCn < 24; LTC2400_RxDataCn++) {
		LTC2400_OutData <<= 1;

		SCLOCK1;
		LTC2400_DELAY;
		if (MISO && BIT0)
			LTC2400_OutData |= 1;
		SCLOCK0;
		LTC2400_DELAY;
	}
	
	//采集LTC2400的转换结果的最后几位数据【无效数据】
	for (LTC2400_RxDataCn = 0; LTC2400_RxDataCn < 4; LTC2400_RxDataCn++) {
		SCLOCK1;
		LTC2400_DELAY;
		SCLOCK0;
		LTC2400_DELAY;
	}
	
	CS1;

	if ((LTC2400_RxDataChar & 0x3) == 0x3) {
		LTC2400_OutData = 0x00ffffff;
		printf("vin > Vref\r\n");
		vTaskDelay(180);
		// delay_ms(200);
		goto rerun;
	} else if ((LTC2400_RxDataChar & 0x1) == 0x1) {
		printf("vin < 0\r\n");
		vTaskDelay(180);
		// delay_ms(200);
		goto rerun;
	} else if ((LTC2400_RxDataChar & 0x2) == 0x2) {
		;
		//printf("0 < vin < Vref\r\n");
	} else
		LTC2400_OutData = 0;

	LTC2400_OutData >>= 4;
	volt = (float)LTC2400_OutData / (float)0x100000;
	volt *= 2500.0;
	volt -= 1250.0;
	volt *= 2.0;
	volt = volt - volt_base;
	return volt;
}
