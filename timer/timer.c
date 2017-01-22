#include "timer.h"

volatile uint16_t timer_overflow = 0;
volatile uint32_t sync_time = 0;

void TIM4_IRQHandler(void) //������� ����������� ���������� �� ������� 4
{
    TIM4->SR &= ~TIM_SR_UIF; //���������� ��� ������ ����������.
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
	  NVIC_SetPriority(TIM4_IRQn, 1); //��������� ����������
	  NVIC_EnableIRQ(TIM4_IRQn); //��������� ��������� ���������� �� ������� 4
	  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;//������������ ������� TIM4
	  TIM4->PSC = 71;
	  TIM4->ARR = 0xFFFF;
	  TIM4->DIER |= TIM_DIER_UIE; //��������� ���������� ��� ������������ ��������
	  TIM4->CR1 |= TIM_CR1_CEN;//��������� ����
}
void delay_us(uint32_t us)
{
	uint32_t start = micros();
	while(micros()-start<us);
}
void start_synchronization(){
	sync_time = micros();
}
void synchronous_delay(uint32_t loop_time){
	while (loop_time > (micros()-sync_time));
	sync_time += loop_time;
}
uint32_t loop_time(){
	return micros()-sync_time;
}

