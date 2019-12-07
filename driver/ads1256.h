#ifndef _BSP_ADS1256_H
#define _BSP_ADS1256_H

#include "bsp.h"

/* 增益选项 */
typedef enum
{
	ADS1256_GAIN_1			= (0),	/* 增益1（缺省） */
	ADS1256_GAIN_2			= (1),	/* 增益2 */
	ADS1256_GAIN_4			= (2),	/* 增益4 */
	ADS1256_GAIN_8			= (3),	/* 增益8 */
	ADS1256_GAIN_16			= (4),	/* 增益16 */
	ADS1256_GAIN_32			= (5),	/* 增益32 */
	ADS1256_GAIN_64			= (6),	/* 增益64 */	
}ADS1256_GAIN_E;

/* 采样速率选项 */
/* 数据转换率选择
	11110000 = 30,000SPS (default)
	11100000 = 15,000SPS
	11010000 = 7,500SPS
	11000000 = 3,750SPS
	10110000 = 2,000SPS
	10100001 = 1,000SPS
	10010010 = 500SPS
	10000010 = 100SPS
	01110010 = 60SPS
	01100011 = 50SPS
	01010011 = 30SPS
	01000011 = 25SPS
	00110011 = 15SPS
	00100011 = 10SPS
	00010011 = 5SPS
	00000011 = 2.5SPS
*/
typedef enum
{
	ADS1256_30000SPS = 0,
	ADS1256_15000SPS,
	ADS1256_7500SPS,
	ADS1256_3750SPS,
	ADS1256_2000SPS,
	ADS1256_1000SPS,
	ADS1256_500SPS,
	ADS1256_100SPS,
	ADS1256_60SPS,
	ADS1256_50SPS,
	ADS1256_30SPS,
	ADS1256_25SPS,
	ADS1256_15SPS,
	ADS1256_10SPS,
	ADS1256_5SPS,
	ADS1256_2d5SPS,
	
	ADS1256_DRATE_MAX
}ADS1256_DRATE_E;

struct ads1256_drv {
	int (*init)(void);
	void (*pwdn)(int pwdn);
	void (*reset)(int reset);
	int (*drdy)(void);
};

struct ads1256_dev {
	const char *name;
	struct ads1256_drv *drv;
	struct spi_bus *bus;
	ADS1256_GAIN_E gain;
	ADS1256_DRATE_E data_rate;
	int adc[8];
	uint8_t channel;
	uint8_t mode; /* 0: com; 1: diff */
	uint8_t id;
};

extern int ads1256_register(struct ads1256_dev *dev);
extern int ads1256_get_adc(struct ads1256_dev *dev, uint8_t channel);

extern struct ads1256_drv stm32f407_ads1256_driver;

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
