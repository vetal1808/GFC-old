#ifndef __UASY_UART_H
#define __UASY_UART_H

#include "stdint.h"

/*
 * Tx - PA.09
 * Rx - PA.10
 */

uint16_t USART1_available(void);
uint8_t USART1_read(uint8_t * str, uint8_t len);
uint8_t USART1_readLine(uint8_t * str, uint8_t len);
void usart_init();
void UARTSend(uint8_t *pucBuffer, uint8_t ulCount);
#endif
