#ifndef __NRF24_LOW_H__
#define __NRF24_LOW_H__


#include <stdint.h>


/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF



#define LOW 0
#define HIGH 1

void nRF24L01_HW_Init(void);
void nRF24L01_Delay_us(unsigned long n);
void nRF24L01_CE(uint8_t level);
void nRF24L01_NSS(uint8_t level);
uint8_t nRF24L01_Read_Regs(uint8_t reg, uint8_t *pBuf, uint8_t len);
uint8_t nRF24L01_Write_Regs(uint8_t reg, uint8_t *pBuf, uint8_t len);
uint8_t nRF24L01_SPI_Send_Byte(uint8_t data);
uint8_t nRF24L01_read_payload(uint8_t* buf, uint8_t len);
uint8_t nRF24L01_write_payload(uint8_t* buf, uint8_t len);

#endif
