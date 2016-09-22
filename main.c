

#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"
#include "mpu6050.h"
#include "MadgwickAHRS.h"
#include "timer.h"
#include "BMP085_timer.h"
#include "gps_parser.h"



int main()
{
	USART_init(USART2);
	while(1)
	{
		gps_update();
	}
	/*
	init_timer();
	uint8_t buffer1[64];
	uint8_t buffer2[64];
	USART_init(USART1);
	USART_init(USART2);
	while(1)
		{
			//delay_us(1000000);
			uint8_t i = USART_available(USART2);
			if(i)
			{
				USART_read(USART2, buffer1,i);
				USART_send(USART1, buffer1,i);
			}
			i = USART_available(USART1);
			if(i)
			{
				USART_read(USART1, buffer2,i);
				USART_send(USART2, buffer2,i);
			}
		}
		*/
	/*

	USART_init(USART1);
	USART_init(USART2);
	const uint8_t str [] = "Hello world!\n";
	USART_send_str(USART2, str,14);

	while(1)
	{
		uint8_t i = USART_available(USART2);
		if(i)
		{
			USART_read(USART2, buffer,i);
			USART_send_str(USART1, buffer,14);
		}
	}
	*/
}

