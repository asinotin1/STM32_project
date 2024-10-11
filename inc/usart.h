#ifndef __USART_H
#define __USART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define UART_HERCULES							USART1
#define UART_HERCULES_GPIO						GPIOA
#define UART_HERCULES_TX						GPIO_Pin_9
#define UART_HERCULES_RX						GPIO_Pin_10
#define UART_HERCULES_RCC						RCC_APB2Periph_USART1
#define UART_HERCULES_GPIO_RCC					RCC_APB2Periph_GPIOA

void UART1_Config(void);
void USART_SendChar(char data);
void USART_SendString(char *str);
void TimingDelay_Decrement(void);

#endif /* __USART_H */

