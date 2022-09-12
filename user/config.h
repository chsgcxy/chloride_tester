#ifndef __MY_CONFIG_H__
#define __MY_CONFIG_H__

/* choose one board */
//#define BOARD_TYPE_DEMO
#define BOARD_TYPE_MY

#define SOFTWARE_VERSION_STR    "V3.1.0"

#define ZSB_LEN_DEFAULT         245

#define ADC_TYPE_LTC2400   0
#define ADC_TYPE_AD770X    1

#define ADC_TYPE    ADC_TYPE_LTC2400

#if (ADC_TYPE == ADC_TYPE_LTC2400)
	#define EXPER_ADC_READ    ltc2400_read_data
#elif (ADC_TYPE == ADC_TYPE_AD770X)
	#define EXPER_ADC_READ    ad7705_read
#endif

/* board special config */
#ifdef BOARD_TYPE_DEMO
    #define HSE_VALUE  ((uint32_t)25000000) /*!< Value of the External oscillator in Hz */
	#define UART1_PIN_PA9_PA10
	#define DEBUG_PORT    USART1
	#define PRINTER_PORT    USART1
#elif defined BOARD_TYPE_MY
    #define HSE_VALUE  ((uint32_t)8000000) /*!< Value of the External oscillator in Hz */
	#define UART1_PIN_PB6_pB7

	//#define DEBUG_PORT    USART1
	#define DEBUG_PORT    UART4
	#define PRINTER_PORT    USART1

#else
	#error "must define BOARD_TYPE_XXX......"
#endif

#endif
