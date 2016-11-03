#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "easy_uart.h"
#include "helpers.h"
#include "stdlib.h"

void set_USARTn(USART_TypeDef* USARTx);
void receive_all_available();
void transmit_masked_channal();
void set_mask(uint32_t val);
void load_tx_buffer(int16_t * p);
void get_rx_buffer(int16_t * p);

#endif
