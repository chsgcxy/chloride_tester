#ifndef __STM32_UART_H__
#define __STM32_UART_H__

extern int uart1_init(void);
extern int uart4_init(void);
extern int uart_send_buf(USART_TypeDef* USARTx, uint8_t *buf, int len);
extern int uart_get_status(void);

#endif
