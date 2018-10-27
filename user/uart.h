#ifndef __UART_H__
#define __UART_H__

extern int uart1_init(void);
extern int uart4_init(void);
extern int uart_send_buf(USART_TypeDef* USARTx, uint8_t *buf, int len);

#endif
