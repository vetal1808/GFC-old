#include "timer.h"

volatile uint16_t timer_overflow = 0;

void TIM4_IRQHandler(void) //Функция обработчика прерывания от таймера 4
{
    TIM4->SR &= ~TIM_SR_UIF; //Сбрасываем бит вызова прерывания.
    timer_overflow++;
}
uint32_t micros()
{
	NVIC_DisableIRQ(TIM4_IRQn);
	uint16_t count = TIM4->CNT;
	uint16_t ovf = timer_overflow;

	if(TIM4->SR & TIM_SR_UIF)//overflow happened just now
	{
//		TIM4->SR &= ~TIM_SR_UIF; //down interrupt flag
//		timer_overflow++;
		ovf++;
		count = 0;
	}
	NVIC_EnableIRQ(TIM4_IRQn);
	return (ovf<<16) | (count);
}
void init_timer()
{
	  NVIC_SetPriority(TIM4_IRQn, 1); //Приоритет прерывания
	  NVIC_EnableIRQ(TIM4_IRQn); //Разрешаем обработку прерывания от таймера 4
	  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;//Тактирование таймера TIM4
	  TIM4->PSC = 72;
	  TIM4->ARR = 0xFFFF;
	  TIM4->DIER |= TIM_DIER_UIE; //Разрешаем прерывание при переполнении счетчика
	  TIM4->CR1 |= TIM_CR1_CEN;//Запускаем счет
}
void delay_us(uint32_t us)
{
	uint32_t start = micros();
	while(micros()-start<us);
}

