/**
  ******************************************************************************
  * @file    USART/USART_RxTx/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-October-2010
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "easy_uart.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"


#define buff_size 256
volatile uint16_t rd_pos1 = 0, wr_pos1 = 0;
volatile uint8_t rx_buff1[buff_size];
volatile uint16_t rd_pos2 = 0, wr_pos2 = 0;
volatile uint8_t rx_buff2[buff_size];
volatile uint8_t line_counter2 = 0;



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(USART_TypeDef* USARTx);
void USART_Configuration(USART_TypeDef* USARTx);
void GPIO_Configuration(USART_TypeDef* USARTx);


/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles USARTx global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		rx_buff1[wr_pos1 % buff_size] = USART_ReceiveData(USART1);
		wr_pos1++;
		if((uint16_t)(wr_pos1 - rd_pos1) > buff_size) //overwriting received data
		  rd_pos1++;
	}	
}
void USART2_IRQHandler(void)
{
	if ((USART2->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		rx_buff2[wr_pos2 % buff_size] = USART_ReceiveData(USART2);
		wr_pos2++;
		if((uint16_t)(wr_pos2 - rd_pos2) > buff_size) //overwriting received data
		  rd_pos2++;
	}
}
uint16_t USART_available(USART_TypeDef* USARTx)
{
	if(USARTx == USART1)
		return (uint16_t)(wr_pos1 - rd_pos1);
	else
		if (USARTx == USART2) {
			return (uint16_t)(wr_pos2 - rd_pos2);
		}
	return 0;
}
uint8_t USART_line_available(USART_TypeDef* USARTx)
{
	uint16_t r, w;
	uint8_t * c;
	if (USARTx == USART1) {
		r= rd_pos1;
		w= wr_pos1;
		c = rx_buff1;
	}else if (USARTx == USART2) {
		r= rd_pos2;
		w= wr_pos2;
		c = rx_buff2;
	}
	for(;(uint16_t)(w-r)>0;r++){
		if (c[r%buff_size] == '\n')
			return 1;
	}
	return 0;
}
uint8_t USART_read(USART_TypeDef* USARTx, uint8_t * str, uint8_t len)
{
	uint8_t i = 0;
	if(USARTx == USART1)
	{
		while((uint16_t)(wr_pos1 - rd_pos1)>0 && i<len){
		      str[i] = rx_buff1[rd_pos1 % buff_size];
		      i++;
		      rd_pos1++;
		  }
  	}
  	else{
  		if (USARTx == USART2) {
  			while((uint16_t)(wr_pos2 - rd_pos2)>0 && i<len){
  			      str[i] = rx_buff2[rd_pos2 % buff_size];
  			      i++;
  			      rd_pos2++;
  			  }
  		}
  	}
	return i;
}
uint8_t USART_readLine(USART_TypeDef* USARTx, uint8_t * str, uint8_t len)
{
	uint8_t i = 0;
	if(USARTx == USART1)
	{
		while(wr_pos1>rd_pos1 && i<len){
		      str[i] = rx_buff1[rd_pos1 % buff_size];
		      i++;
		      rd_pos1++;
		      if(str[i-1] == '\n')
		              break;
		  }
  	}
  	else{
  		if (USARTx == USART2) {
  			while(wr_pos2>rd_pos2 && i<len){
  			      str[i] = rx_buff2[rd_pos2 % buff_size];
  			      i++;
  			      rd_pos2++;
  			      if(str[i-1] == '\n')
  			            break;
  			  }
  		}
  	}
	return i;
}


void USART_init(USART_TypeDef* USARTx)
{
	if (USARTx == USART1) {
		/* Enable USART1 and GPIOA clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if (USARTx == USART2) {
		/* Enable USART2 and GPIOA clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else {
		return;
	}

	/* NVIC Configuration */
	NVIC_Configuration(USARTx);


	/* Configure the GPIOs */
	GPIO_Configuration(USARTx);

	/* Configure the USARTx */
	USART_Configuration(USARTx);

	/* Enable the USARTx1 Receive interrupt: this interrupt is generated when the
		 USARTx receive data register is not empty */
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(USART_TypeDef* USARTx)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if (USARTx == USART1) {
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if (USARTx == USART2) {
		/* Configure USART1 Tx (PA.02) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure USART1 Rx (PA.03) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}


}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(USART_TypeDef* USARTx)
{
  USART_InitTypeDef USART_InitStructure;

/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USARTx, &USART_InitStructure);
    
  /* Enable USART1 */
  USART_Cmd(USARTx, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(USART_TypeDef* USARTx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the USARTx Interrupt */
	if (USARTx == USART1) {
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	}
	else if(USARTx == USART2){
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : USARTSend
* Description    : Send a string to the USART.
* Input          : - pucBuffer: buffers to be printed.
*                : - ulCount  : buffer's length
* Output         : None
* Return         : None
*******************************************************************************/
void USART_send(USART_TypeDef* USARTx, uint8_t *pucBuffer, uint8_t ulCount)
{
    //
    // Loop while there are more characters to send.
    //
//	pucBuffer--;
    while(ulCount--)
    {
        USART_SendData(USARTx, *pucBuffer++);// Last Version USART_SendData(USART1,(uint16_t) *pucBuffer++);
        /* Loop until the end of transmission */
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}
void USART_send_str(USART_TypeDef* USARTx, const uint8_t *pucBuffer, uint8_t ulCount)
{
    //
    // Loop while there are more characters to send.
    //
	pucBuffer--;
    while(ulCount--)
    {
        USART_SendData(USARTx, *pucBuffer++);// Last Version USART_SendData(USART1,(uint16_t) *pucBuffer++);
        /* Loop until the end of transmission */
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
