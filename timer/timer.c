#include "timer.h"

volatile uint16_t timer_overflow = 0;

void TIM4_IRQHandler(void) //������� ����������� ���������� �� ������� 6
{
    TIM4->SR &= ~TIM_SR_UIF; //���������� ��� ������ ����������.
    timer_overflow++;
}
uint32_t micros()
{
	return (timer_overflow<<16) | (TIM4->CNT);
}
void init_timer()
{
	  NVIC_SetPriority(TIM4_IRQn, 1); //��������� ����������
	  NVIC_EnableIRQ(TIM4_IRQn); //��������� ��������� ���������� �� ������� 2
	  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;//������������ ������� TIM4
	  TIM4->PSC = 72;
	  TIM4->ARR = 0xFFFF;
	  TIM4->DIER |= TIM_DIER_UIE; //��������� ���������� ��� ������������ ��������
	  TIM4->CR1 |= TIM_CR1_CEN;//��������� ����
}
void delay_us(uint32_t us)
{
	uint32_t start = micros();
	while(micros()-start<us);
}

