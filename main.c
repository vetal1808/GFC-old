

#include "stm32f10x.h"
#include "stdint.h"
#include "easy_uart.h"

int main()
{
	usart_init();
	char rx [5];
	char str [] = "Hello world!\r\n";
	UARTSend(str, sizeof(str));
	while(1)
	{
		uint8_t l = USART1_available();
		if(l)
		{
			uint8_t i = USART1_read(rx, 5);
			i = i;
		}

	}
}

