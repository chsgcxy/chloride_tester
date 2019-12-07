
#include "ads1256.h"
#include "delay.h"

/**
 * ���ӷ�ʽ
 * +5V   <------  5.0V      5V����
 * GND   -------  GND       ��
 * DRDY  ------>  ***       ׼������
 * CS    <------  ***       SPI_CS
 * DIN   <------  ***       SPI_MOSI
 * DOUT  ------>  ***       SPI_MISO
 * SCLK  <------  ***       SPIʱ��
 * GND   -------  GND       ��
 * PDWN  <------  ***       �������
 * RST   <------  ***       ��λ�ź�
 * NC   �ս�
 * NC   �ս�
 */

/**
 * ADS1256��������:
 * 1��ģ�ⲿ�ֹ���5V;
 * 2��SPI���ֽӿڵ�ƽ��3.3V
 * 3��PGA���÷�Χ�� 1��2��4��8��16��32��64��
 * 4���ο���ѹ2.5V (�Ƽ�ȱʡ�ģ����õģ�
 * 5�������ѹ��Χ��PGA = 1 ʱ, ����������5V
 * 6. �Զ�У׼ ����������PGA,BUFʹ�ܡ����ݲ�����ʱ����������У׼)
 * 7. ����Ļ��������������ú͹رգ�һ��ѡ���ã�
 * 
 * �ⲿ����Ƶ�� = 7.68MHz, 
 * ʱ��Ƶ�� tCLK = 1/7.68M = 0.13uS
 * ����������� tDATA =  1 / 30K = 0.033mS  (��30Ksps����)
 * 
 * SPI��ʱ���ٶ�Ҫ��: (ads1256.pdf page 6)
 * 		��� 4��tCLK = 0.52uS
 * 		���� 10��tDATA = 0.3mS (�� 30Ksps ����)
 * 		SCL�ߵ�ƽ�͵͵�ƽ����ʱ����С 200ns
 * 
 * RREG, WREG, RDATA ����֮����Ҫ�ӳ� 4 * tCLK = 0.52uS;
 * RDATAC, RESET, SYNC ����֮����Ҫ�ӳ� 24 * tCLK = 3.12uS;
 * 
 * ʵ�ʲ��ԣ���3.3V�ϵ��, ��ʱ�����κ����ã�
 * ADS125��DRDY ���߼���ʼ��������źţ�2.6us��,33.4�ͣ�Ƶ��30KHz��
 */

/**
 * ���Լ�¼
 * (1) ���üĴ���ʱ��SCK���쵼��оƬ����ÿ�ζ��յ����ݡ�
 * 		ԭ��: ���͵����ڵ��ֽ�֮����Ҫ�ӳ�һС��ʱ��.
 * (2) ������λCPUʱ��ż������оƬ����������쳣��
*/

/* �Ĵ������壺 Table 23. Register Map --- ADS1256�����ֲ��30ҳ */
enum {
	/* �Ĵ�����ַ�� �����Ǹ�λ��ȱʡֵ */
	REG_STATUS = 0,	// x1H
	REG_MUX    = 1, // 01H
	REG_ADCON  = 2, // 20H
	REG_DRATE  = 3, // F0H
	REG_IO     = 4, // E0H
	REG_OFC0   = 5, // xxH
	REG_OFC1   = 6, // xxH
	REG_OFC2   = 7, // xxH
	REG_FSC0   = 8, // xxH
	REG_FSC1   = 9, // xxH
	REG_FSC2   = 10, // xxH
};

/* ����壺 TTable 24. Command Definitions --- ADS1256�����ֲ��34ҳ */
enum {
	CMD_WAKEUP  = 0x00,	// Completes SYNC and Exits Standby Mode 0000  0000 (00h)
	CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
	CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
	CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
	CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
	CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
	CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
	CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
	CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
	CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
	CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
	CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
	CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
	CMD_RESET   = 0xFE, // Reset to Power-Up Values 1111   1110 (FEh)
};

static const uint8_t s_tabDataRate[ADS1256_DRATE_MAX] = 
{
	0xF0,		/* ��λʱȱʡֵ */
	0xE0,
	0xD0,
	0xC0,
	0xB0,
	0xA1,
	0x92,
	0x82,
	0x72,
	0x63,
	0x53,
	0x43,
	0x33,
	0x20,
	0x13,
	0x03
};

static void ads1256_reset(struct ads1256_dev *dev)
{
	dev->drv->reset(0);
	delay_ms(5);
	dev->drv->reset(1);
	delay_ms(5);
}

static void ads1256_write_reg(struct ads1256_dev *dev, uint8_t reg, uint8_t val)
{
	struct spi_bus *bus = dev->bus;

	bus->cs_ctrl(0);
	bus->send_byte(CMD_WREG | reg);
	bus->send_byte(0x00);
	bus->send_byte(val);
	bus->cs_ctrl(0);
}

static uint8_t ads1256_read_reg(struct ads1256_dev *dev, uint8_t reg)
{
	struct spi_bus *bus = dev->bus;
	uint8_t val;

	bus->cs_ctrl(0);
	bus->send_byte(CMD_RREG | reg);
	bus->send_byte(0x00);
	delay_ms(10);
	val = bus->send_byte(0x00);
	bus->cs_ctrl(1);

	return val;
}

static void ads1256_write_cmd(struct ads1256_dev *dev, uint8_t cmd)
{
	struct spi_bus *bus = dev->bus;

	bus->cs_ctrl(0);
	bus->send_byte(cmd);
	bus->cs_ctrl(1);
}

static void ads1256_set_channel(struct ads1256_dev *dev, uint8_t channel)
{
	/*
	Bits 7-4 PSEL3, PSEL2, PSEL1, PSEL0: Positive Input Channel (AINP) Select
		0000 = AIN0 (default)
		0001 = AIN1
		0010 = AIN2 (ADS1256 only)
		0011 = AIN3 (ADS1256 only)
		0100 = AIN4 (ADS1256 only)
		0101 = AIN5 (ADS1256 only)
		0110 = AIN6 (ADS1256 only)
		0111 = AIN7 (ADS1256 only)
		1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are ��don��t care��)

		NOTE: When using an ADS1255 make sure to only select the available inputs.

	Bits 3-0 NSEL3, NSEL2, NSEL1, NSEL0: Negative Input Channel (AINN)Select
		0000 = AIN0
		0001 = AIN1 (default)
		0010 = AIN2 (ADS1256 only)
		0011 = AIN3 (ADS1256 only)
		0100 = AIN4 (ADS1256 only)
		0101 = AIN5 (ADS1256 only)
		0110 = AIN6 (ADS1256 only)
		0111 = AIN7 (ADS1256 only)
		1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are ��don��t care��)
	*/
	if (channel > 7)
		return;

	/* don't care NSEL */
	ads1256_write_reg(dev, REG_MUX, (channel << 4) | (1 << 3));
	dev->channel = channel;
	dev->mode = 0;
}

static void ads1256_set_channel_diff(struct ads1256_dev *dev, uint8_t diff_channel)
{
	/*
	Bits 7-4 PSEL3, PSEL2, PSEL1, PSEL0: Positive Input Channel (AINP) Select
		0000 = AIN0 (default)
		0001 = AIN1
		0010 = AIN2 (ADS1256 only)
		0011 = AIN3 (ADS1256 only)
		0100 = AIN4 (ADS1256 only)
		0101 = AIN5 (ADS1256 only)
		0110 = AIN6 (ADS1256 only)
		0111 = AIN7 (ADS1256 only)
		1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are ��don��t care��)

		NOTE: When using an ADS1255 make sure to only select the available inputs.

	Bits 3-0 NSEL3, NSEL2, NSEL1, NSEL0: Negative Input Channel (AINN)Select
		0000 = AIN0
		0001 = AIN1 (default)
		0010 = AIN2 (ADS1256 only)
		0011 = AIN3 (ADS1256 only)
		0100 = AIN4 (ADS1256 only)
		0101 = AIN5 (ADS1256 only)
		0110 = AIN6 (ADS1256 only)
		0111 = AIN7 (ADS1256 only)
		1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are ��don��t care��)
	*/
	if (diff_channel > 3)
		return;

	/**
	 * chan                func
	 * 0         AIN0 Positive,  AIN1 Negative
	 * 1         AIN2 Positive,  AIN3 Negative
	 * 2         AIN4 Positive,  AIN5 Negative
	 * 3         AIN6 Positive,  AIN7 Negative
	*/
	ads1256_write_reg(dev, REG_MUX, (diff_channel << 5) | ((diff_channel << 1) + 1));
	dev->channel = diff_channel;
	dev->mode = 1;
}

static void ads1256_wait_drdy(struct ads1256_dev *dev)
{
	uint32_t i;

	for (i = 0; i < 40000000; i++) {
		if (dev->drv->drdy())
			break;
	}

	if (i >= 40000000)
		BSP_ERR("%s:waitting for drdy overtime\r\n", __FUNCTION__);
}
   	
static int ads1256_read_data(struct ads1256_dev *dev)
{
	int val = 0;
	struct spi_bus *bus = dev->bus;

	bus->cs_ctrl(0);
	bus->send_byte(CMD_RDATA);
	delay_ms(5);
	val = bus->send_byte(0x00) << 16;
	val |= bus->send_byte(0x00) << 8;
	val |= bus->send_byte(0x00) << 0;
	bus->cs_ctrl(1);
	
	/* ����������չ��24λ�з�������չΪ32λ�з����� */
	//if (val & 0x800000)
	//	val |= 0xFF000000;

	if (val > 0x7FFFFF)           // if MSB=1, 
		val -= 0x1000000;       // do 2's complement

	return val;
}

int ads1256_get_adc(struct ads1256_dev *dev, uint8_t channel)
{
	/* ADS1256 �����ֲ��21ҳ */
	ads1256_wait_drdy(dev);
	if (dev->mode)
		ads1256_set_channel_diff(dev, channel);
	else
		ads1256_set_channel(dev, channel);
	delay_ms(5);
	
	ads1256_write_cmd(dev, CMD_SYNC);
	delay_ms(5);
	
	ads1256_write_cmd(dev, CMD_WAKEUP);
	delay_ms(25);
	
	dev->adc[dev->channel] = ads1256_read_data(dev);
	return dev->adc[dev->channel];	
}

int ads1256_register(struct ads1256_dev *dev)
{
	uint8_t id;

	if (!dev) {
		BSP_ERR("%s: null device, register fail\r\n", __FUNCTION__);
		return -1;
	}

	if (!dev->bus) {
		BSP_ERR("%s: null bus, register fail\r\n", __FUNCTION__);
		return -1;
	}

	if (!dev->drv) {
		BSP_ERR("%s: null driver, register fail\r\n", __FUNCTION__);
		return -1;
	}

	dev->bus->init();
	dev->drv->init();

	dev->drv->reset(1);
	dev->drv->pwdn(1);
	dev->bus->cs_ctrl(1);

	ads1256_wait_drdy(dev);
	id = ads1256_read_reg(dev, REG_STATUS);
	dev->id = id >> 4;
	dev->mode = 1;
	dev->channel = 0;
	BSP_INFO("get ads1256 id 0x%x\r\n", dev->id);

   	ads1256_wait_drdy(dev);
	ads1256_write_cmd(dev, CMD_SELFCAL);
	ads1256_wait_drdy(dev);
	
	ads1256_write_reg(dev, REG_STATUS, 0x06);               // ��λ��ǰ��ʹ�û���
	ads1256_write_reg(dev, REG_MUX, 0x08);
	ads1256_write_reg(dev, REG_ADCON, ADS1256_GAIN_1);                // �Ŵ���1
	ads1256_write_reg(dev, REG_DRATE, s_tabDataRate[ADS1256_10SPS]);  // ����10sps
	ads1256_write_reg(dev, REG_IO, 0x00);
	delay_ms(50);
    return 0;
}

/****************************device driver*****************************************/

static int stm32f407_ads1256_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	return 0;
}

static void stm32f407_ads1256_pwdn(int pwdn)
{
	if (pwdn)
		GPIO_SetBits(GPIOF, GPIO_Pin_6);
	else
		GPIO_ResetBits(GPIOF, GPIO_Pin_6);
}

static void stm32f407_ads1256_reset(int reset)
{
	//if (reset)
	//	GPIO_SetBits(GPIOB, GPIO_Pin_11);
	//else
	//	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}

static int stm32f407_ads1256_drdy(void)
{
	return (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8) == Bit_RESET);
}

struct ads1256_drv stm32f407_ads1256_driver = {
	stm32f407_ads1256_init,
	stm32f407_ads1256_pwdn,
	stm32f407_ads1256_reset,
	stm32f407_ads1256_drdy
};
