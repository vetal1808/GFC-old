#include "ESC_control.h"

#define min_pwm 1000
//#define max_pwm 2000

void ESC_init()
{
	/*
	 * PORT initialization. This code use timer2 and PA0-PA3
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	GPIO_InitTypeDef PORT;
	PORT.GPIO_Pin = (GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3);
	PORT.GPIO_Mode = GPIO_Mode_AF_PP;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &PORT);


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 72;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 3000;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);

	/*
	 * Timer was set on period 5ms and 1ms pulse width
	 * TIM_Pulse/2 = pulse width (us)
	 */


	TIM_OCInitTypeDef outputChannelInit = {0,};
	outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
	outputChannelInit.TIM_Pulse = 1000;
	outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
	outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM2, &outputChannelInit);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM2, &outputChannelInit);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM2, &outputChannelInit);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM2, &outputChannelInit);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
}

void ESC_set_power(uint16_t * val)
{
	TIM2->CCR1=val[0]+min_pwm;
	TIM2->CCR2=val[1]+min_pwm;
	TIM2->CCR3=val[2]+min_pwm;
	TIM2->CCR4=val[3]+min_pwm;
}
void update_rotors(rotor4 thrust4, uint8_t rotor_mask){
	if(!(rotor_mask & (1<<0)))
		thrust4.LFW = 0;
	if(!(rotor_mask & (1<<1)))
		thrust4.RFC = 0;
	if(!(rotor_mask & (1<<2)))
		thrust4.RBW = 0;
	if(!(rotor_mask & (1<<3)))
		thrust4.LBC = 0;	
	uint16_t tmp[4] = {thrust4.LFW, thrust4.RFC, thrust4.RBW, thrust4.LBC};
	ESC_set_power(tmp);
}

