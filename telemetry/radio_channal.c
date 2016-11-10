#include "radio_channal.h"

USART_TypeDef * USART;

volatile int16_t rx_channal[rx_channal_num];
volatile int16_t tx_channal[tx_channal_num];
int16_t tx_mask = 0;

void set_USARTn(USART_TypeDef* USARTx){
	USART = USARTx;
	USART_init(USARTx, 921600);
}

void receive_all_available(){
	if(USART_line_available(USART)){
		uint8_t tmp_buff[package_size+2];
		USART_readLine(USART, tmp_buff, package_size+2);
		tmp_buff[0] -= 'A';
		tmp_buff[package_size+1] = 0;
		if(tmp_buff[0]<(rx_channal_num)){
			rx_channal[tmp_buff[0]] = atoi(tmp_buff+1);
		}
	}
}
void transmit_masked_channal(){
	uint8_t i = 0;
	for(i=0;i<tx_channal_num;i++){
		if(tx_mask && (1<<i)){
			uint8_t tmp_buff[package_size+2];
			itoa((tx_channal[i]),(char *)tmp_buff+1,10);
			int j;
			for(j=2 ; j < package_size+2; j++)
				if(tmp_buff[j] == 0){
					tmp_buff[j] = '\n';
					break;
				}
			j++;
			tmp_buff[0] = 'A' + i;
			USART_send(USART3,tmp_buff,j);
		}
	}
	USART_send(USART3,"W0\n",3);
}
void set_tx_mask(uint32_t val){
	tx_mask = val;
}
void load_tx_buffer(int16_t * p){
	uint8_t i = 0;
	for(i = 0; i < tx_channal_num;i++)
		tx_channal[i] = p[i];
}
void get_rx_buffer(int16_t * p){
	uint8_t i = 0;
	for(i = 0; i < rx_channal_num;i++)
		p[i] = rx_channal[i];
}
void add_to_channal(int16_t val, int16_t channal){
	rx_channal[channal]+=val;
}

