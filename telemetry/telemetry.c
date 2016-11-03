#include "telemetry.h"

USART_TypeDef * USART;

#define rx_channal_num 6
#define tx_channal_num 6
#define package_size 6
#define max_val 10000
volatile int16_t rx_channal[rx_channal_num];

volatile int16_t tx_channal[tx_channal_num];
uint32_t tx_mask = 0;

void set_USARTn(USART_TypeDef* USARTx){
	USART = USARTx;
	USART_init(USARTx, 921600);
}

void receive_all_available(){
	if(USART_line_available(USART)){
		uint8_t tmp_buff[package_size+2];
		USART_readLine(USART, tmp_buff, package_size+2);
		tmp_buff[0] -= 'A';
		tmp_buff[package_size+1] = NULL;
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
			itoa((tx_channal[i]),tmp_buff+1,10);
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
void set_mask(uint32_t val){
	tx_mask = val;
}
void load_tx_buffer(int16_t * p){
	int i = 0;
	for(i = 0; i < tx_channal_num;i++)
		tx_channal[i] = p[i];
}
void get_rx_buffer(int16_t * p){
	int i = 0;
	for(i = 0; i < rx_channal_num;i++)
		p[i] = rx_channal[i];
}