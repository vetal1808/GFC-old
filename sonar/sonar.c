#include "sonar.h"
#include "misc.h"

/*
 * A9 - echo
 * A10 - trig
 * */

#include "timer.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"

#define echo GPIO_Pin_11
#define trig GPIO_Pin_12
volatile uint16_t distanse = 0;
uint32_t sync_time_ = 0;

void init_gpio();
void init_external_interupt();


void init_sonar(){
	init_gpio();
	init_external_interupt();
}

void init_gpio(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	GPIO_InitTypeDef PORT;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;

	PORT.GPIO_Pin = echo;
	PORT.GPIO_Mode = GPIO_Mode_IPD;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &PORT);

	PORT.GPIO_Pin = trig;
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &PORT);

}
void init_external_interupt(){

	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE);


	AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI1_PA;
	EXTI_InitTypeDef  EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_EnableIRQ(EXTI15_10_IRQn);
}


void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & 1<<echo)
	{
		distanse = ((uint16_t)(micros() - sync_time_)*174)/1024;
		EXTI->PR =  1<<echo; // reset interrupt
	}
}
void sonar_start(){
	GPIO_SetBits(GPIOA, trig);
	delay_us(11);
	GPIO_ResetBits(GPIOA, trig);
}

void sonar_update(){
	const uint32_t period_us = 35000; //TODO may be sonar can work faster
	if ((micros() - sync_time_)>=period_us) {
		sonar_start();
		sync_time_ = micros();
	}
}
uint16_t get_sonar_distanse(){
	return distanse;
}
