#include "ad770x.h"
#include "stm32_spi.h"
#include "stdio.h"

#define  AD7705_GLOBALS
#define MAX_AD_TIME_OUT           ( 100 * 3 )

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

int NewDataFlag;
u8 ChNum = 0;
u16 AD_Rec[ 2 ] = { 0, 0 };
u8 SetupReg;
u8 AD770xStartFlag = 0;

void AD770xDelay( u16 i )
{
	while( i-- );
}

uint16_t AD770xReadWriteByte(u8 data)
{
	return spi_send_byte(SPI1, data, SPI_MODE_REG);
}

void AD770xRegRest(void)
{
	AD770X_ClrCs( );
    AD770xReadWriteByte( 0xff );
	AD770xReadWriteByte( 0xff );
	AD770xReadWriteByte( 0xff );
	AD770xReadWriteByte( 0xff );
	AD770xReadWriteByte( 0xff );
	AD770X_SetCs( );
	AD770xDelay( 500 );
}

void AD770xWriteSetupReg( u8 Data )
{
	AD770X_ClrCs( );
	AD770xReadWriteByte( SETUP_REG + WRITE_DATA + RUN_MODE + ChNum );
	AD770xReadWriteByte( Data );                                    
	AD770X_SetCs( );
	AD770xDelay( 100 );
}

void AD770xWriteClockReg( u8 Data )
{
	AD770X_ClrCs( );
	AD770xReadWriteByte( CLOCK_REG + WRITE_DATA + RUN_MODE + ChNum );
	AD770xReadWriteByte( Data );                                    
	AD770X_SetCs( );
	AD770xDelay( 100 );
}

u16 AD770xReadDataReg( void )
{
	u16 Data = 0;
		
	AD770X_ClrCs( );
	AD770xReadWriteByte( DATA_REG + READ_DATA + RUN_MODE + ChNum );
	Data = AD770xReadWriteByte( 0xff );
	Data <<= 8;
	Data |= AD770xReadWriteByte( 0xff );
	AD770X_SetCs( );
	
	return Data;
}

void AD770xChangeChannel( u8 NewCh )
{
	NewCh &= 0x03;
	AD770X_ClrCs( );
	AD770xReadWriteByte( COM_REG + WRITE_DATA + RUN_MODE + NewCh );
	AD770X_SetCs( );
	AD770xDelay( 100 );
	AD770xWriteSetupReg( SetupReg );
}

void AD770xIoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;    
	
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
	//GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#if 0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
    EXTI_ClearITPendingBit(EXTI4_IRQn);
#endif 

	AD770X_ClrRst( );
	AD770xDelay( 10000 );
	AD770X_SetRst( );
	AD770xDelay( 10000 );
}

int ad770x_init(void)
{	
	AD770xIoInit( );
	AD770xRegRest( );
	
	ChNum = 0;
	
    SetupReg = SetupReg = AUTO_CALI_MODE | GAIN_1 | UNIPOLAR | FSYNC_ON | BUF_IN;
	printf("pre write clock reg\r\n");
	AD770xWriteClockReg( CLOCK_REG_SET );
	printf("pre change channel\r\n");
	AD770xChangeChannel( ChNum );
	
	AD770xStartFlag = 1;
	return 0;
}

int ad770x_read_val(u16 *val)
{
	int flag;

    val[0] = AD_Rec[0];
	val[1] = AD_Rec[1];
	flag = NewDataFlag; 
	NewDataFlag = 0;
	
	return flag;
} 

#if 0
void EXTI4_IRQHandler( void )
{
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line4);
		AD_Rec[ ChNum ] = AD770xReadDataReg( );
		if( ChNum )
			ChNum = 0;
		else
			ChNum = 1;
		NewDataFlag = 1;
		AD770xWriteSetupReg( SetupReg  );
	} 
}
#else 

void ad7705_test(void)
{
	u16 data[2];
	float volt;
	
	while (1) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0) {
			AD_Rec[ ChNum ] = AD770xReadDataReg( );
			if( ChNum ) {
				ChNum = 0;
				volt = (float)(data[0] * (2.5 / 65535));
				printf("ad1 orign data = %d, volt = %.4f\r\n", data[0], volt);
				volt = (float)(data[1] * (2.5 / 65535));
				printf("ad2 orign data = %d, volt = %.4f\r\n", data[1], volt);
			}
			else
				ChNum = 1;
			NewDataFlag = 1;
			AD770xWriteSetupReg( SetupReg  );
		}
	}
}
#endif
