#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void init_timer();
void delay_us(uint32_t us);
uint32_t micros();
void start_synchronization();
void synchronous_delay(uint32_t loop_time);
uint32_t loop_time();
#endif
