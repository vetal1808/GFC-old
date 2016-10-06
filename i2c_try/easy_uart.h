#ifndef __UASY_UART_H
#define __UASY_UART_H

#include "stdint.h"
#include "stm32f10x_usart.h"
/*
 * USART1
 * Tx - PA.09
 * Rx - PA.10
 *
 * USART2
 * Tx - PA.02
 * Rx - PA.03
 */

uint16_t USART_available(USART_TypeDef* USARTx);
uint8_t USART_read(USART_TypeDef* USARTx, uint8_t * str, uint8_t len);
uint8_t USART_readLine(USART_TypeDef* USARTx, uint8_t * str, uint8_t len);
void USART_init(USART_TypeDef* USARTx, uint32_t baud_rate);
void USART_send(USART_TypeDef* USARTx, uint8_t *pucBuffer, uint8_t ulCount);
void USART_send_str(USART_TypeDef* USARTx, const uint8_t *pucBuffer, uint8_t ulCount);
uint8_t USART_line_available(USART_TypeDef* USARTx);
#endif
