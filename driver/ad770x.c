#include "ad770x.h"
#include "stm32_spi.h"
#include "stdio.h"
#include "sysconf.h"

#define  AD7705_GLOBALS
#define MAX_AD_TIME_OUT           (100 * 3)

#define COM_REG     0x00
#define SETUP_REG   0x10
#define CLOCK_REG   0x20
#define DATA_REG    0x30
#define TEST_REG    0x40
#define OFFSET_REG  0x60
#define GAIN_REG    0x70

#define WRITE_DATA    0x00
#define READ_DATA     0x08

#define WAIT_MODE   0x04
#define RUN_MODE    0x00

#define AD_CH1      0x00  //AIN1(+)  AIN1(-)
#define AD_CH2      0x01  //AIN2(+)  AIN2(-)
#define AD_CH3      0x02  //AIN1(-)  AIN1(-)
#define AD_CH4      0x03  //AIN2(-)  AIN2(-)

#define NORMAL_MODE     0x00
#define AUTO_CALI_MODE  0x40
#define ZERO_CALI_MODE  0x80
#define FULL_CALI_MPDE  0xC0

#define GAIN_1          0x00
#define GAIN_2          0x08
#define GAIN_4          0x10
#define GAIN_8          0x18
#define GAIN_16         0x20
#define GAIN_32         0x28
#define GAIN_64         0x30
#define GAIN_128        0x38

#define UNIPOLAR        0x04
#define BIPOLAR         0x00 

#define BUF_NONE        0x00
#define BUF_IN          0x02

//FSYNC
#define FSYNC_ON        0x00
#define FSYNC_OFF       0x01

#define CLOCK_REG_SET  0x01

#define AD770X_ClrCs( )        GPIO_ResetBits(GPIOA, GPIO_Pin_2) 	
#define AD770X_SetCs( )        GPIO_SetBits(GPIOA, GPIO_Pin_2)	  

#define AD770X_ClrRst( )       GPIO_ResetBits(GPIOA, GPIO_Pin_3) 	
#define AD770X_SetRst( )       GPIO_SetBits(GPIOA, GPIO_Pin_3)	

#define AD770X_ReadRdyStu( )   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) 

static uint8_t setup_reg;

static volatile float volt_base = 0.0;

void volt_base_clear(void)
{
	volt_base = 0.0;
}

void volt_base_set(float volt)
{
	volt_base = volt;
}

void AD770xDelay(u16 i)
{
	while(i--);
}

uint16_t AD770xReadWriteByte(u8 data)
{
	return spi_send_byte(SPI1, data, SPI_MODE_REG);
}

void AD770xRegRest(void)
{
	AD770X_ClrCs();
    AD770xReadWriteByte(0xff);
	AD770xReadWriteByte(0xff);
	AD770xReadWriteByte(0xff);
	AD770xReadWriteByte(0xff);
	AD770xReadWriteByte(0xff);
	AD770X_SetCs();
	AD770xDelay(500);
}

void AD770xWriteSetupReg(u8 Data)
{
	AD770X_ClrCs();
	AD770xReadWriteByte(SETUP_REG + WRITE_DATA + RUN_MODE + 0);
	AD770xReadWriteByte(Data);                                    
	AD770X_SetCs();
	AD770xDelay(100);
}

void AD770xWriteClockReg(u8 Data)
{
	AD770X_ClrCs();
	AD770xReadWriteByte(CLOCK_REG + WRITE_DATA + RUN_MODE + 0);
	AD770xReadWriteByte(Data);                                    
	AD770X_SetCs();
	AD770xDelay(100);
}

u16 AD770xReadDataReg(void)
{
	u16 Data = 0;
		
	AD770X_ClrCs();
	AD770xReadWriteByte(DATA_REG + READ_DATA + RUN_MODE + 0);
	Data = AD770xReadWriteByte(0xff);
	Data <<= 8;
	Data |= AD770xReadWriteByte(0xff);
	AD770X_SetCs();
	
	return Data;
}

void AD770xChangeChannel(u8 NewCh)
{
	NewCh &= 0x03;
	AD770X_ClrCs();
	AD770xReadWriteByte(COM_REG + WRITE_DATA + RUN_MODE + NewCh);
	AD770X_SetCs();
	AD770xDelay(100);
	AD770xWriteSetupReg(setup_reg);
}

void AD770xIoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
	/* CS   RST */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* DRDY */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

	AD770X_ClrRst();
	AD770xDelay(10000);
	AD770X_SetRst();
	AD770xDelay(10000);
}

int ad770x_init(void)
{	
	struct sysconf *conf = sysconf_get();
	setup_reg = AUTO_CALI_MODE | GAIN_1 | UNIPOLAR | FSYNC_ON | BUF_NONE;

	AD770xIoInit();
	AD770xRegRest();
	AD770xWriteClockReg(CLOCK_REG_SET);
	AD770xChangeChannel(0);

	if (conf->djdw_valid == DJDW_VALID_FLAG)
		volt_base = conf->djdw_val;
	else
	{
		/* code */
		volt_base = 0.0;
	}
	
	return 0;
}

float ad7705_read(void)
{
	u16 data;
	float volt;
	
	while (1) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) {
			data = AD770xReadDataReg();
			//volt = ((float)data * 0.0762940) - 1250.0;
			volt = (float)((float)data * ((float)2500.0 / (float)65535.0)) - (float)204.0;
			//printf("volt = %f, base = %f\r\n", volt, volt_base);
			volt = volt - volt_base;
			AD770xWriteSetupReg(setup_reg);
			return volt;
		}
	}
}
