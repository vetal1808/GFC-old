

#include "stm32f10x.h"

uint32_t lol;

int main()
{

	init_timer();
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; //Тактирование порта C
	GPIOC->CRH |= GPIO_CRH_MODE13_0;//Вывод PC8 порта C - выход
	GPIOC->CRH &= ~GPIO_CRH_CNF13;//Режим Push-Pull для вывода PC8 порта C
	lol = micros();
	while(1)
	{
		if(micros()-lol>500000)
		{
			lol = micros();
			GPIOC->ODR ^= GPIO_ODR_ODR13;
		}
	}
}

