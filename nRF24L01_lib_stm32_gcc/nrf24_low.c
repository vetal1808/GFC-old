#include "stm32f10x_RCC.h"
#include "stm32f10x_GPIO.h"
#include "stm32f10x_SPI.h"

#include "nrf24_low.h"

void nRF24L01_HW_Init(void){
	GPIO_init();
  	SPIx_init();

	nRF24L01_CE(LOW);
	nRF24L01_NSS(HIGH);
}
void nRF24L01_Delay_us(unsigned long n){
	unsigned long i;

	while(n--)
	{
 	   i=2;
 	   while(i--);
  	}
}
void nRF24L01_CE(uint8_t level){
	if (level)
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
	else
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
void nRF24L01_NSS(uint8_t level){
	if (level)
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}
uint8_t nRF24L01_Read_Regs(uint8_t reg, uint8_t *buf, uint8_t len){
	uint8_t status;

  	nRF24L01_NSS(LOW);
  	status = nRF24L01_SPI_Send_Byte( R_REGISTER | ( REGISTER_MASK & reg ) );
  	while ( len-- )
    	*buf++ = nRF24L01_SPI_Send_Byte(0xff);
  	nRF24L01_NSS(HIGH);
  	
	return status;	
}
uint8_t nRF24L01_Write_Regs(uint8_t reg, uint8_t *buf, uint8_t len){
	uint8_t status;

	nRF24L01_NSS(LOW);
	status = nRF24L01_SPI_Send_Byte( W_REGISTER | ( REGISTER_MASK & reg ) );
	while ( len-- )
		nRF24L01_SPI_Send_Byte(*buf++);
	nRF24L01_NSS(HIGH);

	return status;
}
uint8_t nRF24L01_SPI_Send_Byte(uint8_t data){
  /* Loop while DR register in not empty */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI2, data);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return (uint8_t) SPI_I2S_ReceiveData(SPI2);
}

uint8_t nRF24L01_write_payload(uint8_t* buf, uint8_t len)
{
	uint8_t status;

	nRF24L01_NSS(LOW);
	status = nRF24L01_SPI_Send_Byte( W_TX_PAYLOAD );
	while ( len-- )
		nRF24L01_SPI_Send_Byte(*buf++);
	nRF24L01_NSS(HIGH);

  return status;
}

/****************************************************************************/

uint8_t nRF24L01_read_payload(uint8_t* buf, uint8_t len)
{
  uint8_t status;
  
  nRF24L01_NSS(LOW);
  status = nRF24L01_SPI_Send_Byte( R_RX_PAYLOAD );
  while ( len-- )
    *buf++ = nRF24L01_SPI_Send_Byte(0xff);
  nRF24L01_NSS(HIGH);

  return status;
}
void SPIx_init(){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_InitTypeDef  SPI_InitStructure;
	/* SPI2 configuration */
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High=ģʽ3��ʱ�ӿ���Ϊ�� //SPI_CPOL_Low=ģʽ0��ʱ�ӿ���Ϊ��
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPI_CPHA_2Edge;//SPI_CPHA_1Edge, SPI_CPHA_2Edge;
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Soft;//SPI_NSS_Hard
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//SPI_BaudRatePrescaler_2=18M;//SPI_BaudRatePrescaler_4=9MHz
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴӸ�λ��ʼ����
  	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	//Config the NSS pin
  	SPI_SSOutputCmd(SPI2, ENABLE);
	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);
}
void GPIO_init(){
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	/** Configure SPI2 pins: SCK, MISO and MOSI */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/**Configure CSN pin(SPI chip select)	*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/**Configure CE pin(nrf24 special pin)	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

}
