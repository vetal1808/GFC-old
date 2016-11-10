#ifndef RADIO_CHANNAL_H_
#define RADIO_CHANNAL_H_

#include "easy_uart.h"
#include "stdlib.h"

#define rx_channal_num 13
#define tx_channal_num 13
#define package_size 6

void set_USARTn(USART_TypeDef* USARTx);
void receive_all_available();
void transmit_masked_channal();
void set_tx_mask(uint32_t val);
uint32_t get_refresh_mask();
void load_tx_buffer(int16_t * p);
void get_rx_buffer(int16_t * p);
void add_to_channal(int16_t val, int16_t channal);

#endif
