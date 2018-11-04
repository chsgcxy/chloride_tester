#include"AD7705Driver.h"
#include"spi.h"
#include "stdio.h"

#define  AD7705_GLOBALS
#define MAX_AD_TIME_OUT           ( 100 * 3 )

u8 NewDataFlag;
u8 ChNum = 0;
u16 AD_Rec[ 2 ] = { 0, 0 };

u8 SetupReg;

u8 AD770xStartFlag = 0;

void AD770xDelay( u16 i )
{
	while( i-- );
}

u8 AD770xReadWriteByte( u8 WriteData )
{
	return spi1_data_rw( WriteData );
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
	AD770xReadWriteByte( DATA_REG + READ_DATA + RUN_MODE + ChNum ); //дͨѶ�Ĵ���
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

void AD770xIoInit( void )
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
	//GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  
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

void AD770xInit( void )
{	
	printf("ad770x_init\r\n");
	spi1_init( );
	AD770xIoInit( );
	
	AD770xRegRest( );
	
	ChNum = 0;
	
    SetupReg = 0x40;
	printf("pre write clock reg\r\n");
	AD770xWriteClockReg( CLOCK_REG_SET );
	printf("pre change channel\r\n");
	AD770xChangeChannel( ChNum );
	
	AD770xStartFlag = 1;
}

u8 AD770xReadValue( u16 *pValue )
{
	u8 Flag = 0;
	
    pValue[ 0 ] = AD_Rec[ 0 ];
	pValue[ 1 ] = AD_Rec[ 1 ];
	Flag = NewDataFlag; 
	NewDataFlag = 0;
	
	return Flag;
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
			if( ChNum )
				ChNum = 0;
			else
				ChNum = 1;
			NewDataFlag = 1;
			AD770xWriteSetupReg( SetupReg  );

			volt = (float)(data[0] * (2.5 / 65535));
			printf("ad1 orign data = %d, volt = %.4f\r\n", data[0], volt);
			volt = (float)(data[1] * (2.5 / 65535));
			printf("ad2 orign data = %d, volt = %.4f\r\n", data[1], volt);
		}
	}
}
#endif

