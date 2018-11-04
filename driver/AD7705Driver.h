
#ifndef AD7705_H__
#define AD7705_H__

#ifdef   AD7705_GLOBALS
#define  AD7705_EXT
#else
#define  AD7705_EXT  extern
#endif

#include "stm32f2xx.h"

/******************************************************************************/
/*ͨѶ�Ĵ�������*/

//�¸���д����Ҫ��д�����ݼĴ���
#define COM_REG     0x00  //ͨѶ�Ĵ���
#define SETUP_REG   0x10  //���üĴ���
#define CLOCK_REG   0x20  //ʱ�ӼĴ���
#define DATA_REG    0x30  //���ݼĴ���
#define TEST_REG    0x40  //���ԼĴ���
#define OFFSET_REG  0x60  //ƫ�ƼĴ���
#define GAIN_REG    0x70  //����Ĵ���

//��д����
#define WRITE_DATA    0x00 //д�Ĵ���
#define READ_DATA     0x08 //���Ĵ���   1Ϊ��  0Ϊд

//ģʽѡ��
#define WAIT_MODE   0x04  //��ȴ�ģʽ��
#define RUN_MODE    0x00  //��������ģʽ

//ͨ��ѡ��
#define AD_CH1      0x00  //AIN1(+)  AIN1(-)
#define AD_CH2      0x01  //AIN2(+)  AIN2(-)
#define AD_CH3      0x02  //AIN1(-)  AIN1(-)
#define AD_CH4      0x03  //AIN2(-)  AIN2(-)

/******************************************************************************/
/* ���üĴ��� */

//����ģʽ
#define NORMAL_MODE     0x00  //����ģʽ
#define AUTO_CALI_MODE  0x40  //��У׼ģʽ
#define ZERO_CALI_MODE  0x80  //����У׼
#define FULL_CALI_MPDE  0xC0  //�����У׼

//����
#define GAIN_1          0x00  //���� x1
#define GAIN_2          0x08  //���� x2
#define GAIN_4          0x10  //���� x4
#define GAIN_8          0x18  //���� x8
#define GAIN_16         0x20  //���� x16
#define GAIN_32         0x28  //���� x32
#define GAIN_64         0x30  //���� x64
#define GAIN_128        0x38  //���� x128

//������ ˫���Թ���
#define UNIPOLAR        0x04  //�����Թ���
#define BIPOLAR         0x00  //˫���Թ���

//������
#define BUF_NONE        0x00  //Ƭ�ڻ�������·
#define BUF_IN          0x02  //�����ڲ�������

//FSYNC
#define FSYNC_ON        0x00  //Ƭ���˲�����
#define FSYNC_OFF       0x01  //Ƭ���˲����ر�


/******************************************************************************/
/* ʱ�ӼĴ��� */

#define CLOCK_REG_SET  0x0C  //ת��Ƶ��100HZ

/******************************************************************************/

#define AD770X_ClrCs( )        GPIO_ResetBits(GPIOA, GPIO_Pin_2) 	
#define AD770X_SetCs( )        GPIO_SetBits(GPIOA, GPIO_Pin_2)	  

#define AD770X_ClrRst( )       GPIO_ResetBits(GPIOA, GPIO_Pin_3) 	
#define AD770X_SetRst( )       GPIO_SetBits(GPIOA, GPIO_Pin_3)	

#define AD770X_ReadRdyStu( )   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) 


AD7705_EXT void AD770xInit(void);
AD7705_EXT u8 AD770xReadValue( u16 *pValue );

#endif




















