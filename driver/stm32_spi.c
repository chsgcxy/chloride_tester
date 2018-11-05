#include "stm32_spi.h"
#include "stm32f2xx.h"
#include "stdio.h"

int spi1_init(void)
{	
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStruct;  

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
   	  
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);  
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);	
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
    
    return 0;
}

uint16_t spi1_send_byte(uint8_t data)
{
#if 0
	int cnt = 0;
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)) {
		cnt++;	
		if (cnt > 5000000) {
			return 0;
		}
	}
	SPI_I2S_SendData(SPI1, data);
	cnt = 0;
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)) {
		cnt++;	
		if (cnt > 5000000) {
			return 0;
		}
	}
    //printf("\r\n");
	return SPI_I2S_ReceiveData(SPI1);
#else
	u16 retry=0;				 
	while( ( SPI1->SR&1<<1 ) == 0) {
		retry++;
		if( retry > 20000 )   
		{
            return 0;
        }
	}			  
	SPI1->DR = data;
	retry = 0;
	while( ( SPI1->SR&1 << 0 ) == 0 )
	{
		retry++;
		if( retry > 20000 ) 
		{
			return 0;
		}
	}	  						    
	return SPI1->DR;

#endif
}

#define CMD_RDY 0X90
#define CMD_RDX	0XD0

static unsigned char touchdrv_spi_sendb(unsigned char byte)
{
	int cnt = 0;
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) {
		cnt++;	
		if (cnt > 5000000) {
			printf("spi errr1.................\r\n");
			return 0;
		}
	}
	SPI_I2S_SendData(SPI3, byte);
	cnt = 0;
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) {
		cnt++;	
		if (cnt > 5000000) {
			printf("spi errr2.................\r\n");
			return 0;
		}
	}
	return SPI_I2S_ReceiveData(SPI3);
}

static u16 touchdrv_read_ad(u8 cmd)	  
{ 	 
	u16 val = 0;
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
    touchdrv_spi_sendb(cmd); 
	val = touchdrv_spi_sendb(0x00);
	val <<= 8;
	val |= touchdrv_spi_sendb(0x00);
    val = (val >> 4) & 0xfff;
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
	return(val); 
}

void trouchdrv_test(void)
{
	u16 xphy = 0xff;
	u16 yphy = 0xff;

	while (1) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
		xphy = touchdrv_read_ad(CMD_RDX);
		yphy = touchdrv_read_ad(CMD_RDY);
		printf("TOUCH: xphy=%d, yphy=%d.\r\n", xphy, yphy);
        }
	}
}

int spi3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    //GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);  
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);	
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3, ENABLE);

    trouchdrv_test();
    return 0;
}

