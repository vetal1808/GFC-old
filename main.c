

#include "stm32f10x.h"

uint32_t lol;

int main()
{

	init_timer();
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; //������������ ����� C
	GPIOC->CRH |= GPIO_CRH_MODE13_0;//����� PC8 ����� C - �����
	GPIOC->CRH &= ~GPIO_CRH_CNF13;//����� Push-Pull ��� ������ PC8 ����� C
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

