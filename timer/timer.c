#include "timer.h"

volatile uint16_t timer_overflow = 0;

void TIM2_IRQHandler(void) //Функция обработчика прерывания от таймера 6
{
    TIM2->SR &= ~TIM_SR_UIF; //Сбрасываем бит вызова прерывания.
    timer_overflow++;
}
uint32_t micros()
{
	return (timer_overflow<<16) | (TIM2->CNT);
}
void init_timer()
{
	  NVIC_SetPriority(TIM2_IRQn, 1); //Приоритет прерывания
	  NVIC_EnableIRQ(TIM2_IRQn); //Разрешаем обработку прерывания от таймера 2
	  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;//Тактирование таймера TIM2
	  TIM2->PSC = 72;
	  TIM2->ARR = 0xFFFF;
	  TIM2->DIER |= TIM_DIER_UIE; //Разрешаем прерывание при переполнении счетчика
	  TIM2->CR1 |= TIM_CR1_CEN;//Запускаем счет
}
void delay_us(uint32_t us)
{
	uint32_t start = micros();
	while(micros()-start<us);
}

