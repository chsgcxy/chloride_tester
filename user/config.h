#ifndef __MY_CONFIG_H__
#define __MY_CONFIG_H__

/* choose one board */
//#define BOARD_TYPE_DEMO
#define BOARD_TYPE_MY


/* board special config */
#ifdef BOARD_TYPE_DEMO
    #define HSE_VALUE  ((uint32_t)25000000) /*!< Value of the External oscillator in Hz */
	#define UART1_PIN_PA9_PA10
	#define DEBUG_PORT    USART1
#elif defined BOARD_TYPE_MY
    #define HSE_VALUE  ((uint32_t)8000000) /*!< Value of the External oscillator in Hz */
	#define UART1_PIN_PB6_pB7
	#define DEBUG_PORT    USART1
#else
	#error "must define BOARD_TYPE_XXX......"
#endif

#endif
